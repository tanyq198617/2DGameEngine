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

def findClassInfo(className, currentNamespace):
	if className in classesmap:
		return classesmap[className]
	if len(className) > 2 and className.startswith('::'):
		className = className[2:]
		if className in classesmap:
			return classesmap[className]
		else:
			return None
	return None

def JSValueMakeNumberCode(varname, type):
	if type.spelling != 'double':
		return 'JSValueMakeNumber(ctx, (double)%s)'%varname
	else:
		return 'JSValueMakeNumber(ctx, %s)'%varname

def JSValueToNumberCode(varname, type):
	if type.spelling != 'double':
		return '(%s)JSValueToNumber(ctx, %s, exception)'%(type.spelling, varname)
	else:
		return 'JSValueToNumber(ctx, %s, exception)'%varname

def JSValueTypeCheckCode(varname, type, builder):
	if type.isNumber() or type.isEnum():
		return 'JSValueIsNumber(ctx, %s)'%varname
	elif type.isString():
		return 'JSValueIsString(ctx, %s)'%varname
#	elif type.isBool():
#		return '(JSValueIsBoolean(ctx, %s) || JSValueIsNull(ctx, %s) || JSValueIsUndefined(ctx, %s))'%(varname, varname, varname)
	elif type.isVector() or type.isList():
		return 'JSValueIsArray(ctx, %s)'%varname
	elif type.isPointer() and findClassInfo(type.ctype.get_pointee().spelling, builder.namespace):
		info = findClassInfo(type.ctype.get_pointee().spelling, builder.namespace)
		return '(JSValueIsNull(ctx, %s) || JSValueIsInstanceOfConstructor(ctx, %s, %s::proto, exception))'%(varname, varname, info['namespace']+info['wrapperclass'])
	return None

def printValueToJSValueCode(printer, template, accessor, type, builder):
	if type.isEnum():
		printer.printline(template%('hoolai::value_to_jsval(ctx, (int)%s)'%accessor))
	elif type.isPointer() and type.spellingNoPointer in classesmap:
		spelling = type.spellingNoPointer
		wrapper = classesmap[spelling]['namespace'] + classesmap[spelling]['wrapperclass']
		if spelling == 'hoolai::gui::HLView':
			printer.printline(template%('hoolai::HLView_to_jsval(ctx, %s)'%accessor))
		else:
			printer.printline(template%('hoolai::value_to_jsval1<%s, %s>(ctx, %s)'%(wrapper, type.ctype.get_pointee().spelling, accessor)))
		include = '#include "%s.h"'%classesmap[spelling]['wrapperclass']
		if include not in builder.includes:
			builder.includes.append(include)
	elif type.isVector() or type.isList():
		printer.printline('{')
		printer.indent();
		printer.printline('%s ret = %s;'%(type.spelling, accessor))
		printer.printline('size_t elementCount = ret.size();')
		printer.printline('JSValueRef elements[elementCount];')
		printer.printline('size_t index = 0;')
		printer.printline('for(auto itr = ret.begin(); itr != ret.end(); ++itr)\n{')
		printer.indent()
		printValueToJSValueCode(printer, 'elements[index++] = %s;', '(*itr)', type.getTemplateArgs()[0], builder)
		printer.outdent()
		printer.printline('}')
		printer.printline('JSValueRef exception = nullptr;')
		printer.printline('JSObjectRef retObj = JSObjectMakeArray(ctx, elementCount, elements, &exception);')
		printer.printline('CHECK_ERROR()')
		printer.printline(template%'retObj')
		printer.outdent()
		printer.printline('}')
	else:
		printer.printline(template%('hoolai::value_to_jsval(ctx, %s)'%accessor))

