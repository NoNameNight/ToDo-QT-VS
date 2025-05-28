#include "GlobalVariables.h"

#include "Util.h"

#include <fstream>

void GlobalVariables::databaseCreateTaskTable()
{
	if (!this->data_database)
	{
		fatalError("data_database is nullptr");
		return;
	}

	std::string _sql =
		"CREATE TABLE tasks("
		"	id BIGINT PRIMARY KEY NOT NULL,"
		"	is_finished BOOLEAN NOT NULL DEFAULT 0,"
		"	create_date_time BIGINT NOT NULL,"
		"	deadline_date BIGINT NOT NULL,"
		"	deadline_time BIGINT NOT NULL,"
		"	finished_date_time BIGINT NOT NULL DEFAULT 0,"
		"	task TEXT NOT NULL DEFAULT ''"
		");";

	char* _error = this->data_database->DBExecuteSQL(_sql);
	if (_error)
	{
		fatalError("data_database table tasks create error");
		return;
	}
}

void GlobalVariables::databaseCheckTaskTable()
{
	if (!this->data_database)
	{
		fatalError("data_database is nullptr");
		return;
	}
}

void GlobalVariables::databaseCreateRepeatTaskTable()
{
	if (!this->data_database)
	{
		fatalError("data_database is nullptr");
		return;
	}

	std::string _sql =
		"CREATE TABLE repeat_tasks("
		"	id BIGINT PRIMARY KEY  NOT NULL,"
		"	repeat_type TINYINT NOT NULL,"
		"	create_date_time BIGINT NOT NULL,"
		"	last_add_date BIGINT  NOT NULL DEFAULT 0,"
		"	duration BIGINT NOT NULL,"
		"	deadline_time BIGINT NOT NULL,"
		"	detail_repeat_date INT  NOT NULL,"
		"	task TEXT NOT NULL DEFAULT ''"
		");";
	char* _error = this->data_database->DBExecuteSQL(_sql);
	if (_error)
	{
		fatalError("data_database table tasks create error");
		return;
	}
}

void GlobalVariables::databaseCheckRepeatTaskTable()
{
	if (!this->data_database)
	{
		fatalError("data_database is nullptr");
		return;
	}
}

void GlobalVariables::loadRepeatDataFromFile()
{
	GlobalVariables* gv = GlobalVariables::instance();
	DataBase* _db = GlobalVariables::instance()->data_database;
	DBQueryRes _res = _db->DBQuery("repeat_tasks", "*");
	for (int _i = 0; _i < _res.rows; ++_i)
	{
		ToDoRepeatData* _data = new ToDoRepeatData();
		_data->id = str2int64_t(_res["id"][_i]);
		int _repeat_type_int = str2int32_t(_res["repeat_type"][_i]);
		_data->repeat_type = static_cast<ToDoRepeatData::RepeatType>(_repeat_type_int);
		_data->detail_repeat_date = str2int32_t(_res["detail_repeat_date"][_i]);
		_data->task = _res["task"][_i];
		_data->create_date_time = str2int64_t(_res["create_date_time"][_i]);
		_data->last_add_date = str2int64_t(_res["last_add_date"][_i]);
		_data->duration = str2int64_t(_res["duration"][_i]);
		_data->deadline_time = str2int64_t(_res["deadline_time"][_i]);
		repeat_data_list.emplace_back(_data);
	}
}

ToDoRepeatData* GlobalVariables::addRepeatData(int64_t id, std::string task, ToDoRepeatData::RepeatType repeat_type, int32_t detail_repeat_date, int64_t duration, int64_t deadline_time)
{
	DataBase* _db = GlobalVariables::instance()->data_database;

	int64_t _create_date_time = QDateTime::currentMSecsSinceEpoch();

	std::string _sql_from = "repeat_tasks";
	std::string _sql_what =
		"id,repeat_type,detail_repeat_date,task,"
		"create_date_time,last_add_date,duration,deadline_time";
	int _repeat_type_int = static_cast<int>(repeat_type);
	std::string _sql_value = std::to_string(id) + "," +
		std::to_string(_repeat_type_int) + "," +
		std::to_string(detail_repeat_date) + "," +
		"'" + task + "'" + "," +
		std::to_string(_create_date_time) + "," +
		std::to_string(0) + "," +
		std::to_string(duration) + "," +
		std::to_string(deadline_time);
	char* _error =_db->DBInsert(_sql_from, _sql_what, _sql_value);
	if (_error)
	{
		qDebug() << _error;
	}
	ToDoRepeatData* _data = new ToDoRepeatData();
	_data->id = id;
	_data->task = task;
	_data->repeat_type = repeat_type;
	_data->create_date_time = _create_date_time;
	_data->detail_repeat_date = detail_repeat_date;
	_data->duration = duration;
	_data->deadline_time = deadline_time;
	repeat_data_list.emplace_back(_data); 
	return _data;
}

void GlobalVariables::deleteRepeatData(int index)
{
	if (repeat_data_list.size() <= index) { return; }
	repeat_data_list[index]->deleteThis();
	delete repeat_data_list[index];
	repeat_data_list.erase(repeat_data_list.begin() + index);
}

//void GlobalVariables::setRepeatDataThing(int index, std::string thing)
//{
//	if (repeat_data_list.size() <= index) { return; }
//	repeat_data_list[index]->setThing(
//		this->getRepeatInfoFilePath(), this->getRepeatTextFilePath(), thing
//	);
//}
//
//void GlobalVariables::setRepeatDataLastAddDateTime(int index, int64_t time)
//{
//	if (repeat_data_list.size() <= index) { return; }
//	repeat_data_list[index]->setLastAddDateTime(this->getRepeatInfoFilePath(), time);
//}

