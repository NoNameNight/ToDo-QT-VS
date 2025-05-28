#include <QMessageBox>
#include "Util.h"

#include "Config.h"

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

class NMessageBox : public QMessageBox
{
public:
	NMessageBox(QWidget* parent = nullptr):
		QMessageBox(parent)
	{ }
	NMessageBox(Icon icon, const QString& title, const QString& text,
		StandardButtons buttons = NoButton, QWidget* parent = nullptr,
		Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint):
		QMessageBox(icon, title, text, buttons, parent, flags)
	{ }

	QSize sizeHint() const override
	{
		QSize _size = QMessageBox::sizeHint();
		if (_size.width() < this->minimumWidth())
		{
			_size.setWidth(this->minimumWidth());
		}
		if (_size.height() < this->minimumHeight())
		{
			_size.setHeight(this->minimumHeight());
		}
		return _size;
	}

	void showEvent(QShowEvent* event) override
	{
		QMessageBox::showEvent(event);
		QSize _size = this->sizeHint();
		this->resize(_size);
	}
};
void fatalError(const std::string error)
{
	//QMessageBox::warning(nullptr, "你不应该看到这个", QString::fromStdString(error));
	static QMessageBox _message_box; 
	static bool is_init = false;
	if (!is_init)
	{
		is_init = true;
		_message_box.setWindowTitle("你不应该看到这个");
		_message_box.setIcon(QMessageBox::Critical);
		_message_box.addButton(QMessageBox::Ok);
		_message_box.setStyleSheet(
			"QMessageBox {"
			"min-width:100px;"
			"min-height:80px;"
			"}"
		);
	}

	_message_box.setText(QString::fromStdString(error));
	_message_box.exec();

	exit(-1);
}

int32_t str2int32_t(const std::string& str)
{
	int32_t res = 0;
	for (const char& c : str)
	{
		res = res * 10 + (c - '0');
	}
	return res;
}

int64_t str2int64_t(const std::string& str)
{
	int64_t res = 0;
	for (const char& c : str)
	{
		res = res * 10 + (c - '0');
	}
	return res;
}

AppInfoData::HotkeyData::HotkeyData(int64_t data_ptr, const char* buf)
{
	this->m_config_ptr = data_ptr;
	int _shortcut_key_offset = 0;
	memcpy(&this->m_is_ctrl, buf + _shortcut_key_offset, 1);
	_shortcut_key_offset += 1;
	memcpy(&this->m_is_alt, buf + _shortcut_key_offset, 1);
	_shortcut_key_offset += 1;
	memcpy(&this->m_is_shift, buf + _shortcut_key_offset, 1);
	_shortcut_key_offset = 7;
	memcpy(&this->m_hotkey_char, buf + _shortcut_key_offset, 1);
	_shortcut_key_offset += 1;
}

