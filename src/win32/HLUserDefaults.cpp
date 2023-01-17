#include "HLUserDefaults.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "HLResourceManager.h"
#include "HLObject.h"
#include <io.h>
#include <fstream>

NS_HL_BEGIN

static rapidjson::Document m_userdefault;

void initUserDefaults()
{
	static bool inited = false;
	if (!inited)
	{
		inited = true;
		std::string path = HLResourceManager::getSingleton()->getWritablePath();
		path += "userdefaults.json";
		if (access(path.c_str(), 0) == 0)
		{
			HLString *str = HLString::stringWithContentsOfFile(path.c_str());
			m_userdefault.Parse(str->_value.c_str());
		}
		else
		{
			m_userdefault.Parse("{}");
		}
	}
}

void saveUserDefaults()
{
	std::string path = HLResourceManager::getSingleton()->getWritablePath();
	path += "userdefaults.json";
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	m_userdefault.Accept(writer);
	std::string strJson(buffer.GetString(), buffer.GetSize());
	std::ofstream outfile(path.c_str());
	if (outfile.fail())
	{
		return;
	}
	outfile << strJson;
	outfile.close();
}

bool HLUserDefaults::getBoolForKey(const char* key, bool defaultvalue)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		return m_userdefault[key].GetBool();
	}
	return defaultvalue;
}

int HLUserDefaults::getIntForKey(const char* key, int defaultvalue)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		return m_userdefault[key].GetInt();
	}
	return defaultvalue;
}

float HLUserDefaults::getFloatForKey(const char* key, float defaultvalue)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		return (float)m_userdefault[key].GetDouble();
	}
	return defaultvalue;
}

std::string HLUserDefaults::getStringForKey(const char* key, std::string defaultvalue)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		return m_userdefault[key].GetString();
	}
	return defaultvalue;
}

void HLUserDefaults::setBoolForKey(const char* key, bool value)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		m_userdefault[key].SetBool(value);
	}
	else 
	{
		rapidjson::Value v;
		v.SetBool(value);
		m_userdefault.AddMember(rapidjson::Value(key, m_userdefault.GetAllocator()), 
			v, 
			m_userdefault.GetAllocator());
	}
	saveUserDefaults();
}

void HLUserDefaults::setIntForKey(const char* key, int value)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		m_userdefault[key].SetInt(value);
	}
	else
	{
		rapidjson::Value v;
		v.SetInt(value);
		m_userdefault.AddMember(rapidjson::Value(key, m_userdefault.GetAllocator()),
			v,
			m_userdefault.GetAllocator());
	}
	saveUserDefaults();
}

void HLUserDefaults::setFloatForKey(const char* key, float value)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		m_userdefault[key].SetDouble(value);
	}
	else
	{
		rapidjson::Value v;
		v.SetDouble(value);
		m_userdefault.AddMember(rapidjson::Value(key, m_userdefault.GetAllocator()),
			v,
			m_userdefault.GetAllocator());
	}
	saveUserDefaults();
}

void HLUserDefaults::setStringForKey(const char* key, std::string value)
{
	initUserDefaults();
	if (m_userdefault.HasMember(key))
	{
		m_userdefault[key].SetString(value.c_str(), m_userdefault.GetAllocator());
	}
	else
	{
		rapidjson::Value v;
		v.SetString(value.c_str(), m_userdefault.GetAllocator());
		m_userdefault.AddMember(rapidjson::Value(key, m_userdefault.GetAllocator()),
			v,
			m_userdefault.GetAllocator());
	}
	saveUserDefaults();
}

NS_HL_END