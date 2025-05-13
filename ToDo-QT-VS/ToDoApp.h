#pragma once

#include "SettingWidget.h"
#include "ToDoListView.h"
#include "ToDoData.h"
#include "Util.h"

#include <QWidget>
#include <QRubberBand>
#include <QPainter>
#include <vector>
#include <QPushButton>

class ToDoApp : public QWidget
{
	Q_OBJECT
public:
	ToDoApp(QWidget* parent = nullptr);
	~ToDoApp();
	void changeTopMost();
	void changeMouseTransparent();
	void addToDoListItem(
		int64_t data_ptr, int64_t info_ptr, int64_t text_ptr,
		std::string thing, bool is_finished, 
		int64_t create_time,
		//ToDoData::DeadlineType deadline_type,
		int64_t deadline_date, int64_t deadline_time, 
		int64_t finished_time 
	);
	void addToDoListItem(
		std::string thing, bool is_finished,
		//ToDoData::DeadlineType deadline_type, 
		int64_t deadline_date, int64_t deadline_time,
		int64_t finished_time
	);
	void showToDoDataAddDialog()
	{
		if (m_to_do_data_add_dialog->isVisible()) { return; } 
		m_to_do_data_add_dialog->show(); 
	}
	void showSettingWidget()
	{
		if(m_setting_widget == nullptr) 
		{
			m_setting_widget = new SettingWidget(this);
		}
		if (m_setting_widget->isVisible()) { return; }
		m_setting_widget->show();
	}
protected:
	void mousePressEvent(QMouseEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	void mouseHitTest(QMouseEvent* ev);
	void updataData();
	void initToDoDataAddDialog();
private:
	QTimer* m_game_timer = nullptr;

	QPoint m_drag_position;
	QPoint m_last_mouse_position;

	int m_top_box_height = 20;
	int m_date_info_y = m_top_box_height + 1; // time info 的 y
	int m_date_info_box_height = 40;
	int m_list_view_y = m_date_info_y + m_date_info_box_height; // list view 的 y

	QFont m_now_year_font = QFont("Serif", 18, QFont::Bold);
	QFont m_now_mouth_day_font = QFont("Serif", 8);

	QPushButton* m_to_do_data_add = nullptr;
	QDialog* m_to_do_data_add_dialog = nullptr;
	ToDoListView* m_list_view = nullptr;

	SettingWidget* m_setting_widget = nullptr;

	QRubberBand* m_rubber_band = nullptr; 
	QPoint m_rubber_band_left_top;                    //相对与屏幕
	QPoint m_rubber_band_right_bottom;                //相对与屏幕
	bool m_is_rubber_band_show = false;
	enum class MouseHitTest
	{
		Normal,
		Top,
		Left,
		Bottom,
		Right,
		LeftTop,
		LeftBottom,
		RightBottom,
		RightTop,
		Move
	};
	MouseHitTest m_mouse_hit_test = MouseHitTest::Normal;

	std::vector<ToDoData*> m_to_do_data_list;
	bool m_is_data_consolidation = false;

	int m_setting_png_width = 15;
	int m_setting_png_padding = 5;
};