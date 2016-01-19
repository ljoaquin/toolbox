#include "EasyConfig.h"

namespace toolbox
{

	// @lj: user should be responsible for the size and security of buf
	static bool getDataFromFile(const std::string& sFileName, char* buf, unsigned int bufSize)
	{
		bool bRet = false;

		if (buf)
		{
			FILE* f1 = fopen(sFileName.c_str(), "rb");
			if (f1)
			{
				fseek(f1, 0, SEEK_END);
				int nContentLen = ftell(f1);
				fseek(f1, 0, SEEK_SET);
				if (bufSize > nContentLen)
				{
					fread(buf, 1, nContentLen, f1);
					bRet = true;
				}
				fclose(f1);
			}
		}
		return bRet;
	}

	EasyConfig::EasyConfig()
	{

	}

	EasyConfig::~EasyConfig()
	{

	}

	EasyConfig::EasyConfig(const std::string& sFileName)
	{
		m_bIsLoaded = false;

		const int bufSize = 1024;
		char buf[bufSize];
		memset(buf, 0, bufSize);
		bool flag = getDataFromFile(sFileName, buf, bufSize);

		if (flag)
		{
			m_cDoc.Parse(buf);
			if (m_cDoc.GetParseError() == rapidjson::kParseErrorNone)
			{
				m_bIsLoaded = true;
			}
		}
	}

	bool EasyConfig::hasMember(const std::string& key)
	{
		return m_cDoc.HasMember(key.c_str());
	}

	int EasyConfig::getInt(const std::string& key, int default_value)
	{
		if (m_bIsLoaded)
		{
			if (hasMember(key.c_str()) && m_cDoc[key.c_str()].IsInt())
			{
				return m_cDoc[key.c_str()].GetInt();
			}
		}
		return default_value;
	}

	float EasyConfig::getFloat(const std::string& key, float default_value)
	{
		return (float)getDouble(key, default_value);
	}

	double EasyConfig::getDouble(const std::string& key, double default_value)
	{
		if (m_bIsLoaded)
		{
			if (hasMember(key.c_str()) && m_cDoc[key.c_str()].IsNumber())
			{
				return m_cDoc[key.c_str()].GetDouble();
			}
		}
		return default_value;
	}

	bool EasyConfig::getBool(const std::string& key, bool default_value)
	{
		if (m_bIsLoaded)
		{
			if (hasMember(key.c_str()) && m_cDoc[key.c_str()].IsBool())
			{
				return m_cDoc[key.c_str()].GetBool();
			}
		}
		return default_value;
	}

	std::string EasyConfig::getString(const std::string& key, const std::string& default_value)
	{
		if (m_bIsLoaded)
		{
			if (hasMember(key.c_str()) && m_cDoc[key.c_str()].IsString())
			{
				return m_cDoc[key.c_str()].GetString();
			}
		}
		return default_value;
	}

}
