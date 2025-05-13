#pragma once

#include "Util.h"

#include <unordered_map>
#include <string>
#include <QColor>

#include <json/json.h>
#include <fstream>

#include <vector>

class UIColorManager
{
public:
	static UIColorManager* instance()
	{
		static UIColorManager manager;
		return &manager;
	}

	QColor getColor(const std::string& name)
	{
		auto _it = m_ui_color_list.find(name);
		if (_it == m_ui_color_list.end())
		{
			fatalError("need " + name);
			return QColor();
		}
		return _it->second;
	}
	bool setColor(const std::string& name, QColor color)
	{
		if (m_ui_color_list.find(name) == m_ui_color_list.end())
		{
			return false;
		}
		m_ui_color_list[name] = color;
		m_ui_color_json[name] = color.name(QColor::HexArgb).toStdString();

		uiColorUpdate();
		return true;
	}
	bool addColor(const std::string& name, QColor color)
	{
		if (m_ui_color_list.find(name) != m_ui_color_list.end())
		{
			return false;
		}
		m_ui_color_list[name] = color;
		m_ui_color_json[name] = color.name(QColor::HexArgb).toStdString();
		return true;
	}

	void addUIColorUpdateFunc(std::function<void()> func)
	{
		m_ui_color_update_func_list.emplace_back(func);
	}

	//bool loadFromFile(const std::string ui_color_file_path)
	bool loadFromFile()
	{
		do
		{
			std::ifstream ifs;
			//ifs.open(ui_color_file_path);
			ifs.open(m_ui_color_file_path);
			if (!ifs.is_open() || !ifs.good()) { break; }
			Json::Reader().parse(ifs, m_ui_color_json);
			ifs.close();

			if (!m_ui_color_json.isObject())
			{
				m_ui_color_json = Json::Value();
				break;
			}
			Json::Value::Members keys = m_ui_color_json.getMemberNames();
			for (const auto& key : keys)
			{
				m_ui_color_list[key] =
					QColor::fromString(m_ui_color_json[key].asString());
			}
		} while (0);

		for (auto _it = m_default_ui_color_list.begin();
			_it != m_default_ui_color_list.end(); ++_it)
		{
			if (m_ui_color_list.find(_it->first) != m_ui_color_list.end())
			{
				continue;
			}
			this->addColor(_it->first, _it->second);
		}
		return true;
	}
	//bool saveToFile(const std::string ui_color_file_path)
	bool saveToFile()
	{
		std::ofstream ofs;
		//ofs.open(ui_color_file_path);
		ofs.open(m_ui_color_file_path);
		if (!ofs.is_open() || !ofs.good()) { return false; }
		ofs << m_ui_color_json.toStyledString();
		ofs.close();
		return true;
	}

	void setColorToDefault()
	{
		for (auto _it = m_default_ui_color_list.begin();
			_it != m_default_ui_color_list.end(); ++_it)
		{
			m_ui_color_list[_it->first] = _it->second;
			m_ui_color_json[_it->first] = 
				_it->second.name(QColor::HexArgb).toStdString();
		}
		this->saveToFile();
		uiColorUpdate();
	}
private:
	UIColorManager() = default;
	UIColorManager(const UIColorManager& other) = delete;
	~UIColorManager() = default;

	void uiColorUpdate()
	{
		for (auto _it = m_ui_color_update_func_list.begin();
			_it != m_ui_color_update_func_list.end(); ++_it)
		{
			(*_it)();
		}
	}
private:
	std::unordered_map<std::string, QColor> m_default_ui_color_list =
	{
		{ "ckeckbox_bg", QColor(200, 200, 200, 50) },
		{ "checkbox_bg_ok",  QColor(100, 100, 100, 150)},
		{ "checkbox_check", QColor(200, 200, 200, 255) },
		{ "checkbox_border", QColor(100, 100, 100, 100) },
		{ "main_bg", QColor(100, 100, 100, 100) },
		{ "main_time", QColor(255, 255, 255, 170) },
		{ "main_year", QColor(255, 255, 255, 150) },
		{ "main_date", QColor(255, 255, 255, 170) },
		{ "main_task_border", QColor(100, 100, 100, 80) },
		{ "main_task_border_out", QColor(255, 0, 0, 80) },
		{ "main_task_bg", QColor(0, 0, 0, 0) },
		{ "main_task_bg_out", QColor(100, 0, 0, 100) },
		{ "main_task_bg_hover", QColor(100, 100, 100, 100) },
		{ "main_task_bg_out_hover", QColor(200, 0, 0, 100) },
		{ "main_task_task", QColor(255, 255, 255, 200) },
		{ "main_task_date", QColor(255, 255, 255, 200) }
	};
	std::unordered_map<std::string, QColor> m_ui_color_list;
	Json::Value m_ui_color_json;

	std::string m_ui_color_file_path = "ui_color_argb.json";

	std::vector<std::function<void()>> m_ui_color_update_func_list;
};