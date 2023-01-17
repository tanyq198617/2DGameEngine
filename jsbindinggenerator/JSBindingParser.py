#!/usr/bin/python

import sys
import clang.cindex, re
import argparse
import shlex
import os

propparamparser = argparse.ArgumentParser(description='JS_BIND property parser')
propparamparser.add_argument('--getter', nargs='?', help='cpp getter function')
propparamparser.add_argument('--setter', nargs='?', help='cpp setter function')
propparamparser.add_argument('--funcname', nargs='?', help='js function name')
propparamparser.add_argument('--name', nargs='?', help='js property name')
propparamparser.add_argument('--readonly', action='store_true')

funcparamparser = argparse.ArgumentParser(description='JS_BIND function parser')
funcparamparser.add_argument('--funcname', nargs='?', help='js function name')

path, _ = os.path.split(os.path.realpath(__file__))
clang.cindex.Config.set_library_path(os.path.join(path, 'clang', 'lib'))

classes = []

number_types = set([
	clang.cindex.TypeKind.CHAR_U,
	clang.cindex.TypeKind.UCHAR,
	clang.cindex.TypeKind.USHORT,
	clang.cindex.TypeKind.UINT,
	clang.cindex.TypeKind.ULONG,
	clang.cindex.TypeKind.ULONGLONG,
	clang.cindex.TypeKind.CHAR_S,
	clang.cindex.TypeKind.SCHAR,
	clang.cindex.TypeKind.WCHAR,
	clang.cindex.TypeKind.SHORT,
	clang.cindex.TypeKind.INT,
	clang.cindex.TypeKind.LONG,
	clang.cindex.TypeKind.LONGLONG,
	clang.cindex.TypeKind.FLOAT,
	clang.cindex.TypeKind.DOUBLE
])

