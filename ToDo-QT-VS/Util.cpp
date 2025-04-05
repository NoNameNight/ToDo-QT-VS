#include "Util.h"

#include <QDateTime>

std::string getUUID(GenderatorUUIDType type)
{
	static boost::uuids::random_generator random;
	static boost::uuids::random_generator_mt19937 random_mt19937;
	static boost::uuids::random_generator_pure random_pure;
	static boost::uuids::time_generator_v1 time_v1;
	static boost::uuids::time_generator_v6 time_v6;
	static boost::uuids::time_generator_v7 time_v7;

	// 生成UUID
	boost::uuids::uuid uuid;

	switch (type)
	{
	case GenderatorUUIDType::random:
		uuid = random();
		break;
	case GenderatorUUIDType::randomMt19937:
		uuid = random_mt19937();
		break;
	case GenderatorUUIDType::randomPure:
		uuid = random_pure();
		break;
	case GenderatorUUIDType::timeV1:
		uuid = time_v1();
		break;
	case GenderatorUUIDType::timeV6:
		uuid = time_v6();
		break;
	case GenderatorUUIDType::timeV7:
		uuid = time_v7();
		break;
	}

	std::string uuid_str = boost::uuids::to_string(uuid);
	uuid_str.erase(
		std::remove(uuid_str.begin(), uuid_str.end(), '-'),
		uuid_str.end()
	);
	// 将UUID转换为字符串并输出
	return uuid_str;
}

int ToDoData::text_ptr_offset = 0;
int ToDoData::is_finished_offset = 8;
int ToDoData::create_time_offset = 9;
int ToDoData::deadline_type_offset = 17;
int ToDoData::deadline_date_offset = 18;
int ToDoData::deadline_time_offset = 26;
int ToDoData::finished_time_offset = 34;

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
	// 3. deadline_type 为 Time 的优先
	if (deadline_type != other.deadline_type)
	{
		return deadline_type == DeadlineType::Time; // Time 排在前面
	}
	// 4. deadline_time 小的优先
	if (deadline_time != other.deadline_time)
	{
		if (!is_finished) { return deadline_time < other.deadline_time; }
		else { return deadline_time > other.deadline_time; }
	}
	// 如果所有条件都相同，则默认返回 false
	return create_time < other.create_time;
}

const int ToDoRepeatData::text_ptr_offset = 0;
const int ToDoRepeatData::repeat_type_offset = 8;
const int ToDoRepeatData::last_add_time_offset = 9;
const int ToDoRepeatData::duration_offset = 17;
//const int ToDoRepeatData::this_offset = 64;

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

void ToDoRepeatData::setLastAddTime(std::string info_file_path, int64_t time)
{
	this->last_add_time = time;
	std::ofstream _info_ofs;
	_info_ofs.open(
		info_file_path, std::ios::binary | std::ios::out | std::ios::in
	);

	_info_ofs.seekp(this->info_ptr + this->last_add_time_offset);
	char size_buf[8] = {};
	memcpy(&size_buf, &this->last_add_time, 8);
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

	_data_fs.seekp(this->data_ptr);
	_data_fs.seekg(this->data_ptr + 64);
	while (_data_fs.tellg() < _data_file_size && _data_fs.peek() != EOF)
	{
		char _data_buf[64] = {};
		_data_fs.read(_data_buf, 64);
		_data_fs.write(_data_buf, 64);
	}
	_data_file_size -= 64;
	memset(_size_buf, 0, 8);
	memcpy(_size_buf, &_data_file_size, 8);
	_data_fs.seekp(0, std::ios::beg);
	_data_fs.write(_size_buf, 8);

	_data_fs.close();
}

