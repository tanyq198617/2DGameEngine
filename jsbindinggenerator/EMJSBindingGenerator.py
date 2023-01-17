#coding=utf-8
#!/usr/bin/python

import os
import sys
import re
from JSBindingParser import parseCPPHeader
import argparse
import shlex
import json

classesmap = {}
mannualcode = {}
enumsmap = {}

def JSValueTypeCheckCode(varname, type, builder):
	if type.isNumber() or type.isEnum():
		return '(typeof(%s) === "number")'%(varname, )
	elif type.isString():
		return '(typeof(%s) === "string")'%(varname, )
	elif type.isVector() or type.isList():
		return '(%s instanceof Array)'%(varname, )
	elif type.isPointer() and findClassInfo(type.ctype.get_pointee().spelling):
		info = findClassInfo(type.ctype.get_pointee().spelling)
		return '(%s == null || %s == undefined || %s instanceof %s)'%(varname, varname, varname, info['wrapperclass'])
	return None

def printJSValueToValueCode(printer, template, jsValue, type, builder):
	if type.isEnum():
		enumInfo = enumsmap[type.spelling]
		if not enumInfo:
			raise Exception('Enum not defined')
		printer.printline(template%('Module.%s.values[%s]'%(enumInfo['binding'], jsValue), ))
	elif type.spelling == 'std::string' or type.spelling == 'const std::string &':
		printer.printline(template%('strToUTF8Arr(%s)'%jsValue, ))
	elif type.isPointer() and type.ctype.get_pointee().spelling in classesmap:
		printer.printline(template%('%s ? %s.nativeInstance : null'%(jsValue, jsValue), ))
	elif type.isVector():
		printer.printline('let vec = new Module.%sVector();'%(type.getTemplateArgs()[0].spelling.replace('::', '_')))
		printer.printline('for (let v of %s) {'%jsValue)
		printer.indent()
		printJSValueToValueCode(printer, 'vec.push_back(%s);', 'v', type.getTemplateArgs()[0], builder)
		printer.outdent()
		printer.printline('}')
		printer.printline(template%'vec')
		return 'vec.delete();'
	elif type.isList():
		raise Exception('to be implemented')
	else:
		printer.printline(template%jsValue)
	return None

def printValueToJSValueCode(printer, template, accessor, type, builder):
	if type.isEnum():
		enumInfo = enumsmap[type.spelling]
		if not enumInfo:
			raise Exception('Enum not defined')
		printer.printline(template%(accessor + '.value', ))
	elif type.isPointer() and type.ctype.get_pointee().spelling in classesmap:
		printer.printline('let value = %s;'%accessor)
		printer.printline(template%'value ? value.getWrapper() : null;')
	elif type.isVector():
			raise Exception('to be implemented')
	elif type.isList():
		print type.spelling
		raise Exception('to be implemented')
	else:
		printer.printline(template%accessor)

def convertToJSDefaultValue(type, value):
	if value == 'nullptr' or value == 'NULL' or value == 'emscripten::val::null()':
		return 'null'
	elif type.isEnum():
		enumInfo = enumsmap[type.spelling]
		return 'Module.%s.%s'%(enumInfo['binding'], value)
	if value == 'HLRectZero':
		return '{x:0, y:0, width:0, height:0}'
	return value

def processCodeLines(code):
	lines = code.split('\n')
	newlines = []
	stripnum = 100
	for line in lines:
		num = len(line) - len(line.lstrip(' '))
		if num != len(line) and stripnum > num:
			stripnum = num
	for line in lines:
		newlines.append(line[stripnum:])
	return '\n'.join(newlines)

def findClassInfo(className):
	if className in classesmap:
		return classesmap[className]
	return None
	
def findEnumInfo(enumName):
	if enumName in enumsmap:
		return enumsmap[enumName]
	return None

class Printer:
	def __init__(self):
		self._indent = ''
		self._out = ''
	
	def indent(self):
		self._indent += '    '
		
	def outdent(self):
		self._indent = self._indent[4:]
		
	def printline(self, str):
		if len(self._indent) > 0:
			str = str.replace('\n', '\n'+self._indent)
		self._out += self._indent + str + '\n';
		
	def printlines(self, *lines):
		for line in lines:
			if len(self._indent) > 0:
				str = str.replace('\n', '\n'+self._indent)
			self._out += self._indent + line + '\n';

class JSFuncBase:
	def __init__(self, cppMethod, builder):
		self.parameters = []
		self.minParamNumber = 0
		for param in cppMethod.arguments:
			if not param.default:
				self.minParamNumber += 1
			self.parameters.append(param)
		self.paramNumber = len(self.parameters)
		self.priority = 100
		
	def canGenJSCode(self):
		return True
		
	def _genParamsCode(self, printer, builder):
		if self.minParamNumber != self.paramNumber:
			if self.minParamNumber == 0:
				printer.printline('if (argumentCount <= %d)\n{'%self.paramNumber)
			else:
				printer.printline('if (argumentCount >= %d && argumentCount <= %d)\n{'%(self.minParamNumber, self.paramNumber))
		else:
			printer.printline('if (argumentCount == %d)\n{'%self.minParamNumber)
		printer.indent()
		argnames = []
		disposecode = []
		for param in self.parameters:
			argnames.append(param.name)
			if param.default:
				printer.printline('let %s = %s;'%(param.name, convertToJSDefaultValue(param.type, param.default)))
			else:
				printer.printline('let %s;'%(param.name, ))
				
		for i in range(0, self.minParamNumber):
			checkCode = JSValueTypeCheckCode('arguments[%s]'%i, self.parameters[i].type, builder)
			if checkCode is not None:
				printer.printline('if (!%s)'%checkCode)
				printer.indent()
				printer.printline('break;')
				printer.outdent()
			ret = printJSValueToValueCode(printer, self.parameters[i].name + ' = %s;', 'arguments[%d]'%i, self.parameters[i].type, builder)
			if ret:
				disposecode.append(ret)
		
		for i in range(self.minParamNumber, self.paramNumber):
			printer.printline('if (argumentCount > %d)\n{'%i)
			printer.indent()
			checkCode = JSValueTypeCheckCode('arguments[%s]'%i, self.parameters[i].type, builder)
			if checkCode is not None:
				printer.printline('if (!%s)'%checkCode)
				printer.indent()
				printer.printline('break;')
				printer.outdent()
			ret = printJSValueToValueCode(printer, self.parameters[i].name + ' = %s;', 'arguments[%d]'%i, self.parameters[i].type, builder)
			if ret:
				disposecode.append(ret)
		
		for i in range(self.minParamNumber, self.paramNumber):
			printer.outdent()
			printer.printline('}')
			
		return argnames, disposecode

class JSCtor(JSFuncBase):
	def __init__(self, cppMethod, builder):
		JSFuncBase.__init__(self, cppMethod, builder)
		self.name = 'ctor'
	
	def genBinding(self, printer, printer1, builder):
		if self.paramNumber > 0:
			args = []
			for param in self.parameters:
				args.append(param.type.spelling)
			printer1.printline('.constructor<%s>()'%(', '.join(args), ))
		else:
			printer1.printline('.constructor()')
			
	def genWrapper(self, printer, builder):
		if self.paramNumber > 0:
			printer.printline('do {')
			printer.indent();
			argnames, disposecodes = self._genParamsCode(printer, builder)
			if builder.parentClassInfo:
				printer.printline('let nativeInstance = new Module.%s(%s);'%('_'.join(builder.namespaces)+'_'+builder.jsclsname, ', '.join(argnames)))
				printer.printline('super(nativeInstance);')
				printer.printline('nativeInstance.registWrapper(this);')
			else:
				printer.printline('this.nativeInstance = new Module.%s(%s);'%('_'.join(builder.namespaces)+'_'+builder.jsclsname, ', '.join(argnames)))
				printer.printline('this.nativeInstance.registWrapper(this);')
			for c in disposecodes:
				printer.printline(c)
			printer.printline('return;')
			printer.outdent()
			printer.printline('}')
			printer.outdent();
			printer.printline('} while(0);')
			return True
		else:
			if builder.parentClassInfo:
				printer.printline('let nativeInstance = new Module.%s();'%('_'.join(builder.namespaces) + '_' + builder.jsclsname, ))
				printer.printline('super(nativeInstance);')
				printer.printline('nativeInstance.registWrapper(this);')
				printer.printline('return;')
			else:
				printer.printline('this.nativeInstance = new Module.%s();'%('_'.join(builder.namespaces) + '_' + builder.jsclsname, ))
				printer.printline('this.nativeInstance.registWrapper(this);')
				printer.printline('return;')
			return False