def printJSValueToValueCode(printer, jsValue, assignTo, type, builder):
	if type.isEnum():
		printer.printline('%s = (%s)hoolai::jsval_to_value<int>(ctx, %s);'%(assignTo, type.spelling, jsValue))
	elif type.isPointer() and type.ctype.get_pointee().spelling in classesmap:
		wrapper = classesmap[type.ctype.get_pointee().spelling]['namespace'] + classesmap[type.ctype.get_pointee().spelling]['wrapperclass']
		printer.printline('%s = hoolai::jsval_to_value1<%s, %s>(ctx, %s);'%(assignTo, wrapper, type.ctype.get_pointee().spelling, jsValue))
		include = '#include "%s.h"'%classesmap[type.ctype.get_pointee().spelling]['wrapperclass']
		if include not in builder.includes:
			builder.includes.append(include)
	elif type.isVector() or type.isList():
		printer.printline('{')
		printer.indent()
		printer.printline('JSValueRef exception = nullptr;')
		printer.printline('JSObjectRef obj = JSValueToObject(ctx, %s, &exception);'%jsValue)
		printer.printline('CHECK_ERROR()')
		printer.printline('JSStringRef lenProp = JSStringCreateWithUTF8CString("length");')
		printer.printline('JSValueRef lenVal = JSObjectGetProperty(ctx, obj, lenProp, &exception);')
		printer.printline('CHECK_ERROR()')
		printer.printline('JSStringRelease(lenProp);')
		printer.printline('int len = JSValueToNumber(ctx, lenVal, &exception);')
		printer.printline('CHECK_ERROR()')
		printer.printline('for (int i = 0; i < len; ++i)\n{')
		printer.indent()
		printer.printline('JSValueRef val = JSObjectGetPropertyAtIndex(ctx, obj, i, &exception);')
		printer.printline('CHECK_ERROR()')
		refType = type.getTemplateArgs()[0]
		printer.printline('%s nativeVal;'%refType.spelling)
		printJSValueToValueCode(printer, 'val', 'nativeVal', refType, builder)
		printer.printline('%s.push_back(nativeVal);'%assignTo)
		printer.outdent()
		printer.printline('}')
		printer.outdent()
		printer.printline('}')
	else:
		printer.printline('%s = hoolai::jsval_to_value<%s>(ctx, %s);'%(assignTo, type.spellingNoReference, jsValue))

def printExceptionCode(printer, message):
	printer.printline('JSStringRef messageStr = JSStringCreateWithUTF8CString("%s");'%message)
	printer.printline('JSValueRef message = JSValueMakeString(ctx, messageStr);')
	printer.printline('*exception = JSObjectMakeError(ctx, 1, &message, nullptr);')
	printer.printline('JSStringRelease(messageStr);')

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
		self.priority = self.paramNumber
		i = 0
		for param in self.parameters:
			i += 1
			if JSValueTypeCheckCode('test', param.type, builder) == None:
				self.priority = i
				break
			
		
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
		for param in self.parameters:
			argnames.append(param.name)
			if param.default:
				printer.printline('%s %s = %s;'%(param.type.spelling, param.name, param.default))
			else:
				printer.printline('%s %s;'%(param.type.spellingNoReference, param.name))
				
		for i in range(0, self.minParamNumber):
			checkCode = JSValueTypeCheckCode('arguments[%s]'%i, self.parameters[i].type, builder)
			if checkCode is not None:
				printer.printline('if (!%s)'%checkCode)
				printer.indent()
				printer.printline('break;')
				printer.outdent()
			printJSValueToValueCode(printer, 'arguments[%d]'%i, self.parameters[i].name, self.parameters[i].type, builder)
		
		for i in range(self.minParamNumber, self.paramNumber):
			printer.printline('if (argumentCount > %d)\n{'%i)
			printer.indent()
			checkCode = JSValueTypeCheckCode('arguments[%s]'%i, self.parameters[i].type, builder)
			if checkCode is not None:
				printer.printline('if (!%s)'%checkCode)
				printer.indent()
				printer.printline('break;')
				printer.outdent()
			printJSValueToValueCode(printer, 'arguments[%d]'%i, self.parameters[i].name, self.parameters[i].type, builder)
		
		for i in range(self.minParamNumber, self.paramNumber):
			printer.outdent()
			printer.printline('}')
			
		return argnames

class JSCtor(JSFuncBase):
	def __init__(self, cppMethod, builder):
		JSFuncBase.__init__(self, cppMethod, builder)
		self.name = 'ctor'
	
	def genSource(self, printer, builder):
		if self.paramNumber > 0:
			printer.printline('do {')
			printer.indent();
			argnames = self._genParamsCode(printer, builder)
			
			printer.printline('auto nativeInstance = new %s(%s);'%(builder.cppclsname, ', '.join(argnames)))
			printer.printline('return getOrCreateWrapper(ctx, nativeInstance)->jsobject;')
			printer.outdent()
			printer.printline('}')
			printer.outdent()
			printer.printline('} while(0);')
			return True
		else:
			printer.printline('auto nativeInstance = new %s();'%builder.cppclsname)
			printer.printline('return getOrCreateWrapper(ctx, nativeInstance)->jsobject;')
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
		
	def genGetterSource(self, printer, builder):
		accessCode = ''
		if not self.static:
			if self.cppGetterFunc:
				accessCode = 'thiz->%s()'%self.cppGetterFunc
			else:
				accessCode = 'thiz->%s'%self.cppname
		else:
			if self.cppGetterFunc:
				accessCode = '%s::%s()'%(builder.cppclsname, self.cppGetterFunc)
			else:
				accessCode = '%s::%s'%(builder.cppclsname, self.cppname)
		codeTemplate = 'return %s;'
		printValueToJSValueCode(printer, codeTemplate, accessCode, self.valueType, builder)
				
	def genSetterSource(self, printer, builder):
		checkCode = JSValueTypeCheckCode('value', self.valueType, builder)
		if checkCode is not None:
			printer.printline('if (!%s)\n{'%checkCode)
			printer.indent();
			printExceptionCode(printer, '%s.%s: wrong type provided to the property'%(builder.jsclsname, self.name))
			printer.printline('return false;')
			printer.outdent();
			printer.printline('}')
		printer.printline('%s nativeValue;'%self.valueType.spelling)
		printJSValueToValueCode(printer, 'value', 'nativeValue', self.valueType, builder)
		if self.static == 0:
			if self.cppSetterFunc:
				printer.printline('thiz->%s(nativeValue);'%self.cppSetterFunc)
			else:
				if self.valueType.isVector() or self.valueType.isList():
					printer.printline('this->%s.swap(nativeValue);')
				else:
					printer.printline('thiz->%s = nativeValue;'%self.cppname)
		else:
			if self.cppSetterFunc:
				printer.printline('%s::%s(nativeValue);'%(builder.cppclsname, self.cppSetterFunc))
			else:
				if self.valueType.isVector() or self.valueType.isList():
					printer.printline('%s::%s.swap(nativeValue);'%(builder.cppclsname, self.cppname))
				else:
					printer.printline('%s::%s = nativeValue;'%(builder.cppclsname, self.cppname))
		printer.printline('return true;')
		
