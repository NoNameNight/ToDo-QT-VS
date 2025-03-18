#pragma once

#include "ToDoListView.h"

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
protected:
	void mousePressEvent(QMouseEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	void mouseHitTest(QMouseEvent* ev);
	void updataData();
private:
	QTimer* m_game_timer = nullptr;

	QPoint m_dragPosition;
	QPoint m_last_mouse_position;

	int m_top_box_height = 20;
	int m_date_info_y = m_top_box_height + 1; // time info 的 y
	int m_date_info_box_height = 40;
	int m_list_view_y = m_date_info_y + m_date_info_box_height; // list view 的 y

	QFont m_now_year_font = QFont("Serif", 18, QFont::Bold);
	QFont m_now_mouth_day_font = QFont("Serif", 8);

	QPushButton* m_to_do_data_add = nullptr;
	ToDoListView* m_list_view = nullptr;

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
};