class JSProp:
	def __init__(self, cppField):
		self.readonly = cppField.args.readonly
		self.name = cppField.name
		self.cppname = cppField.name
		self.static = cppField.static
		self.cppGetterFunc = None
		self.cppSetterFunc = None
		if cppField.args.name:
			self.name = cppField.args.name
		if cppField.args.getter:
			self.cppGetterFunc = cppField.args.getter
		if cppField.args.setter:
			self.cppSetterFunc = cppField.args.setter
		self.valueType = cppField.var.type
		
	def genEmbindSource(self, printer, printer1, builder):
		if self.valueType.isPointer():
			funcname = 'class_function' if self.static  else 'function'
			if self.cppGetterFunc:
				getterFunc = builder.namespace+builder.cppclsname+'::'+self.cppGetterFunc
				printer1.printline('.%s("get%s", &%s, allow_raw_pointers())'%(funcname, self.name.capitalize(), getterFunc))
				if not self.readonly:
					setterFunc = builder.namespace+builder.cppclsname+'::'+self.cppSetterFunc
					printer1.printline('.%s("set%s", &%s, allow_raw_pointers())'%(funcname, self.name.capitalize(), setterFunc))
			else:
				funcname = '_'.join(builder.namespaces) + '_' + builder.cppclsname + '_' + self.name
				if self.static:
					printer.printline('%s %s_getter()'%(self.valueType.spelling, funcname))
				else:
					printer.printline('%s %s_getter(%s* instance)'%(self.valueType.spelling, funcname, builder.namespace+builder.cppclsname))
				printer.printline('{')
				printer.indent()
				if self.static:
					printer.printline('return %s::%s;'%(builder.namespace+builder.cppclsname, self.cppname))
				else:
					printer.printline('return instance->%s;'%(self.cppname, ))
				printer.outdent()
				printer.printline('}\n')
				printer1.printline('.function("get%s", &%s_getter, allow_raw_pointers())'%(self.name.capitalize(), funcname))
				if not self.readonly:
					if self.static:
						printer.printline('void %s_setter(%s value)'%(funcname, self.valueType.spelling))
					else:
						printer.printline('void %s_setter(%s* instance, %s value)'%(funcname, builder.namespace+builder.cppclsname, self.valueType.spelling))
					printer.printline('{')
					printer.indent()
					if self.static:
						printer.printline('%s::%s = value;'%(builder.namespace+builder.cppclsname, self.cppname))
					else:
						printer.printline('instance->%s = value;'%(self.cppname, ))
					printer.outdent()
					printer.printline('}\n')
					printer1.printline('.function("set%s", &%s_setter, allow_raw_pointers())'%(self.name.capitalize(), funcname))
					
		else:
			funcname = 'class_property' if self.static else 'property'
			if self.cppGetterFunc:
				if self.readonly:
					printer1.printline('.%s("%s", &%s::%s)'%(funcname, self.name, builder.namespace+builder.cppclsname, self.cppGetterFunc))
				else:
					printer1.printline('.%s("%s", &%s::%s, select_overload<void(%s)>(&%s::%s))'%(funcname, self.name, builder.namespace+builder.cppclsname, self.cppGetterFunc, self.valueType.spelling, builder.namespace+builder.cppclsname, self.cppSetterFunc))
			else:
				printer1.printline('.%s("%s", &%s::%s)'%(funcname, self.name, builder.namespace+builder.cppclsname, self.cppname))
				
	def genWrapper(self, printer, builder):
		if self.valueType.isPointer():
			printer.printline(('static ' if self.static else '') + 'get ' + self.name + '() {')
			printer.indent()
			printValueToJSValueCode(printer, 'return %s;', 'this.nativeInstance.get'+ self.name.capitalize()+'();', self.valueType, builder)
			printer.outdent()
			printer.printline('}')
			if not self.readonly:
				printer.printline(('static ' if self.static else '') + 'set ' + self.name + '(value) {')
				printer.indent()
				ret = printJSValueToValueCode(printer, 'this.nativeInstance.set%s(%%s);'%(self.name.capitalize(), ), 'value', self.valueType, builder)
				if ret:
					printer.printline(ret)
				printer.outdent()
				printer.printline('}')
		else:
			printer.printline(('static ' if self.static else '') + 'get ' + self.name + '() {')
			printer.indent()
			printValueToJSValueCode(printer, 'return %s;', 'this.nativeInstance.' + self.name, self.valueType, builder)
			printer.outdent()
			printer.printline('}')
			if not self.readonly:
				printer.printline(('static ' if self.static else '') + 'set ' + self.name + '(value) {')
				printer.indent()
				ret = printJSValueToValueCode(printer, 'this.nativeInstance.' + self.name + ' = %s;', 'value', self.valueType, builder)
				if ret:
					printer.printline(ret)
				printer.outdent()
				printer.printline('}')
		
class JSFunc(JSFuncBase):
	def __init__(self, cppMethod, builder):
		JSFuncBase.__init__(self, cppMethod, builder)
		self.name = cppMethod.name
		self.originName = self.name
		if cppMethod.funcname:
			self.name = cppMethod.funcname
		self.static = cppMethod.static
		self.returnType = cppMethod.returnType
		paramTypes = []
		self.rawPointer = False;
		for param in self.parameters:
			paramTypes.append(param.type.spelling)
			if param.type.isPointer():
				self.rawPointer = True