class JSFunc(JSFuncBase):
	def __init__(self, cppMethod, builder):
		JSFuncBase.__init__(self, cppMethod, builder)
		self.name = cppMethod.name
		self.originName = self.name
		if cppMethod.funcname:
			self.name = cppMethod.funcname
		self.cppname = 'js%s'%capitalize(self.name)
		self.static = cppMethod.static
		self.returnType = cppMethod.returnType
	
	def genSource(self, printer, builder):
		fullclsname = builder.namespace + builder.cppclsname
		if fullclsname in mannualcode:
			if self.name in mannualcode[fullclsname]:
				printer.printline('//!impl_%s'%self.name)
				printer.printline(mannualcode[fullclsname][self.name])
				printer.printline('//!impl_%s_end'%self.name)
				return False
		if self.static == 0:
			if builder.singleton:
				accessor = 'thiz%s%s'%(builder.singletonAccessor, self.originName)
			else:
				accessor = 'thiz->%s'%self.originName
		else:
			accessor = '%s::%s'%(builder.cppclsname, self.originName)
		
		if self.paramNumber > 0:
			printer.printline('do {')
			printer.indent();
			argnames = self._genParamsCode(printer, builder)
			if self.returnType.isVoid():
				printer.printline('%s(%s);'%(accessor, ', '.join(argnames)))
				printer.printline('return nullptr;')
			else:
#				printer.printline('%s ret = %s(%s);'%(self.returnType.spelling, accessor, ', '.join(argnames)))
				printValueToJSValueCode(printer, 'return %s;', '%s(%s)'%(accessor, ', '.join(argnames)), self.returnType, builder)
			printer.outdent()
			printer.printline('}')
			printer.outdent();
			printer.printline('} while(0);')
			return True
		else:
			if self.returnType.isVoid():
				printer.printline('%s();'%accessor)
				printer.printline('return nullptr;')
			else:
				printValueToJSValueCode(printer, 'return %s;', '%s()'%accessor, self.returnType, builder)
			return False

	def scanCode(self, code, builder):
		fullclsname = builder.namespace + builder.cppclsname
		if fullclsname in mannualcode:
			if self.name in mannualcode[fullclsname]:
				result = re.findall('//!impl_%s\\n(.*?)//!impl_%s_end'%(self.name, self.name), code, re.S)
				if len(result):
					code = result[0].rstrip()
					mannualcode[fullclsname][self.name] = processCodeLines(code)

