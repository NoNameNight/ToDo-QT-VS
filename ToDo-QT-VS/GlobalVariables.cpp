#include "GlobalVariables.h"

#include <fstream>

void GlobalVariables::loadRepeatDataFromFile()
{
	std::ifstream data_ifs;
	std::ifstream info_ifs;
	std::ifstream text_ifs;
	data_ifs.open(this->getRepeatDataFilePath(), std::ios::binary | std::ios::in);
	info_ifs.open(this->getRepeatInfoFilePath(), std::ios::binary | std::ios::in);
	text_ifs.open(this->getRepeatTextFilePath(), std::ios::binary | std::ios::in);
	if (!data_ifs.is_open() || !info_ifs.is_open() || !text_ifs.is_open())
	{
		return;
	}
	char size_buf[8] = {};
	int64_t data_file_size = 0;
	data_ifs.read(size_buf, 8);
	memcpy(&data_file_size, size_buf, 8);

	while (data_ifs.tellg() < data_file_size && data_ifs.peek() != EOF)
	{
		ToDoRepeatData* _data = new ToDoRepeatData();
		_data->data_ptr = data_ifs.tellg();
		char data_buf[64] = {};
		data_ifs.read(data_buf, 64);
		memcpy(&_data->info_ptr, data_buf, 8);

		{
			char info_buf[64] = {};
			info_ifs.seekg(_data->info_ptr);
			info_ifs.read(info_buf, 64);
			char type_char = 0;
			memcpy(&_data->text_ptr, info_buf + ToDoRepeatData::text_ptr_offset, 8);
			memcpy(&type_char, info_buf + ToDoRepeatData::repeat_type_offset, 1);
			_data->repeat_type = static_cast<ToDoRepeatData::RepeatType>(type_char);
			memcpy(&_data->detail_repeat_date, info_buf + ToDoRepeatData::detail_repeat_date_offset, 4);
			memcpy(&_data->last_repeat_date, info_buf + ToDoRepeatData::last_add_date_time_offset, 8);
			memcpy(&_data->duration, info_buf + ToDoRepeatData::duration_offset, 8);
			memcpy(&_data->deadline_time, info_buf + ToDoRepeatData::deadline_time_offset, 8);
		}

		{
			int64_t text_size = 0;
			memset(size_buf, 0, 8);
			text_ifs.seekg(_data->text_ptr);
			text_ifs.read(size_buf, 8);
			memcpy(&text_size, size_buf, 8);
			char _text_buf[1024] = { };
			text_ifs.read(_text_buf, text_size);
			_data->thing = std::string(_text_buf, text_size);
		}

		repeat_data_list.emplace_back(_data);
	}

	data_ifs.close();
	info_ifs.close();
	text_ifs.close();
}

void GlobalVariables::addRepeatData(std::string thing, ToDoRepeatData::RepeatType type, int32_t detail_repeat_date, int64_t duration, int64_t deadline_time)
{
	ToDoRepeatData* _data = new ToDoRepeatData();
	_data->thing = thing;
	_data->repeat_type = type;
	_data->detail_repeat_date = detail_repeat_date;
	_data->duration = duration;
	_data->deadline_time = deadline_time;

	std::fstream _data_fs;
	std::ofstream _info_ofs;
	std::ofstream _text_ofs;
	_data_fs.open(this->getRepeatDataFilePath(), std::ios::binary | std::ios::app);
	_data_fs.close();
	_data_fs.open(
		this->getRepeatDataFilePath(),
		std::ios::binary | std::ios::out | std::ios::in
	);
	_info_ofs.open(
		this->getRepeatInfoFilePath(),
		std::ios::binary | std::ios::app
	);
	_text_ofs.open(
		this->getRepeatTextFilePath(),
		std::ios::binary | std::ios::app
	);
	_data_fs.seekp(0, std::ios::end);
	if (_data_fs.tellp() == 0)
	{
		int64_t _size = 8;
		char _size_str[8] = {};
		memcpy(_size_str, &_size, 8);
		_data_fs.write(_size_str, 8);
	}

	_data_fs.seekg(0, std::ios::beg);
	_info_ofs.seekp(0, std::ios::end);
	_text_ofs.seekp(0, std::ios::end);
	int64_t _data_file_size = 0;
	int64_t _info_ptr = 0;
	int64_t _text_ptr = 0;
	char _size_buf[8] = {};

	_data_fs.read(_size_buf, 8);
	memcpy(&_data_file_size, _size_buf, 8);
	_data->data_ptr = _data_file_size;
	_info_ptr = _info_ofs.tellp();
	memset(_size_buf, 0, 8);
	_text_ptr = _text_ofs.tellp();

	_data_fs.seekp(_data_file_size);
	{
		char _data_buf[64] = {};
		memcpy(_data_buf, &_info_ptr, 8);
		_data_fs.write(_data_buf, 64);

		_data_file_size += 64;
		memset(_size_buf, 0, 8);
		memcpy(_size_buf, &_data_file_size, 8);
		_data_fs.seekp(0, std::ios::beg);
		_data_fs.write(_size_buf, 8);
	}

	{
		memset(_size_buf, 0, 8);
		int64_t thing_size = thing.size();
		memcpy(_size_buf, &thing_size, 8);
		_text_ofs.write(_size_buf, 8);
		_text_ofs.write(thing.data(), thing.size());
	}
	_data->text_ptr = _text_ptr;

	{
		char _info_buf[64] = {};
		char _type_char = static_cast<char>(type);
		memcpy(_info_buf + ToDoRepeatData::text_ptr_offset, &_text_ptr, 8);
		memcpy(_info_buf + ToDoRepeatData::repeat_type_offset, &_type_char, 1);
		memcpy(_info_buf + ToDoRepeatData::detail_repeat_date_offset, &_data->detail_repeat_date, 4);
		//memcpy(info_buf + ToDoRepeatData::last_add_time_offset, 0, 8);
		memcpy(_info_buf + ToDoRepeatData::duration_offset, &_data->duration, 8);
		memcpy(_info_buf + ToDoRepeatData::deadline_time_offset, &_data->deadline_time, 8);
		_info_ofs.write(_info_buf, 64);
	}
	_data->info_ptr = _info_ptr;

	_data_fs.close();
	_info_ofs.close();
	_text_ofs.close();
	repeat_data_list.emplace_back(_data);
}

void GlobalVariables::deleteRepeatData(int index)
{
	if (repeat_data_list.size() <= index) { return; }
	repeat_data_list[index]->deleteThis(this->getRepeatDataFilePath());
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