#		self.funcType = cppMethod.cnode.type.spelling
		if self.returnType.isVoid():
			self.funcType = 'void(' + ', '.join(paramTypes) + ')'
		else:
			self.funcType = self.returnType.spelling + '(' + ', '.join(paramTypes) + ')'
			if self.returnType.isPointer():
				self.rawPointer = True
		if cppMethod.isConst:
			self.funcType += 'const'
	
	def genBinding(self, printer, printer1, builder, index):
		fullclsname = builder.namespace + builder.cppclsname
		if fullclsname in mannualcode:
			if self.name in mannualcode[fullclsname]:
				info = mannualcode[fullclsname][self.name]
				code = info['cppfunc']
				if code != None:
					if len(code) == 0:
						return
					printer.printline('//!binding_%s::%s'%(fullclsname, self.name))
					printer.printline(code)
					printer.printline('//!binding_%s::%s_end\n'%(fullclsname, self.name))
					if self.static:
						printer1.printline('.class_function("%s", %s, allow_raw_pointers())'%(self.name, info['cppfuncname']))
					else:
						printer1.printline('.function("%s", %s, allow_raw_pointers())'%(self.name, info['cppfuncname']))
					return
					
		outstr = '.'
		if self.static:
			outstr += 'class_function('
		else:
			outstr += 'function('
		if index > 0:
			outstr = outstr + ('"%s%d", select_overload<%s>(&%s::%s)'%(self.name, index, self.funcType, builder.namespace+builder.cppclsname, self.originName))
		else:
			outstr = outstr + ('"%s", select_overload<%s>(&%s::%s)'%(self.name, self.funcType, builder.namespace+builder.cppclsname, self.originName))
		if self.rawPointer:
			outstr += ', allow_raw_pointers()'
		outstr += ')'
		printer1.printline(outstr)
		
	def genWrapper(self, printer, builder, index):
		fullclsname = builder.namespace + builder.cppclsname
		if fullclsname in mannualcode:
			if self.name in mannualcode[fullclsname]:
				info = mannualcode[fullclsname][self.name]
				code = info['js']
				printer.printline('//!wrapper_%s::%s'%(fullclsname, self.name))
				printer.printline(code)
				printer.printline('//!wrapper_%s::%s_end'%(fullclsname, self.name))
				return False
		accessor = ''
		if self.static == 0:
			if builder.singleton:
				accessor = 'Module.%s.getInstance().%s'%('_'.join(builder.namespaces) + '_' + builder.jsclsname, self.name)
			else:
				accessor = 'this.nativeInstance.'+self.name
		else:
			accessor = 'Module.%s.%s'%('_'.join(builder.namespaces) + '_' + builder.jsclsname, self.name)
		if self.paramNumber > 0:
			printer.printline('do {')
			printer.indent();
			argnames, disposecodes = self._genParamsCode(printer, builder)
			if self.returnType.isVoid():
				printer.printline('%s(%s);'%(accessor, ', '.join(argnames)))
				if len(disposecodes) == 0:
					printer.printline('return;')
			else:
				if len(disposecodes) == 0:
					printValueToJSValueCode(printer, 'return %s;', '%s(%s);'%(accessor, ', '.join(argnames)), self.returnType, builder)
				else:
					printValueToJSValueCode(printer, 'let retValue = %s;', '%s(%s);'%(accessor, ', '.join(argnames)), self.returnType, builder)
			if len(disposecodes) > 0:
				for c in disposecodes:
					printer.printline(c)
				if not self.returnType.isVoid():
					printer.printline('return retValue')
				else:
					printer.printline('return;')
			printer.outdent()
			printer.printline('}')
			printer.outdent();
			printer.printline('} while(0);')
			return True
		else:
			if self.returnType.isVoid():
				printer.printline('%s()'%accessor)
			else:
				printValueToJSValueCode(printer, 'return %s;', '%s()'%accessor, self.returnType, builder)
			return False

	def scanCppCode(self, code, builder):
		fullclsname = builder.namespace + builder.cppclsname
		if fullclsname in mannualcode:
			if self.name in mannualcode[fullclsname]:
				result = re.findall('//!binding_%s::%s\\n(.*?)//!binding_%s::%s_end'%(fullclsname, self.name, fullclsname, self.name), code, re.S)
				if len(result):
					code = result[0].rstrip()
					mannualcode[fullclsname][self.name]['cppfunc'] = processCodeLines(code)
		
	def scanJsCode(self, code, builder):
		fullclsname = builder.namespace + builder.cppclsname
		if fullclsname in mannualcode:
			if self.name in mannualcode[fullclsname]:
				result = re.findall('//!wrapper_%s::%s\\n(.*?)//!wrapper_%s::%s_end'%(fullclsname, self.name, fullclsname, self.name), code, re.S)
				if len(result):
					code = result[0].rstrip()
					mannualcode[fullclsname][self.name]['js'] = processCodeLines(code)