class CPPType:
	def __init__(self, ctype):
		self.originalctype = ctype
		self.ctype = ctype.get_canonical()
		if self.ctype.kind == clang.cindex.TypeKind.LVALUEREFERENCE:
			self.ctype = self.ctype.get_pointee()
		self.templateArgs = []
		argsnum = self.originalctype.get_num_template_arguments()
		if argsnum > 0:
			self._isTemplate = True
			for i in range(argsnum):
				self.templateArgs.append(CPPType(self.originalctype.get_template_argument_as_type(i)))
		else:
			argsnum = self.ctype.get_num_template_arguments()
			if argsnum > 0:
				self._isTemplate = True
				for i in range(argsnum):
					self.templateArgs.append(CPPType(self.ctype.get_template_argument_as_type(i)))
			else:
				self._isTemplate = False
				
		if ctype.kind == clang.cindex.TypeKind.TYPEDEF:
			ctype = ctype.get_declaration().underlying_typedef_type
		ret = re.findall('<(.*)\\s?>', ctype.spelling)
		self.templateArgNames = []
		if len(ret) == 1:
			openNum = 0
			closeNum = 0
			tmp = ''
			for item in ret[0].split(', '):
				lNum = item.count('<')
				rNum = item.count('>')
				openNum += lNum
				closeNum += rNum
				if len(tmp):
					tmp += ', ' + item
				else:
					tmp = item
				if openNum == closeNum:
					self.templateArgNames.append(tmp.strip())
					tmp = ''
					openNum = 0
					closeNum = 0
			
	def __str__(self):
		return self.spelling
		
	def __repr__(self):
		return "<CPPType %s>"%self.spelling
		
	def isPointer(self):
		return self.ctype.kind == clang.cindex.TypeKind.POINTER
		
	def isVoid(self):
		return self.ctype.kind == clang.cindex.TypeKind.VOID
		
	def isBool(self):
		return self.ctype.kind == clang.cindex.TypeKind.BOOL
		
	def isEnum(self):
		return self.ctype.kind == clang.cindex.TypeKind.ENUM
		
	def isNumber(self):
		return self.ctype.kind in number_types
		
	def isRecord(self):
		return self.ctype.kind == clang.cindex.TypeKind.RECORD
		
	def isTemplate(self):
		return self._isTemplate
		
	def getTemplateArgs(self):
		if self._isTemplate:
			return self.templateArgs
		return None
		
	def isTypedef(self):
		return self.originalctype.kind == clang.cindex.TypeKind.TYPEDEF
		
	def isString(self):
		if self.isPointer():
			return self.ctype.get_pointee().kind == clang.cindex.TypeKind.CHAR_S
		elif self.isTemplate:
			return self.originalctype.spelling == 'std::string'
		
	def isLValueReference(self):
		return self.originalctype.kind == clang.cindex.TypeKind.LVALUEREFERENCE
		
	def isVector(self):
		if not self._isTemplate:
			return False
		ctype = self.originalctype
		if self.originalctype.kind == clang.cindex.TypeKind.LVALUEREFERENCE:
			ctype = self.originalctype.get_pointee()
		if ctype.kind == clang.cindex.TypeKind.TYPEDEF:
			typeseplling = ctype.get_declaration().underlying_typedef_type.spelling
			return typeseplling.startswith('std::vector') or typeseplling.startswith('const std::vector')
		else:
			typeseplling = ctype.spelling
			return typeseplling.startswith('std::vector') or typeseplling.startswith('const std::vector')
	
	def isArray(self):
		return self.ctype.kind == clang.cindex.TypeKind.CONSTANTARRAY
	
	def isList(self):
		if not self._isTemplate:
			return False
		ctype = self.originalctype
		if self.originalctype.kind == clang.cindex.TypeKind.LVALUEREFERENCE:
			ctype = self.originalctype.get_pointee()
		if ctype.kind == clang.cindex.TypeKind.TYPEDEF:
			typeseplling = ctype.get_declaration().underlying_typedef_type.spelling
			return typeseplling.startswith('std::list') or typeseplling.startswith('const std::list')
		else:
			typeseplling = ctype.spelling
			return typeseplling.startswith('std::list') or typeseplling.startswith('const std::list')
		
	@property
	def spelling(self):
		if self.isEnum():
			spelling = self.originalctype.spelling
			if '::' in spelling:
				spelling = spelling.split('::')[-1]
			ns = ''
			node = self.ctype.get_declaration()
			while node.semantic_parent.kind == clang.cindex.CursorKind.NAMESPACE or node.semantic_parent.kind == clang.cindex.CursorKind.CLASS_DECL:
				ns = node.semantic_parent.spelling + '::' + ns
				node = node.semantic_parent
			return ns + spelling
		return self.originalctype.spelling
	
	@property
	def spellingNoPointer(self):
		spelling = None
		if self.isPointer():
			spelling = self.originalctype.get_pointee().spelling
		else:
			spelling = self.originalctype.spelling
		if spelling.startswith('const '):
			return spelling[6:]
		return spelling
	
	@property
	def spellingNoReference(self):
		spelling = None
		if self.isLValueReference():
			spelling = self.originalctype.get_pointee().spelling
		else:
			spelling = self.originalctype.spelling
		if spelling.startswith('const '):
			return spelling[6:]
		return spelling
	
	@property
	def canonical_spelling(self):
		return self.ctype.spelling

class CPPVariable:
	def __init__(self, node):
		self.name = node.spelling
		self.default = None
		self.type = CPPType(node.type)
		self.cnode = node
		tokens = node.get_tokens()
		try:
			while True:
				token = tokens.next()
				if token.spelling == '=' or token.spelling == '{':
					self.default = tokens.next().spelling
					if self.default == '-':
						self.default = self.default + tokens.next().spelling
					break
		except StopIteration:
			pass
			
	def __str__(self):
		return self.name
		
	def __repr__(self):
		if self.default:
			return "<CPPVariable %s default=%s>"%(self.name, self.default)
		return "<CPPVariable %s>"%self.name
	
