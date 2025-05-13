#define _CRT_SECURE_NO_WARNINGS
#include "ToDoApp.h"
#include "NSharedMemory.h"
#include "GlobalVariables.h"
#include "UIColorManager.h"
#include "ToDoData.h"
#include "Util.h"

#include <QApplication>

#include <fstream>
#include <json/json.h>

#include <QDateTime>
#include <cstdio>
#include <cerrno>
#include <cstring> 
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>

#include <QDir>

//#include <QtColorWidgets/color_dialog.hpp>

ToDoApp* w = nullptr;

static void loadResource();
static void toDoRepeatDataConsolidation();
static void toDoDataConsolidation();

static void systemTrayIconContext();
static void systemTrayIconOnActivated(QSystemTrayIcon::ActivationReason reason);

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	//NSharedMemory sharedMemory("20250405NNNCreated");
	//if (!sharedMemory.canUse())
	//{
	//	return -1;
	//}

	//color_widgets::ColorDialog cdlg;
	//cdlg.show();

	{
		QDir file_dir(
			QString::fromStdString(GlobalVariables::instance()->file_dir)
		);
		QDir temp_file_dir(
			QString::fromStdString(GlobalVariables::instance()->temp_file_dir)
		);
		if (!file_dir.exists()) 
		{
			file_dir.mkpath(file_dir.absolutePath());
		}
		if (!temp_file_dir.exists())
		{
			temp_file_dir.mkpath(temp_file_dir.absolutePath());
		}
	}


	Config::instance()->loadConfig();
	loadResource();

	UIColorManager::instance()->loadFromFile();
	UIColorManager::instance()->saveToFile();

	toDoRepeatDataConsolidation();
	toDoDataConsolidation();
	GlobalVariables::instance()->loadRepeatDataFromFile();

	w = new ToDoApp(); 

	// 加载任务数据
	{
		GlobalVariables* gv = GlobalVariables::instance();
		std::ifstream _data_ifs;
		std::ifstream _info_ifs;
		std::ifstream _text_ifs;
		_data_ifs.open(gv->getDataFilePath(), std::ios::binary | std::ios::in);
		_info_ifs.open(gv->getInfoFilePath(), std::ios::binary | std::ios::in);
		_text_ifs.open(gv->getTextFilePath(), std::ios::binary | std::ios::in);

		char _size_buf[8] = {};
		int64_t _data_file_size = 0;
		_data_ifs.read(_size_buf, 8);
		memcpy(&_data_file_size, _size_buf, 8);

		while (_data_ifs.tellg() < _data_file_size && _data_ifs.peek() != EOF)
		{
			uint64_t _data_ptr = _data_ifs.tellg();
			uint64_t _info_ptr = 0;
			uint64_t _text_ptr = 0;
			char _data[64] = { };
			char _info[128] = { };
			_data_ifs.read(_data, 64);
			memcpy(&_info_ptr, _data, 8);

			_info_ifs.seekg(_info_ptr);
			_info_ifs.read(_info, 128);
			bool _is_finished = false;
			int64_t _create_time = 0;
			char _deadline_type_char = 0;
			int64_t _deadline_date = 0;
			int64_t _deadline_time = 0;
			int64_t _finished_time = 0;
			memcpy(&_text_ptr, _info + ToDoData::text_ptr_offset, 8);
			memcpy(&_is_finished, _info + ToDoData::is_finished_offset, 1);
			memcpy(&_create_time, _info + ToDoData::create_date_time_offset, 8);
			//memcpy(&deadline_type_char, _info + ToDoData::deadline_type_offset, 1);
			memcpy(&_deadline_date, _info + ToDoData::deadline_date_offset, 8);
			memcpy(&_deadline_time, _info + ToDoData::deadline_time_offset, 8);
			memcpy(&_finished_time, _info + ToDoData::finished_date_time_offset, 8);
			//ToDoData::DeadlineType deadline_type =
			//	static_cast<ToDoData::DeadlineType>(deadline_type_char);

			size_t _text_size = 0;
			char _text_size_char[8] = { };
			char _text_buf[1024] = { };
			_text_ifs.seekg(_text_ptr);
			_text_ifs.read(_text_size_char, 8);
			memcpy(&_text_size, _text_size_char, 8);
			_text_ifs.read(_text_buf, _text_size);
			std::string thing(_text_buf, _text_size);
			w->addToDoListItem(
				_data_ptr ,_info_ptr, _text_ptr,
				thing, _is_finished, _create_time,// deadline_type, 
				_deadline_date, _deadline_time, _finished_time
			);
		}
		_data_ifs.close();
		_info_ifs.close();
		_text_ifs.close();
	}

	// 加载热键
	{
		Config* config = Config::instance();
		GlobalVariables* gv = GlobalVariables::instance();
		gv->mouse_transparent_hotkey = new QHotkey(
			QKeySequence(
				QString::fromStdString(
					config->getHotkeyData("mouse_transparent")->getHotkeyString()
				)
			), true
		);
		QObject::connect(gv->mouse_transparent_hotkey,
			&QHotkey::activated,
			[]() {
				w->changeMouseTransparent();
			}
		);
		gv->top_most_hotkey = new QHotkey(
			QKeySequence(
				QString::fromStdString(
					config->getHotkeyData("top_most")->getHotkeyString()
				)
			), true
		);
		QObject::connect(gv->top_most_hotkey,
			&QHotkey::activated,
			[]() {
				w->changeTopMost();
			}
		);
	}

	w->show();

	QIcon icon = QIcon(":/public/resource/ico.ico");
	QSystemTrayIcon sysTray(icon);
	sysTray.connect(&sysTray, &QSystemTrayIcon::activated, systemTrayIconOnActivated);
	sysTray.show();

	w->setWindowIcon(icon);

	return a.exec();
}