class CustomCtor:
	def __init__(self, config):
		self.config = config
		self.priority = 0
		self.minParamNumber = 0
		self.paramNumber = 0
		self.name = 'ctor'
		
	def genSource(self, printer, builder):
		printer.printline('//!ctor')
		printer.printline(self.config['constructor'])
		printer.printline('//!ctor_end')
		
	def scanCode(self, code, builder):
		result = re.findall('//!ctor\\n(.*?)//!ctor_end', code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.code = processCodeLines(code)
			self.config['constructor'] = self.code

class CustomProp:
	def __init__(self, name, config):
		self.name = name
		self.config = config
		self.readonly = False
		if 'setter' not in self.config:
			self.readonly = True
		
	def genGetterSource(self, printer, builder):
		if 'getter' in self.config:
			printer.printline('//!getter_%s'%self.name)
			printer.printline(self.config['getter'])
			printer.printline('//!getter_%s_end'%self.name)
		
	def genSetterSource(self, printer, builder):
		if 'setter' in self.config:
			printer.printline('//!setter_%s'%self.name)
			printer.printline(self.config['setter'])
			printer.printline('//!setter_%s_end'%self.name)

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
	def __init__(self, name, config):
		self.name = name
		self.code = config[name]
		self.config = config
		self.cppname = 'js%s'%capitalize(self.name)
		self.minParamNumber = 0
		self.paramNumber = 0
		self.priority = 0
		
	def genSource(self, printer, builder):
		printer.printline('//!func_%s'%self.name)
		printer.printline(self.code)
		printer.printline('//!func_%s_end'%self.name)
		return False
		
	def scanCode(self, code, builder):
		result = re.findall('//!func_%s\\n(.*?)//!func_%s_end'%(self.name, self.name), code, re.S)
		if len(result):
			code = result[0].rstrip()
			self.code = processCodeLines(code)
			self.config[self.name] = self.code

def capitalize(str):
	return str[0].upper() + str[1:] if len(str) > 0 else str

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
				self.name = 'set'+capitalize(self.propName)
			elif self.type == 'GETTER':
				self.name = 'get'+capitalize(self.propName)
			elif self.type == 'DELEGATEADD':
				self.name = 'add'+capitalize(self.propName)
			else:
				self.name = 'remove'+capitalize(self.propName)
		self.cppname = 'js%s'%capitalize(self.name)
		
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
			
		self.singleton = cls.singleton
		if self.singleton:
			self.singletonFunc = cls.singletonFuncName
			if cls.singletonFunc.returnType.isPointer():
				self.singletonAccessor = '->'
			else:
				self.singletonAccessor = '.'
		self.jsclsname = cls.jsclsname
		self.cppclsname = cls.name
		self.bindingclsname = 'JS%s'%self.jsclsname
		
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
					self.staticProps.append(CustomProp(prop, props[prop]))
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
					self.staticFuncs[func].append(CustomFunc(func, funcs))
					
		self.numFuncs = len(self.funcNames)
		self.numStaticFuncs = len(self.staticFuncs.keys())
		
	def generateBindingHeader(self, path):
		printer = Printer()
		printer.printlines('#pragma once\n', '#include "JSCPPWrapper.h"\n')
		
		for namespace in self.namespaces:
			printer.printline('namespace %s\n{'%namespace)
			
		printer.printline('\nclass %s;\n'%self.cppclsname)
		if self.singleton:
			printer.printline('class %s\n{'%self.bindingclsname)
		else:
			printer.printline('class %s: public hoolai::JSCPPWrapper<%s, %s>\n{'%(self.bindingclsname, self.bindingclsname, self.cppclsname))
		printer.printline('public:')
		printer.indent()
		printer.printline('static void jsCreateClass(JSContextRef ctx, JSObjectRef global);')
		if len(self.ctors) > 0:
			printer.printline('static JSObjectRef jsConstructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);')
		if self.numStaticFuncs > 0 or len(self.staticProps) > 0:
			printer.printline('static bool jsHasInstance(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);')
		if len(self.props) > 0 or len(self.staticProps) > 0:
			printer.printline('static JSValueRef jsGetProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception);')
			printer.printline('static bool jsSetProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception);')
		for key in self.staticFuncNames:
			func = self.staticFuncs[key][0]
			printer.printline('static JSValueRef %s(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);'%func.cppname)
		for key in self.funcNames:
			func = self.funcs[key][0]
			printer.printline('static JSValueRef %s(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);'%func.cppname)
		printer.outdent()
		printer.printline('};\n')
		
		for namespace in self.namespaces:
			printer.printline('}')
		
		fp = open(os.path.join(path, '%s.h'%self.bindingclsname), 'w')
		fp.write(printer._out)
		fp.close()
		
	def _generateNSParentCode(self, printer):
		printer.printline('JSObjectRef parent = global;')
		printer.printline('JSValueRef exception = nullptr;')
		for ns in self.namespaces:
			printer.printline('JSStringRef %sPropName = JSStringCreateWithUTF8CString("%s");'%(ns, ns))
			printer.printline('if (JSObjectHasProperty(ctx, parent, %sPropName))\n{'%ns)
			printer.indent()
			printer.printline('JSValueRef %sValue = JSObjectGetProperty(ctx, parent, %sPropName, &exception);'%(ns, ns))
			printer.printline('CHECK_ERROR()')
			printer.printline('parent = JSValueToObject(ctx, %sValue, &exception);'%ns)
			printer.printline('CHECK_ERROR()')
			printer.outdent()
			printer.printline('}\nelse\n{')
			printer.indent()
			printer.printline('JSObjectRef %sObj = JSObjectMake(ctx, nullptr, nullptr);'%ns)
			printer.printline('JSObjectSetProperty(ctx, parent, %sPropName, %sObj, kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete, &exception);'%(ns, ns))
			printer.printline('CHECK_ERROR()')
			printer.printline('parent = %sObj;'%ns)
			printer.outdent()
			printer.printline('}')
			printer.printline('JSStringRelease(%sPropName);'%ns)
			printer.printline('if (!parent) return;')
	
	def _generateCreateClassImpl(self, printer):
		printer.printline('void %s::jsCreateClass(JSContextRef ctx, JSObjectRef global)\n{'%self.bindingclsname)
		printer.indent()
		if self.singleton:
			printer.printline('static JSClassRef jsclass = nullptr;')
			printer.printline('if (!jsclass)\n{')
			printer.indent()
			printer.printline('JSClassDefinition classDef = kJSClassDefinitionEmpty;')
			printer.printline('classDef.className = "%s";'%self.jsclsname)
			if len(self.props) > 0:
				printer.printline('JSStaticValue props[] = \n{')
				printer.indent();
				for prop in self.props:
					if prop.readonly:
						printer.printline('{ "%s", &%s::jsGetProperty, nullptr, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(prop.name, self.bindingclsname))
					else:
						printer.printline('{ "%s", &%s::jsGetProperty, &%s::jsSetProperty, kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(prop.name, self.bindingclsname, self.bindingclsname))
				printer.printline('{ 0, 0, 0, 0 }');
				printer.outdent();
				printer.printline('};')
				printer.printline('classDef.staticValues = props;')
			if self.numFuncs > 0:
				printer.printline('JSStaticFunction funcs[] = \n{')
				printer.indent()
				for key in self.funcs:
					func = self.funcs[key][0]
					printer.printline('{ "%s", &%s::%s, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(func.name, self.bindingclsname, func.cppname))
				printer.printline('{ 0, 0, 0 }')
				printer.outdent()
				printer.printline('};')
				printer.printline('classDef.staticFunctions = funcs;')
				printer.printline('jsclass = JSClassCreate(&classDef);')
				printer.outdent()
				printer.printline('}')
				printer.printline('JSObjectRef obj = JSObjectMake(ctx, jsclass, nullptr);')
				parent = 'global'
				if len(self.namespaces) != 0:
					self._generateNSParentCode(printer)
					parent = 'parent'
				printer.printline('JSStringRef className = JSStringCreateWithUTF8CString("%s");'%self.jsclsname)
				printer.printline('JSObjectSetProperty(ctx, %s, className, obj, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete, nullptr);'%parent)
				printer.printline('JSStringRelease(className);')
		else:
			printer.printline('if (!jsclass)\n{')
			printer.indent()
			printer.printline('JSClassDefinition classDef = kJSClassDefinitionEmpty;')
			printer.printline('classDef.className = "%s";'%self.jsclsname)
			if self.parentInfo:
				printer.printline('classDef.parentClass = %s%s::jsclass;'%(self.parentInfo['namespace'], self.parentInfo['wrapperclass']))
			if len(self.props) > 0:
				printer.printline('JSStaticValue props[] = \n{')
				printer.indent();
				for prop in self.props:
					if prop.readonly:
						printer.printline('{ "%s", &%s::jsGetProperty, nullptr, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(prop.name, self.bindingclsname))
					else:
						printer.printline('{ "%s", &%s::jsGetProperty, &%s::jsSetProperty, kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(prop.name, self.bindingclsname, self.bindingclsname))
				printer.printline('{ 0, 0, 0, 0 }');
				printer.outdent();
				printer.printline('};')
				printer.printline('classDef.staticValues = props;')
			if self.numFuncs > 0:
				printer.printline('JSStaticFunction funcs[] = \n{')
				printer.indent()
				for key in self.funcs:
					func = self.funcs[key][0]
					printer.printline('{ "%s", &%s::%s, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(func.name, self.bindingclsname, func.cppname))
				printer.printline('{ 0, 0, 0 }')
				printer.outdent()
				printer.printline('};')
				printer.printline('classDef.staticFunctions = funcs;')
			printer.printline('jsclass = JSClassCreate(&classDef);')
			printer.outdent()
			printer.printline('}')
			
			if len(self.staticProps) > 0 or self.numStaticFuncs > 0:
				printer.printline('static JSClassRef ctorClass = nullptr;')
				printer.printline('if (!ctorClass)\n{')
				printer.indent()
				printer.printline('JSClassDefinition ctorClassDef = kJSClassDefinitionEmpty;')
				printer.printline('ctorClassDef.className = "%sConstructor";'%self.jsclsname);
				printer.printline('ctorClassDef.hasInstance = &%s::jsHasInstance;'%self.bindingclsname)
				if len(self.staticProps) > 0:
					printer.printline('JSStaticValue st_props[] = \n{')
					printer.indent();
					for prop in self.staticProps:
						if prop.readonly:
							printer.printline('{ %s, &%s::jsGetProperty, nullptr, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(prop.name, self.bindingclsname))
						else:
							printer.printline('{ %s, &%s::jsGetProperty, &%s::jsSetProperty, kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(prop.name, self.bindingclsname, self.bindingclsname))
					printer.printline('{ 0, 0, 0, 0 }');
					printer.outdent();
					printer.printline('};')
					printer.printline('ctorClassDef.staticValues = st_props;')
				if self.numStaticFuncs > 0:
					printer.printline('JSStaticFunction st_funcs[] = \n{')
					printer.indent()
					for key in self.staticFuncs:
						func = self.staticFuncs[key][0]
						printer.printline('{ "%s", &%s::%s, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete },'%(func.name, self.bindingclsname, func.cppname))
					printer.printline('{ 0, 0, 0 }')
					printer.outdent()
					printer.printline('};')
					printer.printline('ctorClassDef.staticFunctions = st_funcs;')
				if len(self.ctors) == 0:
					printer.printline('ctorClassDef.callAsConstructor = hoolai::jsNoConstructor;')
				else:
					printer.printline('ctorClassDef.callAsConstructor = &%s::jsConstructor;'%self.bindingclsname)
				printer.printline('ctorClass = JSClassCreate(&ctorClassDef);')
				printer.outdent();
				printer.printline('}')
				printer.printline('proto = JSObjectMake(ctx, ctorClass, nullptr);')
				if len(self.ctors) > 0:
					printer.printline('JSStringRef nameprop = JSStringCreateWithUTF8CString("name");')
					printer.printline('JSStringRef nameval = JSStringCreateWithUTF8CString("%s");'%('.'.join(self.namespaces) + '.' + self.jsclsname))
					printer.printline('JSObjectSetProperty(ctx, proto, nameprop, JSValueMakeString(ctx, nameval), kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete, nullptr);')
					printer.printline('JSStringRelease(nameprop);')
					printer.printline('JSStringRelease(nameval);')
			else:
				if len(self.ctors) == 0:
					printer.printline('proto = JSObjectMakeConstructor(ctx, jsclass, hoolai::jsNoConstructor);')
				else:
					printer.printline('proto = JSObjectMakeConstructor(ctx, jsclass, &%s::jsConstructor);'%self.bindingclsname)
					printer.printline('JSStringRef nameprop = JSStringCreateWithUTF8CString("name");')
					printer.printline('JSStringRef nameval = JSStringCreateWithUTF8CString("%s");'%('.'.join(self.namespaces) + '.' + self.jsclsname))
					printer.printline('JSObjectSetProperty(ctx, proto, nameprop, JSValueMakeString(ctx, nameval), kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete, nullptr);')
					printer.printline('JSStringRelease(nameprop);')
					printer.printline('JSStringRelease(nameval);')
			parent = 'global'
			if len(self.namespaces) != 0:
				self._generateNSParentCode(printer)
				parent = 'parent'
			printer.printline('JSStringRef className = JSStringCreateWithUTF8CString("%s");'%self.jsclsname)
			printer.printline('JSObjectSetProperty(ctx, %s, className, proto, kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontEnum|kJSPropertyAttributeDontDelete, nullptr);'%parent)
			printer.printline('JSStringRelease(className);')
		printer.outdent()
		printer.printline('}')
		
	def _generateConstructorImpl(self, printer):
		printer.printline('JSObjectRef %s::jsConstructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)\n{'%self.bindingclsname)
		printer.indent()

		genExceptionCode = True
		for ctor in self.ctors:
			if not ctor.genSource(printer, self):
				genExceptionCode = False
				break
		if genExceptionCode:
			printExceptionCode(printer, 'Wrong arguments passed to the constructor %s'%self.jsclsname)
			printer.printline('return nullptr;')
		printer.outdent()
		printer.printline('}')
		
	def _generateHasInstanceImpl(self, printer):
		printer.printline('bool %s::jsHasInstance(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception)\n{'%self.bindingclsname)
		printer.indent()
		printer.printline('return JSValueIsObjectOfClass(ctx, possibleInstance, jsclass);')
		printer.outdent()
		printer.printline('}')
		
	def _generatePropertyGetter(self, printer):
		printer.printline('JSValueRef %s::jsGetProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)\n{'%self.bindingclsname)
		printer.indent()
		genElse = False
		if len(self.props) > 0:
			if self.singleton:
				if self.singletonAccessor == '.':
					printer.printline('auto& thiz = %s::%s();'%(self.cppclsname, self.singletonFunc))
				else:
					printer.printline('auto thiz = %s::%s();'%(self.cppclsname, self.singletonFunc))
			else:
				printer.printline('if (JSValueIsObjectOfClass(ctx, object, jsclass))\n{')
				printer.indent()
				printer.printline('auto thizwrapper = (%s*)JSObjectGetPrivate(object);'%self.bindingclsname)
				printer.printline('if (!thizwrapper)\n{')
				printer.indent()
				printExceptionCode(printer, '%s.getProperty: wrong \'this\' object, native object may be freed'%self.jsclsname)
				printer.printline('return nullptr;')
				printer.outdent()
				printer.printline('}')
				printer.printline('auto thiz = thizwrapper->getNativeObject();')
			for prop in self.props:
				if genElse:
					printer.printline('else if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
				else:
					printer.printline('if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
					genElse = True
				printer.indent()
				prop.genGetterSource(printer, self)
				printer.outdent()
				printer.printline('}')
			if not self.singleton:
				printer.outdent()
				printer.printline('}')
		genElse = False
		for prop in self.staticProps:
			if genElse:
				printer.printline('else if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
			else:
				printer.printline('if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
				genElse = True
			printer.indent()
			prop.genGetterSource(printer, self)
			printer.outdent()
			printer.printline('}')
		printer.printline('return nullptr;')
		printer.outdent()
		printer.printline('}')
		
	def _generatePropertySetter(self, printer):
		printer.printline('bool %s::jsSetProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)\n{'%self.bindingclsname)
		printer.indent()
		genElse = False
		props = [x for x in self.props if not x.readonly]
		if len(props) > 0:
			if self.singleton:
				if self.singletonAccessor == '.':
					printer.printline('auto& thiz = %s::%s();'%(self.cppclsname, self.singletonFunc))
				else:
					printer.printline('auto thiz = %s::%s();'%(self.cppclsname, self.singletonFunc))
			else:
				printer.printline('if (JSValueIsObjectOfClass(ctx, object, jsclass))\n{')
				printer.indent()
				printer.printline('auto thizwrapper = (%s*)JSObjectGetPrivate(object);'%self.bindingclsname)
				printer.printline('if (!thizwrapper)\n{')
				printer.indent()
				printExceptionCode(printer, '%s.getProperty: wrong \'this\' object, native object may be freed'%self.jsclsname)
				printer.printline('return false;')
				printer.outdent()
				printer.printline('}')
				printer.printline('auto thiz = thizwrapper->getNativeObject();')
			for prop in props:
				if genElse:
					printer.printline('else if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
				else:
					printer.printline('if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
					genElse = True
				printer.indent()
				prop.genSetterSource(printer, self)
				printer.outdent()
				printer.printline('}')
			if not self.singleton:
				printer.outdent()
				printer.printline('}')
		genElse = False
		staticProps = [x for x in self.staticProps if not x.readonly]
		for prop in staticProps:
			if genElse:
				printer.printline('else if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
			else:
				printer.printline('if (JSStringIsEqualToUTF8CString(propertyName, "%s"))\n{'%prop.name)
				genElse = True
			printer.indent()
			prop.genSetterSource(printer, self)
			printer.outdent()
			printer.printline('}')
		printer.printline('return false;')
		printer.outdent()
		printer.printline('}')
	
	def generateBindingSource(self, path):
		if os.path.exists(os.path.join(path, '%s.cpp'%self.bindingclsname)):
			fp = open(os.path.join(path, '%s.cpp'%self.bindingclsname))
			code = fp.read()
			fp.close()
			for ctor in self.ctors:
				if hasattr(ctor, 'scanCode'):
					ctor.scanCode(code, self)
			for key in self.funcs:
				for func in self.funcs[key]:
					if hasattr(func, 'scanCode'):
						func.scanCode(code, self)
			for key in self.staticFuncs:
				for func in self.staticFuncs[key]:
					if hasattr(func, 'scanCode'):
						func.scanCode(code, self)
			for prop in self.props:
				if hasattr(prop, 'scanCode'):
					prop.scanCode(code, self)
			for prop in self.staticProps:
				if hasattr(prop, 'scanCode'):
					prop.scanCode(code, self)
		
		printer = Printer()
		self.includes = []
		self.includes.append('#include "JSConversions.h"')
		self.includes.append('#include "%s.h"'%self.bindingclsname)
		self.includes.append('#include "%s"'%self.cppheader)
		self.parentInfo = None
		printer.printline('namespace hoolai\n{')
		printer.printline('template<> JSClassRef JSCPPWrapper<%s, %s>::jsclass = nullptr;'%(self.namespace+self.bindingclsname, self.namespace+self.cppclsname))
		printer.printline('template<> JSObjectRef JSCPPWrapper<%s, %s>::proto = nullptr;'%(self.namespace+self.bindingclsname, self.namespace+self.cppclsname))
		printer.printline('}\n')
		
		if self.parentClass:
			classInfo = findClassInfo(self.parentClass, self.namespace)
			if classInfo:
				self.parentInfo = classInfo
				self.includes.append('#include "%s.h"'%classInfo['wrapperclass'])
		
		for namespace in self.namespaces:
			printer.printline('namespace %s\n{'%namespace)
					
		printer.printline('')
		
		self._generateCreateClassImpl(printer)
		
		printer.printline('')
		
		if len(self.ctors):
			self._generateConstructorImpl(printer)
			printer.printline('')
		
		if len(self.staticProps) > 0 or self.numStaticFuncs > 0:
			self._generateHasInstanceImpl(printer)
			printer.printline('')
			
		if len(self.props) > 0 or len(self.staticProps) > 0:
			self._generatePropertyGetter(printer)
			printer.printline('')
			self._generatePropertySetter(printer)
			printer.printline('')
			
			
		for key in self.staticFuncNames:
			self.staticFuncs[key].sort(compFunc)
			checkFuncs(self.staticFuncs[key])
			func = self.staticFuncs[key][0]
			printer.printline('JSValueRef %s::%s(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)\n{'%(self.bindingclsname, func.cppname))
			printer.indent()
			genExceptionCode = True;
			for fun in self.staticFuncs[key]:
				if not fun.genSource(printer, self):
					genExceptionCode = False;
					break;
			if genExceptionCode:
				printExceptionCode(printer, '%s.%s: invalid arguments'%(self.jsclsname, func.name))
				printer.printline('return nullptr;')
			printer.outdent()
			printer.printline('}\n')
		for key in self.funcNames:
			self.funcs[key].sort(compFunc)
			checkFuncs(self.funcs[key])
			func = self.funcs[key][0]
			printer.printline('JSValueRef %s::%s(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)\n{'%(self.bindingclsname, func.cppname))
			printer.indent()
			if self.singleton:
				if self.singletonAccessor == '.':
					printer.printline('auto& thiz = %s::%s();'%(self.cppclsname, self.singletonFunc))
				else:
					printer.printline('auto thiz = %s::%s();'%(self.cppclsname, self.singletonFunc))
			else:
				printer.printline('auto thizwrapper = (%s*)JSObjectGetPrivate(thisObject);'%self.bindingclsname)
				printer.printline('if (!thizwrapper)\n{')
				printer.indent()
				printExceptionCode(printer, '%s.%s: wrong \'this\' object, native object may be freed'%(self.jsclsname, func.name))
				printer.printline('return nullptr;')
				printer.outdent()
				printer.printline('}')
				printer.printline('auto thiz = thizwrapper->getNativeObject();')
			genExceptionCode = True;
			for fun in self.funcs[key]:
				if not fun.genSource(printer, self):
					genExceptionCode = False
					break;
			if genExceptionCode:
				printExceptionCode(printer, '%s.%s: invalid arguments'%(self.jsclsname, func.name))
				printer.printline('return nullptr;')
			printer.outdent()
			printer.printline('}\n')
		
		for namespace in self.namespaces:
			printer.printline('}')
		
		fp = open(os.path.join(path, '%s.cpp'%self.bindingclsname), 'w')
		fp.write('\n'.join(self.includes))
		fp.write('\n\n')
		if len(self.customincludes) > 0:
			fp.write('\n'.join(self.customincludes))
			fp.write('\n\n')
		fp.write(printer._out)
		fp.close()
		
	def generateTSDeclaration(self):
		pass
		
builders = []

def parseFile(path, headerSearchPaths):
	classes = parseCPPHeader(path, headerSearchPaths)
	global builders
	for cls in classes:
		builders.append(JSClassBindingBuilder(cls))

if __name__=='__main__':
	parser = argparse.ArgumentParser(description='JSBindingGenerator')
	parser.add_argument('inputs', nargs='+', help='input header file')
	parser.add_argument('-o', '--out', required=True, help='output path')
	args = parser.parse_args()
	
	pathname = os.path.dirname(sys.argv[0])
	path, _ = os.path.split(os.path.realpath(__file__))
	
	outPath = os.path.abspath(os.path.join(path, args.out))

	fp = open(os.path.join(path, 'config.json'), 'r')
	config = json.load(fp)
	fp.close()
	classesmap = config['classesmap']
	mannualcode = config['mannualcode']
	headerSearchPaths = map(lambda x:os.path.abspath(os.path.join(path, x)), config['headerSearchPaths'])
	print 'parsing headers...'
	for f in args.inputs:
		parseFile(os.path.abspath(os.path.join(path, f)), headerSearchPaths)
	print 'generating files...'
	for builder in builders:
		print 'generating %s'%builder.jsclsname
		builder.generateBindingHeader(outPath)
		builder.generateBindingSource(outPath)
		
	fp = open(os.path.join(path, 'config.json'), 'w')
	json.dump(config, fp, indent=4)
	fp.close()