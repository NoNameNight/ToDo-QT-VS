#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <boost/uuid.hpp>
#include <fstream>
#include <QPixmap>
#include <QHotkey>

enum class GenderatorUUIDType
{
	random = 0,
	randomMt19937,
	randomPure,
	timeV1,
	timeV6,
	timeV7
};
static std::string getUUID(GenderatorUUIDType type = GenderatorUUIDType::timeV7);

class ToDoData
{
public:
	//enum class DeadlineType : char
	//{
	//	Day,
	//	Time
	//};
public:
	int64_t data_ptr = 0;
	int64_t info_ptr = 0;
	int64_t text_ptr = 0; 
	//std::string uuid = "";
	std::string thing = "";
	bool is_finished = false;
	int64_t create_date_time = 0;
	//DeadlineType deadline_type = DeadlineType::Day;
	int64_t deadline_date = 0;    // 截止日期
	int64_t deadline_time = 0;    // 截止日期那天的时间
	int64_t finished_date_time = 0;

	void setThing(
		std::string info_file_path, std::string text_file_path,
		std::string thing
	);
	void setIsFinished(std::string info_file_path, bool flag);
	void setFinishedTime(std::string info_file_path, int64_t time);

	bool operator<(const ToDoData& other) const;
public:
	static const int text_ptr_offset;
	static const int is_finished_offset;
	static const int create_date_time_offset;
	//static const int deadline_type_offset;
	static const int deadline_date_offset;
	static const int deadline_time_offset;
	static const int finished_date_time_offset;

	static const int this_size;
};

class ToDoRepeatData
{
public:
	enum class RepeatType : char
	{
		EveryDay,
		EveryWeek,
		EveryMonth,
		EveryYear
	};
public:
	int64_t data_ptr = 0;
	int64_t info_ptr = 0;
	int64_t text_ptr = 0;
	RepeatType repeat_type;
	std::string thing = "";
	int64_t last_add_date_time = 0;
	int64_t duration = 0;
	int64_t deadline_time = 0;  // 结束那天的时间

	std::string getRepeatTypeString() const
	{
		return this->getRepeatTypeString(this->repeat_type);
	}
	std::string getRepeatTypeString(RepeatType type) const;

	void setThing(
		std::string info_file_path, std::string text_file_path,
		std::string thing
	);
	void setRepeatType(std::string info_file_path, RepeatType type);
	void setLastAddDateTime(std::string info_file_path, int64_t time);
	void setDuration(std::string info_file_path, int64_t duration);
	void setDeadlineTime(std::string info_file_path, int64_t time);
	void deleteThis(std::string data_file_path);

	bool isNeedRepeat(int64_t now_time) const;
public:
	static const int text_ptr_offset;
	static const int repeat_type_offset;
	static const int last_add_date_time_offset;
	static const int duration_offset;
	static const int deadline_time_offset;
	//static const int this_offset;
	static const int this_size;
};

#include <QDateTimeEdit>
class NNNDateEditor : public QDateTimeEdit
{
public:
	NNNDateEditor(QWidget* parent = nullptr) :
		QDateTimeEdit(parent)
	{
		this->setDate(QDate::currentDate());
		this->setDisplayFormat("yyyy/MM/dd");
		this->setMinimumDate(QDate::currentDate());
		this->setCalendarPopup(true);
	}
};

class NNNTimeEditor : public QTimeEdit
{
public:
	NNNTimeEditor(QWidget* parent = nullptr) :
		QTimeEdit(parent)
	{
		this->setTime(QTime::currentTime());
		this->setDisplayFormat("hh:mm:ss");
	}

};

class Config
{
public:
	class HotkeyData
	{
		friend class Config;
	public:
		HotkeyData(
			int64_t config_ptr, 
			bool is_ctrl, bool is_alt, bool is_shift, char c
		) :
			m_is_ctrl(is_ctrl), m_is_alt(is_alt), m_is_shift(is_shift), 
			m_hotkey_char(c), m_config_ptr(config_ptr)
		{ }
		/**
		* @param data_ptr  data 在文件中的位置
		* @param buf       大小8B
		*/
		HotkeyData(int64_t data_ptr, const char* buf)
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
		/**
		* @param buf       大小8B
		*/
		void toBinary(char* buf) const
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
		void setIsCtrl(bool flag);
		bool isCtrl() const { return m_is_ctrl; }
		void setIsAlt(bool flag);
		bool isAlt() const { return m_is_alt; }
		void setIsShift(bool flag);
		bool isShift() const { return m_is_shift; }
		void setHotkeyChar(char c);
		char hotkeyChar() const { return m_hotkey_char; }
		std::string getHotkeyString() const
		{
			return this->getHotkeyFuncKeyString() + "+" + m_hotkey_char;
		}
	private:
		std::string getHotkeyFuncKeyString() const;
	private:
		int64_t m_config_ptr = 0; 
		bool m_is_ctrl = true; 
		bool m_is_alt = true;
		bool m_is_shift = false;
		char m_hotkey_char = ' ';
	};
public:
	static Config* instance()
	{
		static Config config;
		return &config;
	}
	void loadConfig();
	void setWindowX(int x);
	int windowX() const { return m_window_x; }
	void setWindowY(int y);
	int windowY() const { return m_window_y; }
	void setWindowWidth(int width);
	int windowWidth() const { return m_window_width; }
	void setWindowHeight(int height);
	int windowHeight() const { return m_window_height; }
	void setIsMouseTransparent(bool flag);
	bool isMouseTransparent() const { return m_is_mouse_transparent; }
	void setIsTopMose(bool flag);
	bool isTopMost() const { return m_is_top_most; }

	HotkeyData* getHotkeyData(std::string name) const;
private:
	Config() = default;
	Config(const Config& other) = delete;
	~Config() = default;

	//std::string getHotkeyFuncKeyString() const;
private:
	std::string m_config_file_path = "config.data";
	int m_window_x = 100;
	int m_window_y = 100;
	int m_window_width = 302;
	int m_window_height = 186;
	bool m_is_mouse_transparent = false;
	bool m_is_top_most = true;

	std::unordered_map<std::string, HotkeyData*> m_hotkey_data_list;
};

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
		int64_t duration, int64_t deadline_time
	);
	void setRepeatDataThing(int index, std::string thing);
	void setRepeatDataLastAddDateTime(int index, int64_t time);
	void deleteRepeatData(int index);
	std::vector<ToDoRepeatData*> repeat_data_list;

private:
	GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& other) = delete;
	~GlobalVariables() = default;
};