#include "ToDoRepeatData.h"

#include "GlobalVariables.h"

#include <QDateTime>

std::string ToDoRepeatData::getRepeatTypeString(RepeatType type) const
{
	switch (type)
	{
	case ToDoRepeatData::RepeatType::EveryDay:
		return "每天";
		break;
	case ToDoRepeatData::RepeatType::EveryWeek:
		return "每周";
		break;
	case ToDoRepeatData::RepeatType::EveryMonth:
		return "每月";
		break;
	case ToDoRepeatData::RepeatType::EveryYear:
		return "每年";
		break;
	}
	return "error";
}

void ToDoRepeatData::setTask(std::string task)
{
	if (task == this->task) { return; }
	this->task = task;

	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "repeat_tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("task", task, true);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoRepeatData::setRepeatType(RepeatType type)
{
	this->repeat_type = type;

	int _repeat_type_int = static_cast<int>(this->repeat_type);
	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "repeat_tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("repeat_type", std::to_string(_repeat_type_int), false);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoRepeatData::setDetailRepeateDate(int32_t time)
{
	this->detail_repeat_date = time;

	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "repeat_tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("detail_repeat_date", std::to_string(detail_repeat_date), false);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoRepeatData::setLastAddDate(int64_t time)
{
	
	this->last_add_date = QDateTime(
		QDateTime::fromMSecsSinceEpoch(time).date(),
		QTime(0, 0, 0)
	).toMSecsSinceEpoch();//time;

	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "repeat_tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("last_add_date", std::to_string(last_add_date), false);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoRepeatData::setDuration(int64_t duration)
{
	this->duration = duration;

	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "repeat_tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("duration", std::to_string(duration), false);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoRepeatData::setDeadlineTime(int64_t time)
{
	this->deadline_time = time;

	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "repeat_tasks";
	DBUpdateList _update_list;
	_update_list.emplace_back("deadline_time", std::to_string(deadline_time), false);
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBUpdate(_sql_from, _update_list, _sql_where);
}

void ToDoRepeatData::deleteThis()
{
	DataBase* _db = GlobalVariables::instance()->data_database;
	std::string _sql_from = "repeat_tasks";
	std::string _sql_where = "id=" + std::to_string(id);
	_db->DBDelete(_sql_from, _sql_where);
}


static QDateTime getNextWeekday(const QDateTime& dt, int target_day_of_week)
{
	QDate date = dt.date();
	int current_day = date.dayOfWeek();  // 当前星期几（1=周一，7=周日）
	int days_to_add = (target_day_of_week - current_day + 7) % 7;
	days_to_add = (days_to_add == 0) ? 7 : days_to_add;  // 如果今天就是周二，跳到下周二
	return dt.addDays(days_to_add);
}
bool ToDoRepeatData::isRepeatDate(int64_t time) const
{
	QDateTime _time = QDateTime::fromMSecsSinceEpoch(time);
	switch (this->repeat_type)
	{
	case RepeatType::EveryDay:
		return true;
	case RepeatType::EveryWeek:
		if (_time.date().dayOfWeek() == this->detail_repeat_date)
		{
			return true;
		}
		break;
	case RepeatType::EveryMonth:
		if (_time.date().day() == 1)
		{
			return true;
		}
		break;
	case RepeatType::EveryYear:
		if (_time.date().day() == 1 && _time.date().month() == 1)
		{
			return true;
		}
		break;
	}

	return false;
}
bool ToDoRepeatData::isNeedRepeat(int64_t now_time) const
{
	QDateTime _temp_now_time = QDateTime::fromMSecsSinceEpoch(now_time);
	QDateTime _need_repeat_time = 
		QDateTime::fromMSecsSinceEpoch(this->getNeedRepeatDate());

	if (_temp_now_time >= _need_repeat_time)
	{
		return true;
	}
	return false;
}

int64_t ToDoRepeatData::getNeedRepeatDate() const
{
	QDateTime _need_repeat_date = QDateTime::fromMSecsSinceEpoch(this->last_add_date);
	if (this->last_add_date == 0)
	{
		//_need_repeat_date = QDateTime::fromMSecsSinceEpoch(this->);
		_need_repeat_date = QDateTime(
			QDateTime::fromMSecsSinceEpoch(this->create_date_time).date(),
			QTime(0, 0, 0)
		);
	}

	switch (this->repeat_type)
	{
	case RepeatType::EveryDay:
		_need_repeat_date = QDateTime(_need_repeat_date.addDays(1).date(), QTime(0, 0, 0));
		break;
	case RepeatType::EveryWeek:
		_need_repeat_date = getNextWeekday(_need_repeat_date, this->detail_repeat_date);
		break;
	case RepeatType::EveryMonth:
		_need_repeat_date = QDateTime(     // 上一次重复的下一个月的第一天
			QDate(_need_repeat_date.date().year(),
				_need_repeat_date.date().month() + 1,
				1),  // 下个月第1天
			QTime(0, 0, 0)
		);
		break;
	case RepeatType::EveryYear:
		_need_repeat_date = QDateTime(     // 上一次重复的下一年的第一天
			QDate(_need_repeat_date.date().year() + 1,
				1,
				1),
			QTime(0, 0, 0)
		);
		break;
	}

	return _need_repeat_date.toMSecsSinceEpoch();
}