class CPPField:
	def __init__(self, node, params, static = False):
		self.static = static
		self.var = CPPVariable(node)
		self.name = self.var.name
		self.genType = params[0]
		if len(self.genType) == 0:
			self.genType = 'PROP'
		self.cnode = node
		if self.genType != 'PROP' and self.genType != 'SETTER' and self.genType != 'GETTER' and self.genType != 'DELEGATEADD' and self.genType != 'DELEGATERM':
			raise Exception('Invalid property parameter')
		self.args = propparamparser.parse_args(shlex.split(params[1]))
	
	def __str__(self):
		return self.name
		
	def __repr__(self):
		return "<CPPField %s>"%self.name
	
class CPPMethod:
	def __init__(self, node, params):
		self.name = node.spelling
		self.cnode = node
		self.static = node.is_static_method()
		self.returnType = CPPType(node.result_type)
		self.arguments = []
		self.isConst = node.is_const_method()
		for arg in node.get_arguments():
			self.arguments.append(CPPVariable(arg))
		args = funcparamparser.parse_args(shlex.split(params))
		if args.funcname:
			self.funcname = args.funcname
		else:
			self.funcname = None
	
	def __str__(self):
		return self.cnode.displayname
		
	def __repr__(self):
		return "<CPPMethod %s>"%self.cnode.displayname
	
class CPPClass:
	def __init__(self, node, params):
#		print node.spelling, node.type.spelling, params
		self.funcs = []
		self.staticFuncs = []
		self.props = []
		self.staticProps = []
		self.ctors = []
		self.superClasses = []
		self.name = node.spelling
		self.file = node.location.file.name
		if len(params[0]) > 0:
			self.singleton = True
			self.singletonFunc = None
			self.singletonFuncName = params[1]
			self.jsclsname = params[2]
		else:
			self.singleton = False
			self.jsclsname = params[1]
		if len(self.jsclsname) == 0:
			self.jsclsname = node.spelling
			
		self.namespaces = node.type.spelling.split('::')[:-1]
		
		for c in node.get_children():
			if c.kind == clang.cindex.CursorKind.CXX_METHOD and c.access_specifier == clang.cindex.AccessSpecifier.PUBLIC:
				self._parseMethod(c)
			elif c.kind == clang.cindex.CursorKind.CONSTRUCTOR and c.access_specifier == clang.cindex.AccessSpecifier.PUBLIC:
				if c.raw_comment:
					self._parseConstructor(c)
			elif c.kind == clang.cindex.CursorKind.FIELD_DECL and c.raw_comment:
				self._parseField(c)
			elif c.kind == clang.cindex.CursorKind.VAR_DECL and c.raw_comment:
				self._parseStaticField(c)
			elif c.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
				self.superClasses.append(c)
						
		if self.singleton and not self.singletonFunc:
			if len(self.superClasses) > 0:
				for superClass in self.superClasses:
					for c in superClass.get_children():
						if c.kind == clang.cindex.CursorKind.TEMPLATE_REF:
							for c1 in c.referenced.get_children():
								if c1.is_static_method() and c1.spelling == self.singletonFuncName:
									self.singletonFunc = CPPMethod(c1, '')
									break;
					if self.singletonFunc:
						break;
					for c in superClass.type.get_canonical().get_declaration().get_children():
						if c.kind == clang.cindex.CursorKind.CXX_METHOD and c.access_specifier == clang.cindex.AccessSpecifier.PUBLIC:
							if c.is_static_method() and c.spelling == self.singletonFuncName:
								self.singletonFunc = CPPMethod(c, '')
								break;
					if self.singletonFunc:
						break;
			if not self.singletonFunc:
				raise Exception('Cannot find singleton function ' + self.singletonFuncName)
			
	def _parseConstructor(self, node):
		result = re.findall(r'///JS_BIND[ \f\t\v]*(.*)', node.raw_comment)
		for v in result:
			self.ctors.append(CPPMethod(node, v))
			
	def _parseField(self, node):
		result = re.findall(r'///JS_BIND[ \f\t\v]*([A-Z]*)(.*)', node.raw_comment)
		for v in result:
			self.props.append(CPPField(node, v))
			
	def _parseStaticField(self, node):
		result = re.findall(r'///JS_BIND[ \f\t\v]*([A-Z]*)(.*)', node.raw_comment)
		for v in result:
			self.staticProps.append(CPPField(node, v, True))
			
	def _parseMethod(self, node):
		if node.is_static_method() and self.singleton and node.spelling == self.singletonFuncName:
			self.singletonFunc = CPPMethod(node, '')
		elif node.raw_comment:
			result = re.findall(r'///JS_BIND[ \f\t\v]*(.*)', node.raw_comment)
			for v in result:
				func = CPPMethod(node, v)
				name = func.name
				if node.is_static_method():
					self.staticFuncs.append(func)
				else:
					self.funcs.append(func)
					
	def __str__(self):
		return self.name
		
	def __repr__(self):
		return "<CPPClass %s>"%self.name
		
	