void AppInfoData::HotkeyData::toBinary(char* buf) const
{
	int _shortcut_key_offset = 0;
	memcpy(buf + _shortcut_key_offset, &this->m_is_ctrl, 1);
	_shortcut_key_offset += 1;
	memcpy(buf + _shortcut_key_offset, &this->m_is_alt, 1);
	_shortcut_key_offset += 1;
	memcpy(buf + _shortcut_key_offset, &this->m_is_shift, 1);
	_shortcut_key_offset = 7;
	memcpy(buf + _shortcut_key_offset, &this->m_hotkey_char, 1);
	_shortcut_key_offset += 1;
}
void AppInfoData::HotkeyData::setIsCtrl(bool flag)
{
	int _is_ctrl_ptr = this->m_config_ptr;
	this->m_is_ctrl = flag;
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_ctrl, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//AppInfoData::instance()->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void AppInfoData::HotkeyData::setIsAlt(bool flag)
{
	int _is_ctrl_ptr = this->m_config_ptr + 1;
	this->m_is_alt = flag;
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_alt, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//AppInfoData::instance()->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void AppInfoData::HotkeyData::setIsShift(bool flag)
{
	int _is_ctrl_ptr = this->m_config_ptr + 2;
	this->m_is_shift = flag;
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_shift, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//AppInfoData::instance()->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void AppInfoData::HotkeyData::setHotkeyChar(char c)
{
	int _is_ctrl_ptr = this->m_config_ptr + 7;
	this->m_hotkey_char = c;
	std::ofstream _config_ofs;
	_config_ofs.open(
		//AppInfoData::instance()->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(_is_ctrl_ptr);
	_config_ofs.write(&this->m_hotkey_char, 1);
	_config_ofs.close();
}

std::string AppInfoData::HotkeyData::getHotkeyFuncKeyString() const
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


void AppInfoData::loadFromFile()
{
	const int64_t _config_data_begin = 0;
	const int64_t _config_data_buf_size = 64;
	const int64_t _config_shortcut_key_begin = 
		_config_data_begin + _config_data_buf_size;
	const int64_t _config_shortcut_key_buf_size = 128;
	const int64_t _config_ui_color_begin = 
		_config_shortcut_key_begin + _config_shortcut_key_buf_size;
	const int64_t _config_ui_color_buf_size = 1024;
	std::fstream _config_fs;
	_config_fs.open(
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::out | std::ios::in
	);

	if (!_config_fs.is_open() || !_config_fs.good())
	{
		_config_fs.open(
			Config::instance()->getWindowStateFilePath(),
			std::ios::binary | std::ios::app
		);
		_config_fs.close();
		_config_fs.open(
			//this->m_window_state_file_path,
			Config::instance()->getWindowStateFilePath(),
			std::ios::binary | std::ios::out | std::ios::in
		);
	}

	// 加载窗口配置
	{
		_config_fs.seekg(_config_data_begin);
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
	}

	// 加载快捷键
	{
		_config_fs.seekg(_config_shortcut_key_begin);
		if (_config_fs.peek() == EOF)
		{
			_config_fs.clear();
			_config_fs.seekp(_config_shortcut_key_begin);
			char _shortcut_key_buf[_config_shortcut_key_buf_size] = {};
			int _shortcut_key_offset = 0;
			{
				HotkeyData _temp_node(-1, true, true, false, 'Q');
				_temp_node.toBinary(_shortcut_key_buf + _shortcut_key_offset);
				_shortcut_key_offset += 8;
			}
			{
				HotkeyData _temp_node(-1, true, true, false, 'T');
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
			for (const auto& _name : m_hotkey_name_list)
			{
				m_hotkey_data_list[_name] =
					new HotkeyData(
						_config_shortcut_key_begin + _shortcut_key_offset,
						_shortcut_key_buf + _shortcut_key_offset
					);
				m_hotkey_data_list[_name]->setHotkey(
					new QHotkey(
						QKeySequence(
							QString::fromStdString(
								m_hotkey_data_list[_name]->getHotkeyString()
							)
						), true
					)
				);
				_shortcut_key_offset += 8;
			}
			//m_hotkey_data_list["mouse_transparent"] =
			//	new HotkeyData(
			//		_config_shortcut_key_begin + _shortcut_key_offset,
			//		_shortcut_key_buf + _shortcut_key_offset
			//	);
			//_shortcut_key_offset += 8;
			//m_hotkey_data_list["top_most"] =
			//	new HotkeyData(
			//		_config_shortcut_key_begin + _shortcut_key_offset,
			//		_shortcut_key_buf + _shortcut_key_offset
			//	);
			//_shortcut_key_offset += 8;
		}
	}

	_config_fs.close();
}

void AppInfoData::setWindowX(int x)
{
	this->m_window_x = x;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_x, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//this->m_window_state_file_path, 
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(0);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void AppInfoData::setWindowY(int y)
{
	this->m_window_y = y;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_y, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//this->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(4);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void AppInfoData::setWindowWidth(int width)
{
	this->m_window_width = width;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_width, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//this->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(8);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void AppInfoData::setWindowHeight(int height)
{
	this->m_window_height = height;
	char _buffer[4] = {};
	memcpy(_buffer, &this->m_window_height, 4);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//this->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(12);
	_config_ofs.write(_buffer, 4);
	_config_ofs.close();
}

void AppInfoData::setIsMouseTransparent(bool flag)
{
	this->m_is_mouse_transparent = flag; 
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_mouse_transparent, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//this->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(16);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

void AppInfoData::setIsTopMose(bool flag)
{
	this->m_is_top_most = flag; 
	char _buffer[1] = {};
	memcpy(_buffer, &this->m_is_top_most, 1);
	std::ofstream _config_ofs;
	_config_ofs.open(
		//this->m_window_state_file_path,
		Config::instance()->getWindowStateFilePath(),
		std::ios::binary | std::ios::in | std::ios::out
	);
	_config_ofs.seekp(17);
	_config_ofs.write(_buffer, 1);
	_config_ofs.close();
}

AppInfoData::HotkeyData* AppInfoData::getHotkeyData(std::string name) const
{
	std::unordered_map<std::string, HotkeyData*>::const_iterator _it =
		m_hotkey_data_list.find(name);
	if (_it == m_hotkey_data_list.end()) { return nullptr; }
	return _it->second;
}
