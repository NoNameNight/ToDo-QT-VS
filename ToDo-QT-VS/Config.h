#pragma once

#include <string>
#include <json/json.h>

#include <fstream>

class Config
{
public:
	static Config* instance()
	{
		static Config config;
		return &config;
	}

	std::string file_dir = "data";
	std::string database_file_name = "data.db";
	std::string window_state_file_name = "app_info.bin";
	std::string ui_color_file_name = "ui_color_argb.json";
	std::string getDataBaseFilePath() const
	{
		return file_dir + "/" + database_file_name;
	}
	std::string getWindowStateFilePath() const
	{
		return file_dir + "/" + window_state_file_name;
	}
	std::string getUiColorFilePath() const
	{
		return file_dir + "/" + ui_color_file_name;
	}

	void loadFromFile()
	{
		do
		{
			std::ifstream ifs;
			ifs.open(m_config_file_path);
			if (!ifs.is_open() || !ifs.good()) { break; }
			Json::Reader().parse(ifs, m_config_json);
			ifs.close();

			if (!m_config_json.isObject())
			{
				m_config_json = Json::Value();
				break;
			}
		} while (0);

		static std::vector<std::pair<std::string, std::string>> _check_list = {
			{ "file_dir", this->file_dir },
			{ "database_file_name", this->database_file_name },
			{ "window_state_file_name", this->window_state_file_name },
			{ "ui_color_file_name", this->ui_color_file_name }
		};
		if (!m_config_json.isMember("file_dir") ||
			!m_config_json["file_dir"].isString())
		{
			m_config_json["file_dir"] = this->file_dir;
		}

		for (const auto& _it : _check_list) 
		{
			if (!m_config_json.isMember(_it.first) ||
				!m_config_json[_it.first].isString())
			{
				m_config_json[_it.first] = _it.second;
			}
		}

		Json::Value::Members _keys = m_config_json.getMemberNames();
		for (const auto& _key : _keys)
		{
			if (_key == "file_dir")
			{
				this->file_dir = m_config_json[_key].asString();
			}
			else if (_key == "database_file_name")
			{
				this->database_file_name = m_config_json[_key].asString();
			}
			else if (_key == "window_state_file_name")
			{
				this->window_state_file_name = m_config_json[_key].asString();
			}
			else if (_key == "ui_color_file_name")
			{
				this->ui_color_file_name = m_config_json[_key].asString();
			}
		}
	}
	bool saveToFile()
	{
		std::ofstream ofs;
		ofs.open(m_config_file_path);
		if (!ofs.is_open() || !ofs.good()) { return false; }
		ofs << m_config_json.toStyledString();
		ofs.close();
		return true;
	}
private:
	Config() = default;
	Config(const Config& other) = delete;
	~Config() = default;
private:
	std::string m_config_file_path = "config.json";
	Json::Value m_config_json;
};