void loadResource()
{
	GlobalVariables* gv = GlobalVariables::instance();
	{
		gv->delete_png = new QPixmap(
			//QString::fromStdString(gv->resource_dir + "/" + "delete.png")
			":/public/resource/delete.png"
		);
		QPainter _painter(gv->delete_png);
		_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
		_painter.fillRect(gv->delete_png->rect(), QColor(180, 180, 180));
		_painter.end();
	}
	{
		gv->setting_png = new QPixmap(
			//QString::fromStdString(gv->resource_dir + "/" + "setting.png")
			":/public/resource/setting.png"
		);
		QPainter _painter(gv->setting_png);
		_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
		_painter.fillRect(gv->setting_png->rect(), QColor(200, 200, 200, 200));
		_painter.end();
	}
}

void toDoRepeatDataConsolidation()
{
	GlobalVariables* gv = GlobalVariables::instance();
	std::ifstream _data_ifs;
	std::ifstream _info_ifs;
	std::ifstream _text_ifs;
	std::ofstream _temp_data_ofs;
	std::ofstream _temp_info_ofs;
	std::ofstream _temp_text_ofs;
	_data_ifs.open(gv->getRepeatDataFilePath(), std::ios::binary | std::ios::in);
	_info_ifs.open(gv->getRepeatInfoFilePath(), std::ios::binary | std::ios::in);
	_text_ifs.open(gv->getRepeatTextFilePath(), std::ios::binary | std::ios::in);
	if (!_data_ifs.is_open() || !_info_ifs.is_open() || !_text_ifs.is_open())
	{
		return;
	}
	_temp_data_ofs.open(
		gv->getTempRepeatDataFilePath(),
		std::ios::binary | std::ios::out | std::ios::trunc
	);
	_temp_info_ofs.open(
		gv->getTempRepeatInfoFilePath(),
		std::ios::binary | std::ios::out | std::ios::trunc
	);
	_temp_text_ofs.open(
		gv->getTempRepeatTextFilePath(),
		std::ios::binary | std::ios::out | std::ios::trunc
	);

	_data_ifs.seekg(0, std::ios::beg);
	char _size_buf[8] = {};
	int64_t _data_file_size = 0;
	_data_ifs.read(_size_buf, 8);
	memcpy(&_data_file_size, _size_buf, 8);
	_temp_data_ofs.write(_size_buf, 8);

	while (_data_ifs.tellg() < _data_file_size && _data_ifs.peek() != EOF)
	{
		char _data_buf[64] = {};
		char _info_buf[64] = {};
		char _text_buf[1024] = {};
		int64_t info_ptr = 0;
		int64_t text_ptr = 0;
		int64_t text_size = 0;
		_data_ifs.read(_data_buf, 64);
		memcpy(&info_ptr, _data_buf, 8);
		_info_ifs.seekg(info_ptr);
		_info_ifs.read(_info_buf, 64);
		memcpy(&text_ptr, _info_buf + ToDoRepeatData::text_ptr_offset, 8);
		_text_ifs.seekg(text_ptr);
		memset(_size_buf, 0, 8);
		_text_ifs.read(_size_buf, 8);
		memcpy(&text_size, _size_buf, 8);
		_text_ifs.read(_text_buf, text_size);

		text_ptr = _temp_text_ofs.tellp();
		_temp_text_ofs.write(_size_buf, 8);
		_temp_text_ofs.write(_text_buf, text_size);
		memcpy(_info_buf + ToDoRepeatData::text_ptr_offset, &text_ptr, 8);
		info_ptr = _temp_info_ofs.tellp();
		_temp_info_ofs.write(_info_buf, 64);
		memcpy(_data_buf, &info_ptr, 8);
		_temp_data_ofs.write(_data_buf, 64);
	}

	_data_ifs.close();
	_info_ifs.close();
	_text_ifs.close();
	_temp_data_ofs.close();
	_temp_info_ofs.close();
	_temp_text_ofs.close();

	int res = 0;
	std::remove(gv->getRepeatDataFilePath().data());
	std::remove(gv->getRepeatInfoFilePath().data());
	std::remove(gv->getRepeatTextFilePath().data());
	res = std::rename(
		gv->getTempRepeatDataFilePath().data(),
		gv->getRepeatDataFilePath().data()
	);
	res = std::rename(
		gv->getTempRepeatInfoFilePath().data(),
		gv->getRepeatInfoFilePath().data()
	);
	res = std::rename(
		gv->getTempRepeatTextFilePath().data(),
		gv->getRepeatTextFilePath().data()
	);
}