projectPath = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
	
def _getClassesInFile(node, file, inCurrentFile):
	if inCurrentFile and (node.spelling != file and node.location.file and node.location.file.name != file):
		return
	if (not inCurrentFile) and (node.spelling != file and node.location.file and (not node.location.file.name.startswith(projectPath) or node.location.file.name.startswith(os.path.join(projectPath, "jsbindinggenerator")))):
		return
	if node.kind == clang.cindex.CursorKind.CLASS_DECL and node.is_definition() and node.raw_comment:
		result = re.findall(r'///JS_BIND(_SINGLETON)?\s*([0-9a-zA-Z]*)\s*([0-9a-zA-Z]*)', node.raw_comment)
		if len(result) > 0:
			if len(result) > 1:
				print 'warning: more then one name binding specified to the class, using the first one'
			classes.append(CPPClass(node, result.pop(0)))
	for c in node.get_children():
		_getClassesInFile(c, file, inCurrentFile)
			
	
def parseCPPHeader(path, headerPaths = None, preprocessorMacros = None, inCurrentFile = True):
	index = clang.cindex.Index(clang.cindex.conf.lib.clang_createIndex(False, True))
	args = ['-x', 'c++', '-std=c++11', '-DTARGET_MACOS', '-DGEN_BINDING', '--sysroot=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk', '-framework', 'OpenGL']
	if headerPaths:
		for p in headerPaths:
			args.append('-I' + p)
	if preprocessorMacros:
		for m in preprocessorMacros:
			args.append('-D' + m)
	tu = index.parse(path, args)
	if tu.diagnostics:
		for d in tu.diagnostics:
			if d.severity > clang.cindex.Diagnostic.Warning:
				raise Exception('Error parsing file ' + path)
	if not tu.cursor:
		raise Exception('Error parsing file ' + path)
		
	global classes
	classes = []
	_getClassesInFile(tu.cursor, path, inCurrentFile)
	return classes
	
#parseCPPHeader('/Users/hoolai/HLEngine.proj/src/gui/RichLabel/HLRichLabel.h', ['/Users/hoolai/HLEngine.proj/src/common/HLFont', '/Users/hoolai/HLEngine.proj/libs/OpenThreads/include', '/Users/hoolai/HLEngine.proj/libs/kazmath/include', '/Users/hoolai/HLEngine.proj/libs/spine-c/include', '/Users/hoolai/HLEngine.proj/src', '/Users/hoolai/HLEngine.proj/src/application', '/Users/hoolai/HLEngine.proj/src/base', '/Users/hoolai/HLEngine.proj/src/common', '/Users/hoolai/HLEngine.proj/src/core', '/Users/hoolai/HLEngine.proj/src/core/utils', '/Users/hoolai/HLEngine.proj/src/core/eventdispatcher', '/Users/hoolai/HLEngine.proj/src/glhelpers', '/Users/hoolai/HLEngine.proj/src/gui', '/Users/hoolai/HLEngine.proj/src/jsscripting', '/Users/hoolai/HLEngine.proj/src/jsscripting/jswrappers', '/Users/hoolai/HLEngine.proj/src/textures'])