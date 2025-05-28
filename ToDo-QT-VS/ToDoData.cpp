#include "ToDoData.h"

#include "Snowflake.h"
#include "GlobalVariables.h"

#include <fstream>

//ToDoData::ToDoData(int64_t id) :
//	id(id)
//{ }
//
//ToDoData::ToDoData(int64_t id, int64_t create_date_time, int64_t deadline_date, int64_t deadline_time, std::string task) :
//	id(id), create_date_time(create_date_time),
//	deadline_date(deadline_date), deadline_time(deadline_time), task(task)
//{
//	//std::string _sql_from = "tasks";
//	//std::string _sql_what = "id,create_date_time,deadline_date,deadline_time,task";
//	//std::string _sql_value =
//	//	std::to_string(id) + "," +
//	//	std::to_string(create_date_time) + "," + 
//	//	std::to_string(deadline_date) + "," + 
//	//	std::to_string(deadline_time) + "," +
//	//	"'" + task + "'";
//	//GlobalVariables::instance()->data_database->DBInsert(_sql_from, _sql_what, _sql_value);
//}

void ToDoData::setTask(//std::string info_file_path, std::string text_file_path,
	std::string task)
{
	this->task = task;
	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("task", task, true);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoData::setIsFinished(//std::string info_file_path,
	bool flag)
{
	this->is_finished = flag;
	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("is_finished", std::to_string(is_finished), false);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoData::setFinishedDateTime(//std::string info_file_path,
	int64_t time)
{
	this->finished_date_time = time;
	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("finished_date_time", std::to_string(finished_date_time), false);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoData::deleteThis()//(std::string data_file_path)
{
	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "tasks";
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBDelete(_sql_from, _sql_where);
}

bool ToDoData::operator<(const ToDoData& other) const
{
	// 1. is_finished 为 false 的优先
	if (is_finished != other.is_finished)
	{
		return !is_finished; // is_finished 为 false 的排在前面
	}
	// 2. deadline_date 小的优先
	if (deadline_date != other.deadline_date)
	{
		if (!is_finished) { return deadline_date < other.deadline_date; }
		else { return deadline_date > other.deadline_date; }
	}
	//// 3. deadline_type 为 Time 的优先
	//if (deadline_type != other.deadline_type)
	//{
	//	return deadline_type == DeadlineType::Time; // Time 排在前面
	//}
	// 4. deadline_time 小的优先
	if (deadline_time != other.deadline_time)
	{
		if (!is_finished) { return deadline_time < other.deadline_time; }
		else { return deadline_time > other.deadline_time; }
	}

	return create_date_time < other.create_date_time;
}