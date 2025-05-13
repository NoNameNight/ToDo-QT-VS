#include "ToDoData.h"

#include <fstream>

const int ToDoData::text_ptr_offset = 0;
const int ToDoData::is_finished_offset = 8;
const int ToDoData::create_date_time_offset = 9;
//const int ToDoData::deadline_type_offset = 17;
const int ToDoData::deadline_date_offset = 18;
const int ToDoData::deadline_time_offset = 26;
const int ToDoData::finished_date_time_offset = 34;
const int ToDoData::this_size = 128;

void ToDoData::setThing(std::string info_file_path, std::string text_file_path, std::string thing)
{
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

void ToDoData::setIsFinished(std::string info_file_path, bool flag)
{
	this->is_finished = flag;
	std::ofstream _info_ofs;
	_info_ofs.open(info_file_path, std::ios::binary | std::ios::in | std::ios::out);

	_info_ofs.seekp(this->info_ptr + this->is_finished_offset);
	char _repeat_type_char = static_cast<char>(this->is_finished);
	_info_ofs.write(&_repeat_type_char, 1);

	_info_ofs.close();
}

void ToDoData::setFinishedTime(std::string info_file_path, int64_t time)
{
	this->finished_date_time = time;
	std::ofstream _info_ofs;
	_info_ofs.open(info_file_path, std::ios::binary | std::ios::in | std::ios::out);

	_info_ofs.seekp(this->info_ptr + this->finished_date_time_offset);
	char size_buf[8] = {};
	memcpy(&size_buf, &this->finished_date_time, 8);
	_info_ofs.write(size_buf, 8);

	_info_ofs.close();
}

#include <QDebug>
void ToDoData::deleteThis(std::string data_file_path)
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

	//_data_fs.seekp(this->data_ptr);
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