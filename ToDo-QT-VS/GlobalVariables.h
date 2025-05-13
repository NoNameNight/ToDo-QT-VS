#pragma once

#include <string>
#include <stdint.h>
#include <QPixmap>
#include <QHotkey>
#include <unordered_map>

#include <json/json.h>

#include "ToDoRepeatData.h"

class GlobalVariables
{
public:
	static GlobalVariables* instance()
	{
		static GlobalVariables gv;
		return &gv;
	}

	std::string resource_dir = "resource";
	QPixmap* delete_png = nullptr;
	QPixmap* setting_png = nullptr;

	QHotkey* mouse_transparent_hotkey = nullptr;
	QHotkey* top_most_hotkey = nullptr;

	std::function<void()> update_main_window_ui = nullptr;

	std::string file_dir = "data";
	std::string temp_file_dir = "data/temp";
	std::string text_file_name = "text.data";
	std::string data_file_name = "data.data";
	std::string info_file_name = "info.data";
	std::string getTextFilePath() const
	{
		return file_dir + "/" + text_file_name;
	}
	std::string getDataFilePath() const
	{
		return file_dir + "/" + data_file_name;
	}
	std::string getInfoFilePath() const
	{
		return file_dir + "/" + info_file_name;
	}
	std::string getTempTextFilePath() const
	{
		return temp_file_dir + "/" + text_file_name;
	}
	std::string getTempDataFilePath() const
	{
		return temp_file_dir + "/" + data_file_name;
	}
	std::string getTempInfoFilePath() const
	{
		return temp_file_dir + "/" + info_file_name;
	}

	std::string repeat_data_file_name = "repeat_data.data";
	std::string repeat_info_file_name = "repeat_info.data";
	std::string repeat_text_file_name = "repeat_text.data";
	std::string getRepeatDataFilePath() const
	{
		return file_dir + "/" + repeat_data_file_name;
	}
	std::string getRepeatInfoFilePath() const
	{
		return file_dir + "/" + repeat_info_file_name;
	}
	std::string getRepeatTextFilePath() const
	{
		return file_dir + "/" + repeat_text_file_name;
	}
	std::string getTempRepeatDataFilePath() const
	{
		return temp_file_dir + "/" + repeat_data_file_name;
	}
	std::string getTempRepeatInfoFilePath() const
	{
		return temp_file_dir + "/" + repeat_info_file_name;
	}
	std::string getTempRepeatTextFilePath() const
	{
		return temp_file_dir + "/" + repeat_text_file_name;
	}

	void loadRepeatDataFromFile();
	void addRepeatData(
		std::string thing, ToDoRepeatData::RepeatType type,
		int32_t detail_repeat_date,
		int64_t duration, int64_t deadline_time
	);
	void deleteRepeatData(int index);
	//void setRepeatDataThing(int index, std::string thing);
	//void setRepeatDataLastAddDateTime(int index, int64_t time);
	std::vector<ToDoRepeatData*> repeat_data_list;

private:
	GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& other) = delete;
	~GlobalVariables() = default;
private:
};