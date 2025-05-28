#define _CRT_SECURE_NO_WARNINGS
#include "ToDoApp.h"
#include "NSharedMemory.h"
#include "GlobalVariables.h"
#include "UIColorManager.h"
#include "ToDoData.h"
#include "Util.h"
#include "DataBase.h"
#include "Snowflake.h"
#include "Config.h"

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

static void systemTrayIconContext();
static void systemTrayIconOnActivated(QSystemTrayIcon::ActivationReason reason);

int main(int argc, char* argv[])
{
	Snowflake::instance()->setStartTimestamp(
		QDateTime(QDate(2025, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch()
	);
	Snowflake::instance()->init();

	QApplication a(argc, argv);

#ifdef _DEBUG
	NSharedMemory sharedMemory("20250405NNNCreatedDEBUG");
#else
	NSharedMemory sharedMemory("20250405NNNCreated");
#endif // _DEBUG

	if (!sharedMemory.canUse())
	{
		return -1;
	}

	//color_widgets::ColorDialog cdlg;
	//cdlg.show();

	Config::instance()->loadFromFile();
	Config::instance()->saveToFile();

	{
		QDir file_dir(
			QString::fromStdString(Config::instance()->file_dir)
		);
		if (!file_dir.exists()) 
		{
			file_dir.mkpath(file_dir.absolutePath());
		}
	}


	AppInfoData::instance()->loadFromFile();
	loadResource();

	UIColorManager::instance()->loadFromFile();
	UIColorManager::instance()->saveToFile();

	GlobalVariables::instance()->data_database =
		new DataBase(Config::instance()->getDataBaseFilePath());
	if (!GlobalVariables::instance()->data_database->tableExists("tasks"))
	{
		GlobalVariables::instance()->databaseCreateTaskTable();
	}
	if (!GlobalVariables::instance()->data_database->tableExists("repeat_tasks"))
	{
		GlobalVariables::instance()->databaseCreateRepeatTaskTable();
	}

	GlobalVariables::instance()->databaseCheckTaskTable();
	GlobalVariables::instance()->databaseCheckRepeatTaskTable();

	GlobalVariables::instance()->loadRepeatDataFromFile();

	w = new ToDoApp(); 

	// 加载任务数据
	{
		GlobalVariables* gv = GlobalVariables::instance();
		DataBase* _db = GlobalVariables::instance()->data_database;
		DBQueryRes _res = _db->DBQuery("tasks", "*");
		for (int _i = 0; _i < _res.rows; ++_i)
		{
			w->addToDoListItem(
				str2int64_t(_res["id"][_i]), _res["task"][_i],
				_res["is_finished"][_i] == "1" ? true : false,
				str2int64_t(_res["deadline_date"][_i]), 
				str2int64_t(_res["deadline_time"][_i]),
				str2int64_t(_res["finished_date_time"][_i])
			);
		}
	}

	// 绑定热键
	{
		AppInfoData* app_info_data = AppInfoData::instance();
		QObject::connect(
			app_info_data->getHotkeyData("mouse_transparent")->hotkey(),
			&QHotkey::activated,
			[]() {
				w->changeMouseTransparent();
			}
		);
		QObject::connect(
			app_info_data->getHotkeyData("top_most")->hotkey(),
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
			":/public/resource/delete.png"
		);
		QPainter _painter(gv->delete_png);
		_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
		_painter.fillRect(gv->delete_png->rect(), QColor(180, 180, 180));
		_painter.end();
	}
	{
		gv->setting_png = new QPixmap(
			":/public/resource/setting.png"
		);
		QPainter _painter(gv->setting_png);
		_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
		_painter.fillRect(gv->setting_png->rect(), QColor(200, 200, 200, 200));
		_painter.end();
	}
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