void toDoDataConsolidation()
{
	int64_t out_time_day = 3;
	GlobalVariables* gv = GlobalVariables::instance();
	std::ifstream _data_ifs;
	std::ifstream _info_ifs;
	std::ifstream _text_ifs;
	std::ofstream _temp_data_ofs;
	std::ofstream _temp_info_ofs;
	std::ofstream _temp_text_ofs;
	_data_ifs.open(gv->getDataFilePath(), std::ios::binary | std::ios::in);
	_info_ifs.open(gv->getInfoFilePath(), std::ios::binary | std::ios::in);
	_text_ifs.open(gv->getTextFilePath(), std::ios::binary | std::ios::in);
	if (!_data_ifs.is_open() || !_info_ifs.is_open() || !_text_ifs.is_open())
	{
		return;
	}
	_temp_data_ofs.open(gv->getTempDataFilePath(),
		std::ios::binary | std::ios::out | std::ios::trunc
	);
	_temp_info_ofs.open(gv->getTempInfoFilePath(),
		std::ios::binary | std::ios::out | std::ios::trunc
	);
	_temp_text_ofs.open(gv->getTempTextFilePath(),
		std::ios::binary | std::ios::out | std::ios::trunc
	);

	_data_ifs.seekg(0, std::ios::beg); 
	char _size_buf[8] = {}; 
	int64_t _data_file_size = 0;  
	_data_ifs.read(_size_buf, 8); 
	memcpy(&_data_file_size, _size_buf, 8); 
	_temp_data_ofs.write(_size_buf, 8); 

	while (_data_ifs.tellg() < _data_file_size && _data_ifs.peek() != EOF)
	{
		uint64_t _info_ptr = 0;
		uint64_t _text_ptr = 0;
		uint64_t _new_info_ptr = 0;
		uint64_t _new_text_ptr = 0;
		char _data[64] = { };
		char _info[128] = { };
		_data_ifs.read(_data, 64);
		memcpy(&_info_ptr, _data, 8);

		_info_ifs.seekg(_info_ptr);
		_info_ifs.read(_info, 128);
		bool _is_finished = false;
		int64_t _create_time = 0;
		char _deadline_type_char = 0;
		int64_t _deadline_date = 0;
		int64_t _deadline_time = 0;
		int64_t _finished_time = 0;
		memcpy(&_text_ptr, _info + ToDoData::text_ptr_offset, 8);
		memcpy(&_is_finished, _info + ToDoData::is_finished_offset, 1);
		memcpy(&_create_time, _info + ToDoData::create_date_time_offset, 8);
		//memcpy(&deadline_type_char, _info + ToDoData::deadline_type_offset, 1);
		memcpy(&_deadline_date, _info + ToDoData::deadline_date_offset, 8);
		memcpy(&_deadline_time, _info + ToDoData::deadline_time_offset, 8);
		memcpy(&_finished_time, _info + ToDoData::finished_date_time_offset, 8);
		//ToDoData::DeadlineType deadline_type =
		//	static_cast<ToDoData::DeadlineType>(deadline_type_char);

		if (_is_finished &&
			QDateTime(QDate::currentDate(), QTime(0, 0, 0))
			.toMSecsSinceEpoch() - _finished_time >
			out_time_day * 24 * 60 * 60 * 1000)
		{
			continue;
		}

		size_t _text_size = 0;
		char _text_size_char[8] = { };
		char _text_buf[1024] = { };
		_text_ifs.seekg(_text_ptr);
		_text_ifs.read(_text_size_char, 8);
		memcpy(&_text_size, _text_size_char, 8);
		_text_ifs.read(_text_buf, _text_size);
		std::string _thing(_text_buf, _text_size);

		_new_info_ptr = _temp_info_ofs.tellp();
		_new_text_ptr = _temp_text_ofs.tellp();
		memcpy(_info + ToDoData::text_ptr_offset, &_new_text_ptr, 8);
		memcpy(_data, &_new_info_ptr, 8);

		_temp_data_ofs.write(_data, 64);
		_temp_info_ofs.write(_info, 128);
		_temp_text_ofs.write(_text_size_char, 8);
		_temp_text_ofs.write(_text_buf, _text_size);
	}
	_data_ifs.close();
	_info_ifs.close();
	_text_ifs.close();
	_temp_data_ofs.close();
	_temp_info_ofs.close();
	_temp_text_ofs.close();

	int _res = 0;
	std::remove(gv->getDataFilePath().data());
	std::remove(gv->getInfoFilePath().data());
	std::remove(gv->getTextFilePath().data());
	_res = std::rename(
		gv->getTempDataFilePath().data(), gv->getDataFilePath().data()
	);
	//if (!res)
	//{
	//	QMessageBox::warning(nullptr, "Data文件覆盖错误", std::strerror(errno));
	//}
	_res = std::rename(
		gv->getTempInfoFilePath().data(), gv->getInfoFilePath().data()
	);
	//if (!res)
	//{
	//	QMessageBox::warning(nullptr, "Info文件覆盖错误", std::strerror(errno));
	//}
	_res = std::rename(
		gv->getTempTextFilePath().data(), gv->getTextFilePath().data()
	);
	//if (!res)
	//{
	//	QMessageBox::warning(nullptr, "Text文件覆盖错误", std::strerror(errno));
	//}
}

