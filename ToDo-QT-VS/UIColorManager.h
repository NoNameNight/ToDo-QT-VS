#pragma once

#include "Util.h"

#include <unordered_map>
#include <string>
#include <QColor>

#include <json/json.h>

#include <vector>

class UIColorManager
{
public:
	static UIColorManager* instance()
	{
		static UIColorManager manager;
		return &manager;
	}

	QColor getColor(const std::string& name);
	bool setColor(const std::string& name, QColor color);
	bool addColor(const std::string& name, QColor color);

	//void addUIColorUpdateFunc(std::function<void()> func)
	//{
	//	m_ui_color_update_func_list.emplace_back(func);
	//}

	//bool loadFromFile(const std::string ui_color_file_path)
	bool loadFromFile();
	//bool saveToFile(const std::string ui_color_file_path)
	bool saveToFile();

	void setColorToDefault();
private:
	UIColorManager() = default;
	UIColorManager(const UIColorManager& other) = delete;
	~UIColorManager() = default;

	//void uiColorUpdate()
	//{
	//	for (auto _it = m_ui_color_update_func_list.begin();
	//		_it != m_ui_color_update_func_list.end(); ++_it)
	//	{
	//		(*_it)();
	//	}
	//}
private:
	std::unordered_map<std::string, QColor> m_default_ui_color_list =
	{
		{ "main_checkbox_bg", QColor(200, 200, 200, 50) },
		{ "main_checkbox_bg_ok",  QColor(100, 100, 100, 150)},
		{ "main_checkbox_check", QColor(200, 200, 200, 255) },
		{ "main_checkbox_border", QColor(100, 100, 100, 100) },
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

	//std::vector<std::function<void()>> m_ui_color_update_func_list;
};