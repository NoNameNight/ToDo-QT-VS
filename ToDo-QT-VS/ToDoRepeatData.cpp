#include "ToDoRepeatData.h"

#include <QDateTime>

#include <fstream>

const int ToDoRepeatData::text_ptr_offset = 0;
const int ToDoRepeatData::repeat_type_offset = 8;
const int ToDoRepeatData::last_add_date_time_offset = 9;
const int ToDoRepeatData::duration_offset = 17;
const int ToDoRepeatData::deadline_time_offset = 25;
const int ToDoRepeatData::detail_repeat_date_offset = 33;
//const int ToDoRepeatData::this_offset = 64;
const int ToDoRepeatData::this_size = 64;

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

void ToDoRepeatData::setThing(std::string info_file_path, std::string text_file_path, std::string thing)
{
	if (thing == this->thing) { return; }
	this->thing = thing;
	std::fstream _text_fs;
	std::ofstream _info_ofs;
	_text_fs.open(text_file_path, std::ios::binary | std::ios::in | std::ios::out);
	_info_ofs.open(info_file_path, std::ios::binary | std::ios::in | std::ios::out);

	char _size_buf[8] = {};
	int64_t _text_file_size = 0;
	int64_t _text_ptr = 0;
	_text_fs.seekg(0, std::ios::beg);
	_text_fs.read(_size_buf, 8);
	memcpy(&_text_file_size, _size_buf, 8);
	_text_ptr = _text_file_size;

	_text_fs.seekp(_text_ptr);
	memset(_size_buf, 0, 8);
	int64_t _thing_size = this->thing.size();
	memcpy(_size_buf, &_thing_size, 8);
	_text_fs.write(_size_buf, 8);
	_text_fs.write(this->thing.data(), this->thing.size());
	_text_file_size += 8 + this->thing.size();
	_text_fs.seekp(0, std::ios::beg);
	memset(_size_buf, 0, 8);
	memcpy(_size_buf, &_text_file_size, 8);
	_text_fs.write(_size_buf, 8);

	_info_ofs.seekp(this->info_ptr + this->text_ptr_offset);
	memset(_size_buf, 0, 8);
	memcpy(_size_buf, &_text_ptr, 8);
	_info_ofs.write(_size_buf, 8);

	_text_fs.close();
	_info_ofs.close();
}

void ToDoRepeatData::setRepeatType(std::string info_file_path, RepeatType type)
{
	this->repeat_type = type;
	std::ofstream _info_ofs;
	_info_ofs.open(info_file_path, std::ios::binary | std::ios::in | std::ios::out);

	_info_ofs.seekp(this->info_ptr + this->repeat_type_offset);
	char _repeat_type_char = static_cast<char>(this->repeat_type);
	_info_ofs.write(&_repeat_type_char, 1);

	_info_ofs.close();
}

void ToDoRepeatData::setDetailRepeateDate(std::string info_file_path, int32_t time)
{
	this->detail_repeat_date = time;
	std::ofstream _info_ofs;
	_info_ofs.open(
		info_file_path, std::ios::binary | std::ios::out | std::ios::in
	);

	_info_ofs.seekp(this->info_ptr + this->detail_repeat_date_offset);
	char size_buf[4] = {};
	memcpy(&size_buf, &this->detail_repeat_date, 4);
	_info_ofs.write(size_buf, 4);

	_info_ofs.close();
}

void ToDoRepeatData::setLastRepeatDate(std::string info_file_path, int64_t time)
{
	this->last_repeat_date = time;
	std::ofstream _info_ofs;
	_info_ofs.open(
		info_file_path, std::ios::binary | std::ios::out | std::ios::in
	);

	_info_ofs.seekp(this->info_ptr + this->last_add_date_time_offset);
	char size_buf[8] = {};
	memcpy(&size_buf, &this->last_repeat_date, 8);
	_info_ofs.write(size_buf, 8);

	_info_ofs.close();
}

void ToDoRepeatData::setDuration(std::string info_file_path, int64_t duration)
{
	this->duration = duration;
	std::ofstream _info_ofs;
	_info_ofs.open(
		info_file_path, std::ios::binary | std::ios::out | std::ios::in
	);

	_info_ofs.seekp(this->info_ptr + this->duration_offset);
	char size_buf[8] = {};
	memcpy(&size_buf, &this->duration, 8);
	_info_ofs.write(size_buf, 8);

	_info_ofs.close();
}

void ToDoRepeatData::setDeadlineTime(std::string info_file_path, int64_t time)
{
	this->deadline_time = time;
	std::ofstream _info_ofs;
	_info_ofs.open(
		info_file_path, std::ios::binary | std::ios::out | std::ios::in
	);

	_info_ofs.seekp(this->info_ptr + this->deadline_time_offset);
	char size_buf[8] = {};
	memcpy(&size_buf, &this->deadline_time, 8);
	_info_ofs.write(size_buf, 8);

	_info_ofs.close();
}

void ToDoRepeatData::deleteThis(std::string data_file_path)
{
	std::fstream _data_fs;
	_data_fs.open(
		data_file_path, std::ios::binary | std::ios::out | std::ios::in
	);

	_data_fs.seekp(0, std::ios::beg);

	char _size_buf[8] = {};
	int64_t _data_file_size = 0;
	_data_fs.read(_size_buf, 8);
	memcpy(&_data_file_size, _size_buf, 8);

	int64_t _now_data_ptr = this->data_ptr + 64;
	_data_fs.seekg(_now_data_ptr);
	while (_data_fs.tellg() < _data_file_size && _data_fs.peek() != EOF)
	{
		char _data_buf[64] = {};
		_data_fs.read(_data_buf, 64);
		_data_fs.seekp(_now_data_ptr - 64);
		_data_fs.write(_data_buf, 64);
		_now_data_ptr += 64;
		_data_fs.seekg(_now_data_ptr);
	}
	_data_file_size -= 64;
	memset(_size_buf, 0, 8);
	memcpy(_size_buf, &_data_file_size, 8);
	_data_fs.seekp(0, std::ios::beg);
	_data_fs.write(_size_buf, 8);

	_data_fs.close();
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
	QDateTime _need_repeat_date = QDateTime::fromMSecsSinceEpoch(this->last_repeat_date);

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