void systemTrayIconContext()
{
	QMenu menu;

	QAction* setting_act = menu.addAction("设置");
	QObject::connect(
		setting_act, &QAction::triggered,
		w, &ToDoApp::showSettingWidget
	);

	QAction* add_to_do_data_act = menu.addAction("添加任务");
	QObject::connect(
		add_to_do_data_act, &QAction::triggered,
		w, &ToDoApp::showToDoDataAddDialog
	);

	QAction* top_most_change_act = menu.addAction("设置/取消窗口置顶");
	QObject::connect(top_most_change_act, &QAction::triggered,
		[]() {
			w->changeTopMost();
		}
	);

	QAction* mouse_transparent_change_act = menu.addAction("设置/取消鼠标穿透");
	QObject::connect(mouse_transparent_change_act, &QAction::triggered,
		[]() {
			w->changeMouseTransparent();
		}
	);

	QAction* close_act = menu.addAction("close");
	QObject::connect(close_act, &QAction::triggered,
		[]() {
			QApplication::quit();
		}
	);
	menu.exec(QCursor::pos());
}
void systemTrayIconOnActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Unknown:
		break;
	case QSystemTrayIcon::Context:
		systemTrayIconContext();
		break;
	case QSystemTrayIcon::DoubleClick:
		break;
	case QSystemTrayIcon::Trigger:
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		break;
	}
}
//#include <iostream>
//#include <QWidget>
//#include <QApplication>
//#include <QPushButton>
//
//int main(int argc, char* argv[])
//{
//	QApplication a(argc, argv);
//
//	return a.exec();
//}