bool ToDoRepeatData::isNeedRepeat(int64_t now_time) const
{
	QDateTime _temp_now_time = QDateTime::fromMSecsSinceEpoch(now_time);
	QDateTime _temp_last_time = QDateTime::fromMSecsSinceEpoch(this->last_add_time);

	switch (this->repeat_type)
	{
	case RepeatType::EveryDay:
		return _temp_now_time.date() != _temp_last_time.date();
	case RepeatType::EveryWeek:
		return !(_temp_now_time.date().year() == _temp_last_time.date().year() &&
			_temp_now_time.date().weekNumber() == _temp_last_time.date().weekNumber());
	case RepeatType::EveryMonth:
		
		return !(_temp_now_time.date().year() == _temp_last_time.date().year() &&
			_temp_now_time.date().month() == _temp_last_time.date().month());
	case RepeatType::EveryYear:
		return _temp_now_time.date().year() != _temp_last_time.date().year();
	}
}

void Config::HotkeyData::setIsCtrl(bool flag)
{
	int _is_ctrl_ptr = this->m_config_ptr;
	this->m_is_ctrl = flag;
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_ctrl, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		Config::instance()->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void Config::HotkeyData::setIsAlt(bool flag)
{
	int _is_ctrl_ptr = this->m_config_ptr + 1;
	this->m_is_alt = flag;
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_alt, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		Config::instance()->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void Config::HotkeyData::setIsShift(bool flag)
{
	int _is_ctrl_ptr = this->m_config_ptr + 2;
	this->m_is_shift = flag;
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_shift, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		Config::instance()->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void Config::HotkeyData::setHotkeyChar(char c)
{
	int _is_ctrl_ptr = this->m_config_ptr + 7;
	this->m_hotkey_char = c;
	std::ofstream _config_ofs;
	_config_ofs.open(
		Config::instance()->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(&this->m_hotkey_char, 1);
	_config_ofs.close();
}

std::string Config::HotkeyData::getHotkeyFuncKeyString() const
{
	std::string _res = "";
	if (m_is_ctrl)
	{
		_res += "Ctrl";
	}
	if (m_is_alt)
	{
		if (_res.size()) { _res += "+"; }
		_res += "Alt";
	}
	if (m_is_shift)
	{
		if (_res.size()) { _res += "+"; }
		_res += "Shift";
	}
	return _res;
}


void Config::loadConfig()
{
	const int64_t _config_data_begin = 0;
	const int64_t _config_data_buf_size = 64;
	const int64_t _config_shortcut_key_begin = _config_data_buf_size;
	const int64_t _config_shortcut_key_buf_size = 128;
	std::fstream _config_fs;
	_config_fs.open(this->m_config_file_path, std::ios::binary | std::ios::out | std::ios::in);

	if (!_config_fs.is_open() || !_config_fs.good())
	{
		_config_fs.open(this->m_config_file_path, std::ios::binary | std::ios::app);
		_config_fs.close();
		_config_fs.open(
			this->m_config_file_path,
			std::ios::binary | std::ios::out | std::ios::in
		);
	}

	_config_fs.seekg(_config_data_begin);
	_config_fs.seekp(_config_data_begin);
	if (_config_fs.peek() == EOF)
	{
		_config_fs.clear();
		_config_fs.seekp(_config_data_begin);
		char _data_buf[_config_data_buf_size] = {};
		int _data_offset = 0;
		memcpy(_data_buf + _data_offset, &this->m_window_x, 4);
		_data_offset += 4;
		memcpy(_data_buf + _data_offset, &this->m_window_y, 4);
		_data_offset += 4;
		memcpy(_data_buf + _data_offset, &this->m_window_width, 4);
		_data_offset += 4;
		memcpy(_data_buf + _data_offset, &this->m_window_height, 4);
		_data_offset += 4;
		memcpy(_data_buf + _data_offset, &this->m_is_mouse_transparent, 1);
		_data_offset += 1;
		memcpy(_data_buf + _data_offset, &this->m_is_top_most, 1);
		_data_offset += 1;

		_config_fs.write(_data_buf, _config_data_buf_size);
	}

	char _data_buf[_config_data_buf_size] = {};

	_config_fs.seekg(_config_data_begin, std::ios::beg);
	_config_fs.read(_data_buf, _config_data_buf_size);

	int _data_offset = 0;
	memcpy(&this->m_window_x, _data_buf + _data_offset, 4);
	_data_offset += 4;
	memcpy(&this->m_window_y, _data_buf + _data_offset, 4);
	_data_offset += 4;
	memcpy(&this->m_window_width, _data_buf + _data_offset, 4);
	_data_offset += 4;
	memcpy(&this->m_window_height, _data_buf + _data_offset, 4);
	_data_offset += 4;
	memcpy(&this->m_is_mouse_transparent, _data_buf + _data_offset, 1);
	_data_offset += 1;
	memcpy(&this->m_is_top_most, _data_buf + _data_offset, 1);
	_data_offset += 1;

	_config_fs.seekg(_config_shortcut_key_begin);
	if (_config_fs.peek() == EOF)
	{
		_config_fs.clear();
		_config_fs.seekp(_config_shortcut_key_begin);
		char _shortcut_key_buf[_config_shortcut_key_buf_size] = {};
		int _shortcut_key_offset = 0;
		{
			HotkeyData _temp_node(0, true, true, false, 'Q');
			_temp_node.toBinary(_shortcut_key_buf + _shortcut_key_offset);
			_shortcut_key_offset += 8;
		}
		{
			HotkeyData _temp_node(0, true, true, false, 'T');
			_temp_node.toBinary(_shortcut_key_buf + _shortcut_key_offset);
			_shortcut_key_offset += 8;
		}
		_config_fs.write(_shortcut_key_buf, _config_shortcut_key_buf_size);
	}

	char _shortcut_key_buf[_config_shortcut_key_buf_size] = {};
	_config_fs.seekg(_config_shortcut_key_begin);
	_config_fs.read(_shortcut_key_buf, _config_shortcut_key_buf_size);

	{
		int _shortcut_key_offset = 0;
		m_hotkey_data_list["mouse_transparent"] = 
			new HotkeyData(
				_config_shortcut_key_begin + _shortcut_key_offset,
				_shortcut_key_buf + _shortcut_key_offset
			);
		_shortcut_key_offset += 8;
		m_hotkey_data_list["top_most"] = 
			new HotkeyData(
				_config_shortcut_key_begin + _shortcut_key_offset,
				_shortcut_key_buf + _shortcut_key_offset
			);
		_shortcut_key_offset += 8;
	}

	_config_fs.close();
}

void Config::setWindowX(int x)
{
	this->m_window_x = x;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_x, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		this->m_config_file_path, 
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(0);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void Config::setWindowY(int y)
{
	this->m_window_y = y;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_y, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		this->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(4);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void Config::setWindowWidth(int width)
{
	this->m_window_width = width;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_width, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		this->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(8);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void Config::setWindowHeight(int height)
{
	this->m_window_height = height;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_height, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		this->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(12);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void Config::setIsMouseTransparent(bool flag)
{
	this->m_is_mouse_transparent = flag; 
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_mouse_transparent, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		this->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(16);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void Config::setIsTopMose(bool flag)
{
	this->m_is_top_most = flag; 
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_top_most, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		this->m_config_file_path,
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(17);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

Config::HotkeyData* Config::getHotkeyData(std::string name) const
{
	std::unordered_map<std::string, HotkeyData*>::const_iterator _it =
		m_hotkey_data_list.find(name);
	if (_it == m_hotkey_data_list.end()) { return nullptr; }
	return _it->second;
}

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
			memcpy(&_data->last_add_time, info_buf + ToDoRepeatData::last_add_time_offset, 8);
			memcpy(&_data->duration, info_buf + ToDoRepeatData::duration_offset, 8);
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

void GlobalVariables::addRepeatData(std::string thing, ToDoRepeatData::RepeatType type, int64_t duration)
{
	ToDoRepeatData* _data = new ToDoRepeatData();
	_data->thing = thing;
	_data->repeat_type = type;
	_data->duration = duration;

	std::fstream data_fs;
	std::ofstream info_ofs;
	std::ofstream text_ofs;
	data_fs.open(this->getRepeatDataFilePath(), std::ios::binary | std::ios::app);
	data_fs.close();
	data_fs.open(
		this->getRepeatDataFilePath(),
		std::ios::binary | std::ios::out | std::ios::in
	);
	info_ofs.open(
		this->getRepeatInfoFilePath(),
		std::ios::binary | std::ios::app
	);
	text_ofs.open(
		this->getRepeatTextFilePath(),
		std::ios::binary | std::ios::app
	);
	data_fs.seekp(0, std::ios::end);
	if (data_fs.tellp() == 0)
	{
		int64_t _size = 8;
		char _size_str[8] = {};
		memcpy(_size_str, &_size, 8);
		data_fs.write(_size_str, 8);
	}

	data_fs.seekg(0, std::ios::beg);
	info_ofs.seekp(0, std::ios::end);
	text_ofs.seekp(0, std::ios::end);
	int64_t data_file_size = 0;
	int64_t info_ptr = 0;
	int64_t text_ptr = 0;
	char size_buf[8] = {};

	data_fs.read(size_buf, 8);
	memcpy(&data_file_size, size_buf, 8);
	_data->data_ptr = data_file_size;
	info_ptr = info_ofs.tellp();
	memset(size_buf, 0, 8);
	text_ptr = text_ofs.tellp();

	data_fs.seekp(data_file_size);
	{
		char data_buf[64] = {};
		memcpy(data_buf, &info_ptr, 8);
		data_fs.write(data_buf, 64);

		data_file_size += 64;
		memset(size_buf, 0, 8);
		memcpy(size_buf, &data_file_size, 8);
		data_fs.seekp(0, std::ios::beg);
		data_fs.write(size_buf, 8);
	}

	{
		memset(size_buf, 0, 8);
		int64_t thing_size = thing.size();
		memcpy(size_buf, &thing_size, 8);
		text_ofs.write(size_buf, 8);
		text_ofs.write(thing.data(), thing.size());
	}
	_data->text_ptr = text_ptr;

	{
		char info_buf[64] = {};
		char type_char = static_cast<char>(type);
		memcpy(info_buf + ToDoRepeatData::text_ptr_offset, &text_ptr, 8);
		memcpy(info_buf + ToDoRepeatData::repeat_type_offset, &type_char, 1);
		//memcpy(info_buf + ToDoRepeatData::last_add_time_offset, 0, 8);
		memcpy(info_buf + ToDoRepeatData::duration_offset, &_data->duration, 8);
		info_ofs.write(info_buf, 64);
	}
	_data->info_ptr = info_ptr;

	data_fs.close();
	info_ofs.close();
	text_ofs.close();
	repeat_data_list.emplace_back(_data);
}

void GlobalVariables::setRepeatDataThing(int index, std::string thing)
{
	if (repeat_data_list.size() <= index) { return; }
	repeat_data_list[index]->setThing(
		this->getRepeatInfoFilePath(), this->getRepeatTextFilePath(), thing
	);
}

void GlobalVariables::setRepeatDataLastAddTime(int index, int64_t time)
{
	if (repeat_data_list.size() <= index) { return; }
	repeat_data_list[index]->setLastAddTime(this->getRepeatInfoFilePath(), time);
}

void GlobalVariables::deleteRepeatData(int index)
{
	if (repeat_data_list.size() <= index) { return; }
	repeat_data_list[index]->deleteThis(this->getRepeatDataFilePath());
	delete repeat_data_list[index];
	repeat_data_list.erase(repeat_data_list.begin() + index);
}

//std::string Config::getHotkeyFuncKeyString() const
//{
//	std::string _res = "";
//	if (m_is_ctrl)
//	{
//		_res += "Ctrl";
//	}
//	if (m_is_alt)
//	{
//		if (_res.size()) { _res += "+"; }
//		_res += "Alt";
//	}
//	if (m_is_shift)
//	{
//		if (_res.size()) { _res += "+"; }
//		_res += "Shirt";
//	}
//	return _res;
//}