class CustomCtor:
	def __init__(self, config):
		self.config = config
		self.priority = 0
		self.minParamNumber = 0
		self.paramNumber = 0
		self.name = 'ctor'
		self.cppname = config['constructor']['cppfuncname']
		
	def genSource(self, printer, builder):
		printer.printline('//!ctor')
		printer.printline(self.config['constructor'])
		printer.printline('//!ctor_end')
		
	def genBinding(self, printer, printer1, builder):
		if len(self.cppname) == 0:
			return
		else:
			raise Exception('to be implemented')
		
	def genWrapper(self, printer, builder):
		fullclsname = builder.namespace + builder.cppclsname
		if self.config['constructor'].has_key('js'):
			printer.printline('//!ctor_%s::%s'%(fullclsname, self.name))
			printer.printline(self.config['constructor']['js'])
			printer.printline('//!ctor_%s::%s_end'%(fullclsname, self.name))
		
	def scanCppCode(self, code, builder):
		pass
		
		
	def scanJsCode(self, code, builder):
		fullclsname = builder.namespace + builder.cppclsname
		result = re.findall('//!ctor_%s::%s\\n(.*?)//!ctor_%s::%s_end'%(fullclsname, self.name, fullclsname, self.name), code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.config['constructor']['js'] = processCodeLines(code)
	
	def scanCode(self, code, builder):
		result = re.findall('//!ctor\\n(.*?)//!ctor_end', code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.code = processCodeLines(code)
			self.config['constructor'] = self.code

class CustomProp:
	def __init__(self, name, config, static = 0):
		self.name = name
		self.config = config
		self.static = static

	def genEmbindSource(self, printer, printer1, builder):
		if self.config.has_key('cppgettername'):
			if self.static == 0:
				printer1.printline('.function("get%s", &%s, allow_raw_pointers())'%(self.name.capitalize(), self.config['cppgettername']))
			else:
				printer1.printline('.class_function(get%s", &%s, allow_raw_pointers())'%(self.name.capitalize(), self.config['cppgettername']))
		if self.config.has_key('cppsettername'):
			if self.static == 0:
				printer1.printline('.function("set%s", &%s, allow_raw_pointers())'%(self.name.capitalize(), self.config['cppsettername']))
			else:
				printer1.printline('.class_function(set%s", &%s, allow_raw_pointers())'%(self.name.capitalize(), self.config['cppsettername']))
		if self.config.has_key('cpp'):
			fullclsname = builder.namespace + builder.cppclsname
			printer.printline('//!prop_%s::%s'%(fullclsname, self.name))
			printer.printline(self.config['cpp'])
			printer.printline('//!prop_%s::%s_end'%(fullclsname, self.name))

	def genWrapper(self, printer, builder):
		fullclsname = builder.namespace + builder.cppclsname
		if self.config.has_key('js'):
			printer.printline('//!prop_%s::%s'%(fullclsname, self.name))
			printer.printline(self.config['js'])
			printer.printline('//!prop_%s::%s_end'%(fullclsname, self.name))

	def scanCppCode(self, code, builder):
		fullclsname = builder.namespace + builder.cppclsname
		result = re.findall('//!prop_%s::%s\\n(.*?)//!prop_%s::%s_end'%(fullclsname, self.name, fullclsname, self.name), code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.config['cpp'] = processCodeLines(code)
		
	def scanJsCode(self, code, builder):
		fullclsname = builder.namespace + builder.cppclsname
		result = re.findall('//!prop_%s::%s\\n(.*?)//!prop_%s::%s_end'%(fullclsname, self.name, fullclsname, self.name), code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.config['js'] = processCodeLines(code)
		
	def scanCode(self, code, buidler):
		if 'getter' in self.config:
			result = re.findall('//!getter_%s\\n(.*?)//!getter_%s_end'%(self.name, self.name), code, re.S)
			if len(result):
				match = result[0].rstrip()
				self.config['getter'] = processCodeLines(match)
		if 'setter' in self.config:
			result = re.findall('//!setter_%s\\n(.*?)//!setter_%s_end'%(self.name, self.name), code, re.S)
			if len(result):
				match = result[0].rstrip()
				self.config['setter'] = processCodeLines(match)

class CustomFunc:
	def __init__(self, name, config, static = False):
		self.name = name
		self.code = config[name]['cppfunc']
		self.config = config
		self.cppname = config[name]['cppfuncname']
		self.minParamNumber = 0
		self.paramNumber = 0
		self.priority = 0
		self.static = static
		
	def genSource(self, printer, builder):
		printer.printline('//!func_%s'%self.name)
		printer.printline(self.code)
		printer.printline('//!func_%s_end'%self.name)
		return False
	
	def genBinding(self, printer, printer1, builder, index):
		if len(self.cppname) == 0:
			return
		printer.printline('//!binding_func_%s'%(self.cppname, ))
		printer.printline(self.code)
		printer.printline('//!binding_func_%s_end\n'%(self.cppname, ))
		if self.static == 0:
			printer1.printline('.function("%s", &%s, allow_raw_pointers())'%(self.name, self.cppname))
		else:
			printer1.printline('.class_function("%s", &%s, allow_raw_pointers())'%(self.name, self.cppname))
		
	def genWrapper(self, printer, builder, index):
		printer.printline('//!binding_func_%s'%(self.cppname, ))
		printer.printline(self.config[self.name]['js'])
		printer.printline('//!binding_func_%s_end\n'%(self.cppname, ))
		return False
			
	def canGenJSCode(self):
		return self.config[self.name].has_key('js')
		
	def scanCppCode(self, code, builder):
		if len(self.cppname) == 0:
			return
		result = re.findall('//!binding_func_%s\\n(.*?)//!binding_func_%s_end'%(self.cppname, self.cppname), code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.code = processCodeLines(code)
			self.config[self.name]['cppfunc'] = self.code
		
	def scanJsCode(self, code, builder):
		if len(self.cppname) == 0:
			return
		result = re.findall('//!binding_func_%s\\n(.*?)//!binding_func_%s_end'%(self.cppname, self.cppname), code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.code = processCodeLines(code)
			self.config[self.name]['js'] = processCodeLines(self.code)
		
	def scanCode(self, code, builder):
		result = re.findall('//!func_%s\\n(.*?)//!func_%s_end'%(self.name, self.name), code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.code = processCodeLines(code)
			self.config[self.name] = self.code

class JSFuncFromProp:
	def __init__(self, cppField):
		self.type = cppField.genType
		self.static = cppField.static
		if self.type != 'SETTER' and self.type != 'GETTER' and self.type != 'DELEGATEADD' and self.type != 'DELEGATERM':
			raise Exception('Invalid type')
		args = cppField.args
		self.cppSetterFunc = None
		self.cppGetterFunc = None
		if args.setter:
			self.cppSetterFunc = args.setter
		if args.getter:
			self.cppGetterFunc = args.getter
		self.valueType = cppField.var.type
		
		self.propName = cppField.name
		if args.funcname:
			self.name = args.funcname
		else:
			if self.type == 'SETTER':
				self.name = 'set'+self.propName.capitalize()
			elif self.type == 'GETTER':
				self.name = 'get'+self.propName.capitalize()
			elif self.type == 'DELEGATEADD':
				self.name = 'add'+self.propName.capitalize()
			else:
				self.name = 'remove'+self.propName.capitalize()
		self.cppname = 'js%s'%self.name.capitalize()
		
	def _genSetterFunc(self, printer, builder, accessor):
		isDelegateAssign = self.valueType.ctype.spelling.startswith('hoolai::CDelegate')
		if isDelegateAssign:
			if self.static == 0:
				printer.printline('void %s_%s_%s(%s* instance, val func)'%('_'.join(builder.namespaces), builder.cppclsname, self.name, builder.namespace+builder.cppclsname))
			else:
				printer.printline('void %s_%s_%s(val func)'%('_'.join(builder.namespaces), builder.cppclsname, self.name))
			printer.printline('{')
			printer.indent()
			templateArgTypes = self.valueType.templateArgNames
			templateArgs = ', '.join(templateArgTypes)
			printer.printline("if (func.isUndefined() || func.isNull())")
			printer.printline("{")
			printer.indent()
			printer.printline("%s.clear();"%(accessor, ))
			printer.outdent()
			printer.printline("}")
			printer.printline("else")
			printer.printline("{")
			printer.indent();
			if len(templateArgs) > 0:
				printer.printline('%s = hoolai::newDelegate<%s>(func);'%(accessor, templateArgs))
			else:
				printer.printline('%s = hoolai::newDelegate(func);'%accessor)
			printer.outdent();
			printer.printline("}")
		else:
			if self.static == 0:
				printer.printline('void %s_%s_%s(%s* instance, %s value)'%('_'.join(builder.namespaces), builder.cppclsname, self.name, builder.namespace+builder.cppclsname, self.valueType.spelling))
			else:
				printer.printline('void %s_%s_%s(%s value)'%('_'.join(builder.namespaces), builder.cppclsname, self.name, self.valueType.spelling))
			printer.printline('{')
			printer.indent()
			if self.cppSetterFunc:
				printer.printline('instance->%s(value);'%self.cppSetterFunc)
			else:
				if self.valueType.isVector() or self.valueType.isList():
					printer.printline('%s.swap(value);'%accessor)
				else:
					printer.printline('%s = value;'%accessor)
						
		printer.outdent()
		printer.printline('}\n')
		
	def _genGetterFunc(self, printer, builder, accessor):
		valueType = self.valueType.spelling
		if self.valueType.isArray():
			valueType = self.valueType.ctype.get_array_element_type().spelling + "*"
		if self.static == 0:
			printer.printline('%s %s_%s_%s(%s* instance)'%(valueType, '_'.join(builder.namespaces), builder.cppclsname, self.name, builder.namespace+builder.cppclsname))
		else:
			printer.printline('%s %s_%s_%s()'%(valueType, '_'.join(builder.namespaces), builder.cppclsname, self.name))
		printer.printline('{')
		printer.indent()
		if self.cppGetterFunc:
			printer.printline('return instance->%s();'%self.cppGetterFunc)
		else:
			printer.printline('return %s;'%accessor)
					
		printer.outdent()
		printer.printline('}\n')
		
	def _genDelegateFunc(self, printer, builder, accessor):
		templateArgTypes = self.valueType.templateArgNames
		templateArg = ', '.join(templateArgTypes)
		if self.static == 0:
			printer.printline('void %s_%s_%s(%s* instance, val func)'%('_'.join(builder.namespaces), builder.cppclsname, self.name, builder.namespace+builder.cppclsname))
		else:
			printer.printline('void %s_%s_%s(val func)'%('_'.join(builder.namespaces), builder.cppclsname, self.name))
		printer.printline('{')
		printer.indent()
		if self.type == 'DELEGATEADD':
			if len(templateArg) > 0:
				printer.printline('%s += hoolai::newDelegate<%s>(func);'%(accessor, templateArg))
			else:
				printer.printline('%s += hoolai::newDelegate(func);'%accessor)
		else:
			if len(templateArg) > 0:
				printer.printline('%s -= hoolai::newDelegate<%s>(func);'%(accessor, templateArg))
			else:
				printer.printline('%s -= hoolai::newDelegate(func);'%accessor)
		printer.outdent()
		printer.printline('}')
		
	def genBinding(self, printer, printer1, builder, idx):
		accessor = ''
		if self.static == 0:
			accessor = 'instance->%s'%self.propName
		else:
			accessor = '%s::%s'%(builder.cppclsname, self.propName)
		if self.type == 'SETTER':
			self._genSetterFunc(printer, builder, accessor)
			if self.static == 0:
				printer1.printline('.function("%s", &%s_%s_%s, allow_raw_pointers())'%(self.name, '_'.join(builder.namespaces), builder.cppclsname, self.name))
			else:
				printer1.printline('.class_function("%s", &%s_%s_%s, allow_raw_pointers())'%(self.name, '_'.join(builder.namespaces), builder.cppclsname, self.name))
		elif self.type == 'GETTER':
			self._genGetterFunc(printer, builder, accessor)
			if self.static == 0:
				printer1.printline('.function("%s", &%s_%s_%s, allow_raw_pointers())'%(self.name, '_'.join(builder.namespaces), builder.cppclsname, self.name))
			else:
				printer1.printline('.class_function("%s", &%s_%s_%s, allow_raw_pointers())'%(self.name, '_'.join(builder.namespaces), builder.cppclsname, self.name))
		else:
			if not self.valueType.ctype.spelling.startswith('hoolai::CMultiDelegate'):
				raise Exception('value is not a multidelegate type')
			self._genDelegateFunc(printer, builder, accessor)
			if self.static == 0:
				printer1.printline('.function("%s", &%s_%s_%s, allow_raw_pointers())'%(self.name, '_'.join(builder.namespaces), builder.cppclsname, self.name))
			else:
				printer1.printline('.class_function("%s", &%s_%s_%s, allow_raw_pointers())'%(self.name, '_'.join(builder.namespaces), builder.cppclsname, self.name))
			
	def _genJsSetter(self, printer, builder, accessor):
		isDelegateAssign = self.valueType.ctype.spelling.startswith('hoolai::CDelegate')
		if isDelegateAssign:
			printer.printline('if (argumentCount != 1 && argumentCount != 2) {')
		else:
			printer.printline('if (argumentCount != 1) {')
		printer.indent()
		if isDelegateAssign:
			printer.printline('throw new Error("%s.%s: wrong argument number(1 or 2 arguments needed)");'%(builder.jsclsname, self.name))
		else:
			printer.printline('throw new Error("%s.%s: wrong argument number(1 arguments needed)");'%(builder.jsclsname, self.name))
		printer.outdent()
		printer.printline('}')
		if isDelegateAssign:
			printer.printline('if (!arguments[0]) {')
			printer.indent()
			printer.printline('%s(null);'%(accessor, ))
			printer.outdent()
			printer.printline('} else {')
			printer.indent()
			printer.printline('let callback = argumentCount == 1 ? arguments[0] : arguments[1];')
			printer.printline('let that = argumentCount > 1 ? arguments[0] : this;')
			printer.printline('let callbackfunc = function() {')
			printer.indent()
			i = 0
			args = ''
			for t in self.valueType.templateArgs:
				printer.printline('let arg%d;'%i)
				i += 1
			i = 0
			for t in self.valueType.templateArgs:
				printer.printline('{')
				printer.indent()
				printValueToJSValueCode(printer, 'arg%d = %%s;'%i, 'arguments[%d]'%i, t, builder)
				printer.outdent()
				printer.printline('}')
				args = args + ', arg%d'%i
				i += 1
			printer.printline('callback.call(that%s);'%args)
			printer.outdent()
			printer.printline('}')
			printer.printline('%s(callbackfunc);'%(accessor, ))
			printer.outdent()
			printer.printline('}')
		else:
			checkCode = JSValueTypeCheckCode('arguments[0]', self.valueType, builder)
			if checkCode is not None:
				printer.printline('if (!%s) {'%checkCode)
				printer.indent()
				printer.printline('throw new Error("%s.%s: wrong argument type");'%(builder.jsclsname, self.name))
				printer.outdent()
				printer.printline('}')
			ret = printJSValueToValueCode(printer, '%s(%%s);'%accessor, 'arguments[0]', self.valueType, builder)
			if ret:
				printer.printline(ret)
		
	def _genJsGetter(self, printer, builder, accessor):
		printer.printline('let ret = %s();'%accessor)
		printValueToJSValueCode(printer, 'return %s', 'ret', self.valueType, builder)
		
	def _genJsDelegate(self, printer, builder, accessor):
		printer.printline('if (!this._%s) {'%self.propName)
		printer.indent()
		printer.printline('this._%s = [];'%self.propName)
		printer.outdent()
		printer.printline('}')
		printer.printline('let wrapper = {obj:argumentCount>1?arguments[0]:this, func:argumentCount>1?arguments[1]:arguments[0]};')
		printer.printline('let index = this._%s.findIndex(function(item) {return item.func == wrapper.func;});'%self.propName)
		if self.type == 'DELEGATEADD':
			printer.printline('if (index >= 0) return;')
			printer.printline('wrapper.callback = function() {')
			printer.indent()
			i = 0
			args = ''
			for t in self.valueType.templateArgs:
				printValueToJSValueCode(printer, 'let arg%d = %%s;'%i, 'arguments[%d]'%i, t, builder)
				args = args + ', arg%d'%i
				i += 1
			printer.printline('this.func.call(this.obj%s);'%args)
			printer.outdent()
			printer.printline('}.bind(wrapper);')
			printer.printline('this._%s.push(wrapper);'%self.propName)
			printer.printline('%s(wrapper.callback);'%accessor)
		else:
			printer.printline('if (index >= 0) {')
			printer.indent();
			printer.printline('let oldwrapper = this._%s[index];'%self.propName)
			printer.printline('this._%s.splice(index, 1);'%self.propName)
			printer.printline('%s(oldwrapper.callback);'%accessor)
			printer.outdent();
			printer.printline('}')
	
	def genWrapper(self, printer, builder, idx):
		accessor = ''
		if self.static == 1:
			accessor = 'Module.' + '_'.join(builder.namespaces) + '_' + builder.jsclsname + '.' + self.name
		else:
			if builder.singleton:
				accessor = 'Module.%s.getInstance().%s'%('_'.join(builder.namespaces) + '_' + builder.jsclsname, self.name)
			else:
				accessor = 'this.nativeInstance.' + self.name
		if self.type == 'SETTER':
			self._genJsSetter(printer, builder, accessor)
		elif self.type == 'GETTER':
			self._genJsGetter(printer, builder, accessor)
		else:
			self._genJsDelegate(printer, builder, accessor)
		
	def canGenJSCode(self):
		return True
		
	def _genSetterCode(self, printer, builder, accessor):
		isDelegateAssign = self.valueType.ctype.spelling.startswith('hoolai::CDelegate')
		if isDelegateAssign:
			printer.printline('if (argumentCount != 1 && argumentCount != 2)\n{')
		else:
			printer.printline('if (argumentCount != 1)\n{')
		printer.indent()
		message = None
		if isDelegateAssign:
			message = '%s.%s: wrong argument number(1 or 2 agrument needed)'%(builder.jsclsname, self.name)
		else:
			message = '%s.%s: wrong argument number(1 agrument needed)'%(builder.jsclsname, self.name)
		printExceptionCode(printer, message)
		printer.printline('return nullptr;')
		printer.outdent()
		printer.printline('}')
		if isDelegateAssign:
			templateArgTypes = self.valueType.templateArgNames
			templateArgs = ', '.join(templateArgTypes)
			printer.printline('if (argumentCount == 1)\n{')
			printer.indent()
			printer.printline('if (JSValueIsNull(ctx, arguments[0]))\n{')
			printer.indent()
			printer.printline('%s.clear();'%accessor)
			printer.outdent()
			printer.printline('}')
			printer.printline('else\n{')
			printer.indent()
			printer.printline('JSObjectRef funcObj = JSValueToObject(ctx, arguments[0], exception);')
			printer.printline('if (*exception) return nullptr;')
			printer.printline('if (!JSObjectIsFunction(ctx, funcObj))\n{')
			printer.indent()
			printExceptionCode(printer, '%s.%s: wrong argument type(function is needed)'%(builder.jsclsname, self.name))
			printer.printline('return nullptr;')
			printer.outdent()
			printer.printline('}')
			if len(templateArgs) > 0:
				printer.printline('%s = hoolai::newDelegate<%s>(thisObject, funcObj);'%(accessor, templateArgs))
			else:
				printer.printline('%s = hoolai::newDelegate(thisObject, funcObj);'%accessor)
			printer.outdent()
			printer.printline('}')
			printer.outdent()
			printer.printline('}') # endif argumentCount == 1
			
			printer.printline('else\n{') # argumentCount == 2
			printer.indent()
			printer.printline('JSObjectRef targetObj = JSValueToObject(ctx, arguments[0], exception);')
			printer.printline('if (*exception) return nullptr;')
			printer.printline('JSObjectRef funcObj = JSValueToObject(ctx, arguments[1], exception);')
			printer.printline('if (*exception) return nullptr;')
			printer.printline('if (!JSObjectIsFunction(ctx, funcObj))\n{')
			printer.indent()
			printExceptionCode(printer, '%s.%s: wrong argument type(function is needed for argument 2)'%(builder.jsclsname, self.name))
			printer.printline('return nullptr;')
			printer.outdent()
			printer.printline('}')
			if len(templateArgs) > 0:
				printer.printline('%s = hoolai::newDelegate<%s>(targetObj, funcObj);'%(accessor, templateArgs))
			else:
				printer.printline('%s = hoolai::newDelegate(targetObj, funcObj);'%accessor)
			printer.outdent()
			printer.printline('}') # endif argumentCount == 2
		else:
			checkCode = JSValueTypeCheckCode('arguments[0]', self.valueType, builder)
			if checkCode is not None:
				printer.printline('if (!%s)\n{'%checkCode)
				printer.indent()
				printExceptionCode(printer, '%s.%s: wrong argument type'%(builder.jsclsname, self.name))
				printer.printline('return nullptr;')
				printer.outdent()
				printer.printline('}')
			printer.printline('%s nativeValue;'%self.valueType.spellingNoReference)
			printJSValueToValueCode(printer, 'arguments[0]', 'nativeValue', self.valueType, builder)
			if self.cppSetterFunc:
				printer.printline('thiz->%s(nativeValue);'%self.cppSetterFunc)
			else:
				if self.valueType.isVector() or self.valueType.isList():
					printer.printline('%s.swap(nativeValue);'%accessor)
				else:
					printer.printline('%s = nativeValue;'%accessor)
		printer.printline('return nullptr;')
		
	def _genGetterCode(self, printer, builder, accessor):
		if self.cppGetterFunc:
			printValueToJSValueCode(printer, 'return %s;', 'thiz->%s()'%self.cppGetterFunc, self.valueType, builder)
		else:
			printValueToJSValueCode(printer, 'return %s;', accessor, self.valueType, builder)
	
	def _genDelegateCode(self, printer, builder, accessor):
		templateArgTypes = self.valueType.templateArgNames
		templateArg = ', '.join(templateArgTypes)
		printer.printline('if (argumentCount != 1 && argumentCount != 2)\n{')
		printer.indent()
		printExceptionCode(printer, '%s.%s: wrong argument number(1 or 2 agrument needed)'%(builder.jsclsname, self.name))
		printer.printline('return nullptr;')
		printer.outdent()
		printer.printline('}')
		printer.printline('if (argumentCount == 1)\n{')
		printer.indent()
		printer.printline('JSObjectRef funcObj = JSValueToObject(ctx, arguments[0], exception);')
		printer.printline('if (*exception) return nullptr;')
		printer.printline('if (!JSObjectIsFunction(ctx, funcObj))\n{')
		printer.indent()
		printExceptionCode(printer, '%s.%s: wrong argument type(function is needed)'%(builder.jsclsname, self.name))
		printer.printline('return nullptr;')
		printer.outdent()
		printer.printline('}')
		if self.type == 'DELEGATEADD':
			if len(templateArg) > 0:
				printer.printline('%s += hoolai::newDelegate<%s>(thisObject, funcObj);'%(accessor, templateArg))
			else:
				printer.printline('%s += hoolai::newDelegate(thisObject, funcObj);'%accessor)
		else:
			if len(templateArg) > 0:
				printer.printline('%s -= hoolai::newDelegate<%s>(thisObject, funcObj);'%(accessor, templateArg))
			else:
				printer.printline('%s -= hoolai::newDelegate(thisObject, funcObj);'%accessor)
		printer.outdent()
		printer.printline('}\nelse\n{') # argumentCount == 2
		printer.indent()
		printer.printline('JSObjectRef targetObj = JSValueToObject(ctx, arguments[0], exception);')
		printer.printline('if (*exception) return nullptr;')
		printer.printline('JSObjectRef funcObj = JSValueToObject(ctx, arguments[1], exception);')
		printer.printline('if (*exception) return nullptr;')
		printer.printline('if (!JSObjectIsFunction(ctx, funcObj))\n{')
		printer.indent()
		printExceptionCode(printer, '%s.%s: wrong argument type(function is needed for argument 2)'%(builder.jsclsname, self.name))
		printer.printline('return nullptr;')
		printer.outdent()
		printer.printline('}')
		if self.type == 'DELEGATEADD':
			if len(templateArg) > 0:
				printer.printline('%s += hoolai::newDelegate<%s>(targetObj, funcObj);'%(accessor, templateArg))
			else:
				printer.printline('%s += hoolai::newDelegate(targetObj, funcObj);'%accessor)
		else:
			if len(templateArg) > 0:
				printer.printline('%s -= hoolai::newDelegate<%s>(targetObj, funcObj);'%(accessor, templateArg))
			else:
				printer.printline('%s -= hoolai::newDelegate(targetObj, funcObj);'%accessor)
		printer.outdent()
		printer.printline('}')
		printer.printline('return nullptr;')
	
	def genSource(self, printer, builder):
		accessor = ''
		if self.static == 0:
			accessor = 'thiz->%s'%self.propName
		else:
			accessor = '%s::%s'%(builder.cppclsname, self.propName)
		if self.type == 'SETTER':
			self._genSetterCode(printer, builder, accessor)
		elif self.type == 'GETTER':
			self._genGetterCode(printer, builder, accessor)
		else:
			if not self.valueType.ctype.spelling.startswith('hoolai::CMultiDelegate'):
				raise Exception('value is not a multidelegate type')
			self._genDelegateCode(printer, builder, accessor)
		return False
		

def compFunc(x, y):
	if x.priority < y.priority:
		return 1
	elif x.priority > y.priority:
		return -1
	return 0

def checkFuncs(funcs):
	size = len(funcs)
	for i in range(0, size - 1):
		func = funcs[i]
		func1 = funcs[i+1]
		if func.priority == func1.priority and func.priority != 100:
			s = set(range(func.minParamNumber, func.paramNumber))
			s1 = set(range(func1.minParamNumber, func1.paramNumber))
			if len(s & s1) > 0:
				print 'warning: function overloads param may conflict %s'%func.name

class JSClassBindingBuilder:
	def __init__(self, cls):
		self.ctors = []
		self.props = []
		self.funcNames = []
		self.funcs = {}
		self.numFuncs = 0
		self.staticProps = []
		self.staticFuncNames = []
		self.staticFuncs = {}
		self.numStaticFuncs = 0
		_, self.cppheader = os.path.split(cls.file)
		self.cppheader
		
		self.namespace = '::'.join(cls.namespaces) + '::'
		self.namespaces = cls.namespaces
		
		if len(cls.superClasses) > 0:
			self.parentClass = cls.superClasses[0].type.spelling
			_, self.parentClassHeader = os.path.split(cls.superClasses[0].location.file.name)
		else:
			self.parentClass = None
			
		self.parentClassInfo = findClassInfo(self.parentClass)
			
		self.singleton = cls.singleton
		if self.singleton:
			self.singletonFunc = cls.singletonFuncName
			if cls.singletonFunc.returnType.isPointer():
				self.singletonFuncIsPointer = True
			else:
				self.singletonFuncIsPointer = False
				raise Exception('reference return value of singleton not supported')
		self.jsclsname = cls.jsclsname
		self.cppclsname = cls.name
		self.bindingclsname = '.'.join(self.namespaces) + '.' + self.jsclsname
		
		global classesmap
		classesmap[self.namespace+self.cppclsname] = {'wrapperclass': self.bindingclsname, 'jsname': self.jsclsname, 'namespace': self.namespace}
		
		for method in cls.ctors:
			self.ctors.append(JSCtor(method, self))
		self.ctors.sort(compFunc)
		checkFuncs(self.ctors)
			
		for method in cls.funcs:
			func = JSFunc(method, self)
			if func.name not in self.funcs:
				self.funcs[func.name] = []
				self.funcNames.append(func.name)
			self.funcs[func.name].append(func)
			
		for method in cls.staticFuncs:
			func = JSFunc(method, self)
			if func.name not in self.staticFuncs:
				self.staticFuncs[func.name] = []
				self.staticFuncNames.append(func.name)
			self.staticFuncs[func.name].append(func)
		
		for prop in cls.props:
			if prop.genType == 'PROP':
				self.props.append(JSProp(prop))
			else:
				func = JSFuncFromProp(prop)
				if func.name in self.funcs:
					raise Exception('property function name duplicated %s'%func.name)
				self.funcs[func.name] = []
				self.funcNames.append(func.name)
				self.funcs[func.name].append(func)
				
		for prop in cls.staticProps:
			if prop.genType == 'PROP':
				self.props.append(JSProp(prop))
			else:
				func = JSFuncFromProp(prop)
				if func.name in self.staticFuncs:
					raise Exception('property function name duplicated %s'%func.name)
				self.staticFuncs[func.name] = []
				self.staticFuncNames.append(func.name)
				self.staticFuncs[func.name].append(func)
				
		self.customincludes = []
		fullname = self.namespace + self.cppclsname
		if fullname in mannualcode:
			clsconfig = mannualcode[fullname]
			if 'includes' in clsconfig:
				self.customincludes = clsconfig['includes']
			if 'constructor' in clsconfig:
				self.ctors.append(CustomCtor(clsconfig))
			if 'properties' in clsconfig:
				props = clsconfig['properties']
				for prop in props:
					self.props.append(CustomProp(prop, props[prop]))
			if 'staticProperties' in clsconfig:
				props = clsconfig['staticProperties']
				for prop in props:
					self.staticProps.append(CustomProp(prop, props[prop], 1))
			if 'functions' in clsconfig:
				funcs = clsconfig['functions']
				for func in funcs:
					if func not in self.funcs:
						self.funcs[func] = []
						self.funcNames.append(func)
					self.funcs[func].append(CustomFunc(func, funcs))
			if 'staticFunctions' in clsconfig:
				funcs = clsconfig['staticFunctions']
				for func in funcs:
					if func not in self.staticFuncs:
						self.staticFuncs[func] = []
						self.staticFuncNames.append(func)
					self.staticFuncs[func].append(CustomFunc(func, funcs, True))
					
		self.numFuncs = len(self.funcNames)
		self.numStaticFuncs = len(self.staticFuncs.keys())
	
	def generateBindingSource(self, printer, printer1):
		bindingName = '_'.join(self.namespaces) + '_' + self.jsclsname
		if self.singleton:
			printer1.printline('class_<%s>("%s")'%(self.namespace+self.cppclsname, bindingName))
			printer1.printline('.class_function("getInstance", &%s::%s, allow_raw_pointers())'%(self.namespace+self.cppclsname, self.singletonFunc))
		else:
			wrapperFuncName = '_'.join(self.namespaces) + '_'+self.cppclsname + '_wrapper_getter'
			printer.printline('val %s(%s* instance)'%(wrapperFuncName, self.namespace+self.cppclsname))
			printer.printline('{')
			printer.indent()
			printer.printline('auto itr = jswrappermap.find(instance);')
			printer.printline('if (itr == jswrappermap.end())')
			printer.printline('{')
			printer.indent()
			wrappercls = None
			if len(self.namespaces) == 0:
				wrappercls = 'val::global("%s")'%(self.jsclsname, )
			else:
				wrappercls = 'val::global("%s")'%(self.namespaces[0], )
				if len(self.namespaces) > 1:
					for i in range(1, len(self.namespaces)):
						wrappercls = wrappercls + '["%s"]'%(self.namespaces[i], )
				wrappercls = wrappercls + '["%s"]'%(self.jsclsname, )
			printer.printline('val wrapper = %s.new_(instance);'%(wrappercls, ))
			printer.printline('jswrappermap.insert(std::make_pair(instance, wrapper));')
			printer.printline('return wrapper;')
			printer.outdent()
			printer.printline('}')
			printer.printline('return itr->second;')
			printer.outdent()
			printer.printline('}\n')
			if self.parentClass:
				classInfo = findClassInfo(self.parentClass)
				if classInfo:
					printer1.printline('class_<%s, base<%s>>("%s")'%(self.namespace+self.cppclsname, self.parentClass, bindingName))
				else:
					printer1.printline('class_<%s>("%s")'%(self.namespace+self.cppclsname, bindingName))
			else:
				printer1.printline('class_<%s>("%s")'%(self.namespace+self.cppclsname, bindingName))
			for ctor in self.ctors:
				ctor.genBinding(printer, printer1, self)
			printer1.printline('.function("getWrapper", &%s, allow_raw_pointers())'%(wrapperFuncName, ))
			if len(self.ctors) > 0:
				printer1.printline('.function("registWrapper", &registWrapper<%s>, allow_raw_pointers())'%(self.namespace+self.cppclsname, ))
		for prop in self.props:
			prop.genEmbindSource(printer, printer1, self)
		for prop in self.staticProps:
			prop.genEmbindSource(printer, printer1, self)
		for key in self.funcNames:
			funcs = self.funcs[key]
			idx = 0
			for func in funcs:
				func.genBinding(printer, printer1, self, idx)
				idx += 1
		for key in self.staticFuncNames:
			funcs = self.staticFuncs[key]
			idx = 0
			for func in funcs:
				func.genBinding(printer, printer1, self, idx)
				idx += 1
		printer1.printline(';\n')
		
	def generateBindingWrapper(self, printer):
		if self.singleton:
			printer.printline('%s = {'%('.'.join(self.namespaces) + '.' + self.jsclsname, ))
			printer.indent()
			for key in self.funcNames:
				funcs = self.funcs[key]
				if not funcs[0].canGenJSCode():
					continue
				printer.printline('%s : function() {'%key)
				printer.indent()
				printer.printline('var argumentCount = arguments.length;')
				idx = 0
				genExceptionCode = True
				for func in funcs:
					if not func.genWrapper(printer, self, idx):
						genExceptionCode = False
						break
					idx += 1
				if genExceptionCode:
					printer.printline('throw new Error("%s.%s: Wrong arguments");'%('.'.join(self.namespaces) + '.' + self.jsclsname, key))
				printer.outdent()
				printer.printline('},')
			printer.outdent()
			printer.printline('};\n')
		else:
			nativeclsname = '_'.join(self.namespaces) + '_' + self.jsclsname
			varfullname = '.'.join(self.namespaces) + '.' + self.jsclsname
			if self.parentClassInfo:
				printer.printline('class %s extends %s {'%(nativeclsname, self.parentClassInfo['wrapperclass']))
			else:
				printer.printline('class %s {'%(nativeclsname,))
			printer.indent()
			printer.printline('constructor() {')
			printer.indent()
			printer.printline('var argumentCount = arguments.length;')
			printer.printline('if (argumentCount == 1 && arguments[0] instanceof Module.%s) {'%(nativeclsname, ))
			printer.indent()
			if self.parentClassInfo:
				printer.printline('super(arguments[0]);')
				printer.printline('return;')
			else:
				printer.printline('this.nativeInstance = arguments[0];\nreturn;')
			printer.outdent()
			printer.printline('}')
			if len(self.ctors) > 0:
				genExceptionCode = True
				for ctor in self.ctors:
					if not ctor.genWrapper(printer, self):
						genExceptionCode = False
						break
				if genExceptionCode:
					printer.printline('throw new Error("%s: Wrong arguments passed to the constructor");'%(varfullname, ))
			else:
				printer.printline('throw new Error("%s: Constructor is not allowed to be called directly");'%(varfullname, ))
			printer.outdent()
			printer.printline('}')
			for prop in self.props:
				prop.genWrapper(printer, self)
			for prop in self.staticProps:
				prop.genWrapper(printer, self)
			for key in self.funcNames:
				funcs = self.funcs[key]
				if not funcs[0].canGenJSCode():
					continue
				printer.printline('%s() {'%key)
				printer.indent()
				printer.printline('var argumentCount = arguments.length;')
				idx = 0
				genExceptionCode = True
				for func in funcs:
					if not func.genWrapper(printer, self, idx):
						genExceptionCode = False
						break
					idx += 1
				if genExceptionCode:
					printer.printline('throw new Error("%s.%s: Wrong arguments");'%(varfullname, key))
				printer.outdent()
				printer.printline('}')
			for key in self.staticFuncNames:
				funcs = self.staticFuncs[key]
				if not funcs[0].canGenJSCode():
					continue
				printer.printline('static %s() {'%key)
				printer.indent()
				printer.printline('var argumentCount = arguments.length;')
				idx = 0
				genExceptionCode = True
				for func in funcs:
					if not func.genWrapper(printer, self, idx):
						genExceptionCode = False
						break
					idx += 1
				if genExceptionCode:
					printer.printline('throw new Error("%s.%s: Wrong arguments");'%(varfullname, key))
				printer.outdent()
				printer.printline('}')
			printer.outdent()
			printer.printline('}')
			printer.printline('%s = %s;\n'%(varfullname, nativeclsname))
		
	def scanCppCode(self, code):
		for ctor in self.ctors:
			if hasattr(ctor, 'scanCppCode'):
				ctor.scanCppCode(code, self)
		for key in self.funcs:
			for func in self.funcs[key]:
				if hasattr(func, 'scanCppCode'):
					func.scanCppCode(code, self)
		for key in self.staticFuncs:
			for func in self.staticFuncs[key]:
				if hasattr(func, 'scanCppCode'):
					func.scanCppCode(code, self)
		for prop in self.props:
			if hasattr(prop, 'scanCppCode'):
				prop.scanCppCode(code, self)
		for prop in self.staticProps:
			if hasattr(prop, 'scanCppCode'):
				prop.scanCppCode(code, self)
				
	def scanJsCode(self, code):
			for ctor in self.ctors:
				if hasattr(ctor, 'scanJsCode'):
					ctor.scanJsCode(code, self)
			for key in self.funcs:
				for func in self.funcs[key]:
					if hasattr(func, 'scanJsCode'):
						func.scanJsCode(code, self)
			for key in self.staticFuncs:
				for func in self.staticFuncs[key]:
					if hasattr(func, 'scanJsCode'):
						func.scanJsCode(code, self)
			for prop in self.props:
				if hasattr(prop, 'scanJsCode'):
					prop.scanJsCode(code, self)
			for prop in self.staticProps:
				if hasattr(prop, 'scanJsCode'):
					prop.scanJsCode(code, self)
		
builders = []

def parseFile(path, headerSearchPaths):
	classes = parseCPPHeader(path, headerSearchPaths, ["BIND_EMSCRIPTEN"], inCurrentFile = False)
	global builders
	for cls in classes:
		builders.append(JSClassBindingBuilder(cls))

def generateBindingCpp(path):
	printer = Printer()
	printer.printline('#include "api.h"')
	printer.printline('#include <emscripten/bind.h>')
	printer.printline('#include <emscripten/val.h>\n')
	printer.printline('#include "JSWrapper.h"')
	printer.printline('#include "SpineSkeletonRenderer.h"')
	printer.printline('#include "HLTransform2DComponent.h"')
	printer.printline('#include "HLShaderComponent.h"')
	printer.printline('#include "HLSkeletonComponent.h"')
	printer.printline('#include "HLParticleComponent.h"')
	printer.printline('#include "HLClassRegister.h"')
	printer.printline('#include "JSComponent.h"')
	printer.printline('using namespace emscripten;\n')
	printer.printline('using namespace hoolai;\n')
	printer.printline('using namespace hoolai::gui;\n')
	printer1 = Printer()
	printer1.printline('EMSCRIPTEN_BINDINGS(HL_BINDINGS_GENERATED) {')
	printer1.indent()
	for builder in builders:
		print 'generating %s'%builder.jsclsname
		if os.path.exists(os.path.join(path, "bindings_generated.cpp")):
			fp = open(os.path.join(path, "bindings_generated.cpp"))
			code = fp.read()
			fp.close()
			builder.scanCppCode(code)
		builder.generateBindingSource(printer, printer1)
	printer1.outdent()
	printer1.printline("}")
	f = open(os.path.join(path, "bindings_generated.cpp"), "w")
	f.write(printer._out + re.sub("\\n\\s*;", ";", printer1._out))
	f.close()
			
	
def generateBindingJs(path):
	printer = Printer()
	printer.printline('var hoolai = hoolai || {};')
	printer.printline('hoolai.gui = hoolai.gui || {};')
	printer.printline('(function() {')
	printer.indent()
	for builder in builders:
		if os.path.exists(os.path.join(path, "bindings_generated.js")):
			fp = open(os.path.join(path, "bindings_generated.js"))
			code = fp.read()
			fp.close()
			builder.scanJsCode(code)
		builder.generateBindingWrapper(printer)
	printer.outdent()
	printer.printline('})();')
	f = open(os.path.join(path, 'bindings_generated.js'), 'w')
	f.write(printer._out)
	f.close()

if __name__=='__main__':
	parser = argparse.ArgumentParser(description='JSBindingGenerator')
	parser.add_argument('inputs', nargs='+', help='input header file')
	parser.add_argument('-o', '--out', required=True, help='output path')
	args = parser.parse_args()
	
	pathname = os.path.dirname(sys.argv[0])
	path, _ = os.path.split(os.path.realpath(__file__))
	
	outPath = os.path.abspath(os.path.join(path, args.out))

	fp = open(os.path.join(path, 'emconfig.json'), 'r')
	config = json.load(fp)
	fp.close()
	classesmap = config['classesmap']
	mannualcode = config['mannualcode']
	enumsmap = config['enumsmap']
	headerSearchPaths = map(lambda x:os.path.abspath(os.path.join(path, x)), config['headerSearchPaths'])
#	headerSearchPaths.append(os.path.join(os.environ['EMSCRIPTEN'], 'system', 'include'))
	print 'parsing headers...'
	for f in args.inputs:
		parseFile(os.path.abspath(os.path.join(path, f)), headerSearchPaths)
	print 'generating files...'
	generateBindingCpp(outPath);
	generateBindingJs(outPath);
		
	fp = open(os.path.join(path, 'emconfig.json'), 'w')
	json.dump(config, fp, indent=4)
	fp.close()