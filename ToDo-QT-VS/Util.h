#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <boost/uuid.hpp>
#include <fstream>
#include <QPixmap>
#include <QHotkey>
#include <string>

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

void fatalError(const std::string error);

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
		//friend class Config;
	public:
		/**
		* @param config_ptr  data 在 config 文件中的位置
		*/
		HotkeyData(
			int64_t config_ptr,
			bool is_ctrl, bool is_alt, bool is_shift, char c
		) :
			m_is_ctrl(is_ctrl), m_is_alt(is_alt), m_is_shift(is_shift),
			m_hotkey_char(c), m_config_ptr(config_ptr)
		{ }
		/**
		* @param data_ptr  data 在 config 文件中的位置
		* @param buf       大小8B
		*/
		HotkeyData(int64_t data_ptr, const char* buf);
		/**
		* @param buf       大小8B
		*/
		void toBinary(char* buf) const;

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