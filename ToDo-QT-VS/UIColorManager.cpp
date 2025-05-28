#include "UIColorManager.h"

#include <fstream>
#include "Config.h"

QColor UIColorManager::getColor(const std::string& name)
{
	auto _it = m_ui_color_list.find(name);
	if (_it == m_ui_color_list.end())
	{
		fatalError("need " + name);
		return QColor();
	}
	return _it->second;
}

bool UIColorManager::setColor(const std::string& name, QColor color)
{
	if (m_ui_color_list.find(name) == m_ui_color_list.end())
	{
		return false;
	}
	m_ui_color_list[name] = color;
	m_ui_color_json[name] = color.name(QColor::HexArgb).toStdString();

	//uiColorUpdate();
	return true;
}

bool UIColorManager::addColor(const std::string& name, QColor color)
{
	if (m_ui_color_list.find(name) != m_ui_color_list.end())
	{
		return false;
	}
	m_ui_color_list[name] = color;
	m_ui_color_json[name] = color.name(QColor::HexArgb).toStdString();
	return true;
}

bool UIColorManager::loadFromFile()
{
	do
	{
		std::ifstream ifs;
		//ifs.open(ui_color_file_path);
		ifs.open(Config::instance()->getUiColorFilePath());
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

bool UIColorManager::saveToFile()
{
	std::ofstream ofs;
	//ofs.open(ui_color_file_path);
	ofs.open(Config::instance()->getUiColorFilePath());
	if (!ofs.is_open() || !ofs.good()) { return false; }
	ofs << m_ui_color_json.toStyledString();
	ofs.close();
	return true;
}

void UIColorManager::setColorToDefault()
{
	for (auto _it = m_default_ui_color_list.begin();
		_it != m_default_ui_color_list.end(); ++_it)
	{
		m_ui_color_list[_it->first] = _it->second;
		m_ui_color_json[_it->first] =
			_it->second.name(QColor::HexArgb).toStdString();
	}
	this->saveToFile();
	//uiColorUpdate();
}