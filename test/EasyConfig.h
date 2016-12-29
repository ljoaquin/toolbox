#ifndef __EASY_CONFIG_H__
#define __EASY_CONFIG_H__

#include <string>
#include "rapidjson/document.h"

/*================================

@description:
	a easy-to-use tool to read json config file with only 1 level

@usage:
	EasyConfig config(filename);

	int n = config.getInt("kInt");
	float f = config.getFloat("kFloat");
	bool b = config.getBool("kBool");
	std::string str = config.getString("kString");

[file content(json)]:
{
	"kNull":null,
	"kInt":17,
	"kFloat":3.1415926,
	"kBool":false,
	"kString":"easy config is ok"
}

================================*/

namespace toolbox
{

	class EasyConfig
	{
	public:
		virtual ~EasyConfig();
		EasyConfig(const std::string& sFileName);
		bool isLoaded(){ return m_bIsLoaded; };

		bool	hasMember	(const std::string& key);
		int		getInt		(const std::string& key, int default_value = 0);
		float	getFloat	(const std::string& key, float default_value = 0);
		double	getDouble	(const std::string& key, double default_value = 0);
		bool	getBool		(const std::string& key, bool default_value = true);
		std::string getString(const std::string& key, const std::string& default_value = "");

	private:
		EasyConfig();

	private:
		rapidjson::Document m_cDoc;
		bool m_bIsLoaded;
	};

}

#endif // __EASY_CONFIG_H__