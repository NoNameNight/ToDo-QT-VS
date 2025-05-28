#pragma once

#include <string>
#include <stdint.h>
#include <QPixmap>
#include <QHotkey>
#include <unordered_map>

#include <json/json.h>

#include "ToDoRepeatData.h"
#include "DataBase.h"

class GlobalVariables
{
public:
	static GlobalVariables* instance()
	{
		static GlobalVariables gv;
		return &gv;
	}

	QPixmap* delete_png = nullptr;
	QPixmap* setting_png = nullptr;

	std::function<void()> update_main_window_ui = nullptr;

	DataBase* data_database = nullptr;
	void databaseCreateTaskTable();
	void databaseCheckTaskTable();
	void databaseCreateRepeatTaskTable();
	void databaseCheckRepeatTaskTable();

	void loadRepeatDataFromFile();
	ToDoRepeatData* addRepeatData(
		int64_t id,
		std::string task, ToDoRepeatData::RepeatType repeat_type,
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