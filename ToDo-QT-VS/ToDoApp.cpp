#include "ToDoApp.h"

#include <QMouseEvent>
#include <iostream>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTimer>

#include <QDate>

ToDoApp::ToDoApp(QWidget* parent) :
	QWidget(parent)
{ 
	this->setWindowFlag(Qt::WindowStaysOnTopHint);
	this->setWindowFlag(Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setMouseTracking(true);
	m_rubber_band = new QRubberBand(QRubberBand::Rectangle);
	this->setMinimumSize(QSize(302, 186));
	//this->setObjectName("todowidget");
	//this->setStyleSheet("QWidget #todowidget { border-radius: 10px;background: white;}");

	// 创建 QListView 对象
	{
		m_list_view = new ToDoListView(this);
		m_list_view->setGeometry(
			1, m_list_view_y, 
			this->width() - 2, this->height() - m_list_view_y - 1
		);

		QStandardItemModel* model = new QStandardItemModel();

		m_list_view->setModel(model);

		//for (int i = 0; i < 20; ++i)
		//{
		//	QStandardItem* _item = new QStandardItem();
		//	ToDoData* _data = new ToDoData();
		//	_data->is_finished = false;
		//	if (!i) { _data->is_finished = true; }
		//	_data->thing = "thing | " + std::to_string(i);
		//	_item->setData(reinterpret_cast<std::uintptr_t>(_data), Qt::UserRole + 1);
		//	model->appendRow(_item);
		//}
		for (size_t i = 0; i < m_to_do_data_list.size(); ++i)
		{
			QStandardItem* _item = new QStandardItem();
			_item->setData(reinterpret_cast<std::uintptr_t>(m_to_do_data_list[i]), Qt::UserRole + 1);
			model->appendRow(_item);
		}
		
		// 设置选择模式为单选  
		m_list_view->setSelectionMode(QAbstractItemView::SingleSelection);
		//QObject::connect(m_list_view, &QListView::clicked, 
		//	[](const QModelIndex& index) {
		//	//qDebug() << "Item clicked:" << index.data().toString();
		//	}
		//);

		// 设置 item 可拖拽
		//m_list_view->setDragEnabled(true);
		//m_list_view->setAcceptDrops(true);
		//m_list_view->setDropIndicatorShown(true);
		//m_list_view->setDefaultDropAction(Qt::MoveAction);

		// 显示 QListView 控件
		m_list_view->show();
	}

	{
		m_to_do_data_add = new QPushButton("+", this);
		m_to_do_data_add->setGeometry(
			this->width() - 30, m_list_view_y - 25, 20, 20
		);

		connect(m_to_do_data_add, &QPushButton::clicked, 
			[this]() {
				QStandardItem* _item = new QStandardItem();
				ToDoData* _data = new ToDoData();
				_data->is_finished = false;
				_data->thing = "thing | ";
				_item->setData(reinterpret_cast<std::uintptr_t>(_data), Qt::UserRole + 1);
				static_cast<QStandardItemModel*>(m_list_view->model())->appendRow(_item);
			}
		);

		m_to_do_data_add->show();
	}

	/// 设置每一秒刷新一次
	{
		std::chrono::milliseconds frame_duration(1000);
		m_game_timer = new QTimer(this);
		connect(m_game_timer, &QTimer::timeout, this, &ToDoApp::updataData);
		m_game_timer->start(frame_duration);
	}
}

ToDoApp::~ToDoApp()
{ }

void ToDoApp::mousePressEvent(QMouseEvent* ev)
{
	if (ev->buttons() & Qt::MouseButton::LeftButton) 
	{
		m_dragPosition = ev->globalPosition().toPoint() - frameGeometry().topLeft();
		ev->accept();
	}

	m_rubber_band_left_top = QPoint(geometry().x(), geometry().y());
	m_rubber_band_right_bottom = m_rubber_band_left_top + 
		QPoint(geometry().width(), geometry().height());
	if (ev->buttons() & Qt::MouseButton::LeftButton) 
	{
		if (m_mouse_hit_test != MouseHitTest::Normal && 
			m_mouse_hit_test != MouseHitTest::Move)
		{
			m_rubber_band->setGeometry(
				QRect(m_rubber_band_left_top, m_rubber_band_right_bottom).normalized()
			);
			m_rubber_band->show();
			m_is_rubber_band_show = true;
		}
	}
}

void ToDoApp::mouseMoveEvent(QMouseEvent * ev)
{
	mouseHitTest(ev);
	if (!(ev->buttons() & Qt::MouseButton::LeftButton)) 
	{ return; }

	QSize min_rect = this->minimumSize() + QSize(2, 2);
	int top_new_position = m_rubber_band_left_top.y() + ev->pos().y();
	int left_new_position = m_rubber_band_left_top.x() + ev->pos().x();
	int bottom_new_position = 
		m_rubber_band_right_bottom.y() + ev->pos().y() - this->rect().height();
	int right_new_position = 
		m_rubber_band_right_bottom.x() + ev->pos().x() - this->rect().width();
	if (m_rubber_band_right_bottom.y() - top_new_position < min_rect.height())
	{
		top_new_position = m_rubber_band_right_bottom.y() - min_rect.height();
	}
	if (bottom_new_position - m_rubber_band_left_top.y() < min_rect.height())
	{
		bottom_new_position = m_rubber_band_left_top.y() + min_rect.height();
	}
	if (m_rubber_band_right_bottom.x() - left_new_position < min_rect.width())
	{
		left_new_position = m_rubber_band_right_bottom.x() - min_rect.width();
	}
	if (right_new_position - m_rubber_band_left_top.x() < min_rect.width())
	{
		right_new_position = m_rubber_band_left_top.x() + min_rect.width();
	}

	switch (m_mouse_hit_test)
	{
	case ToDoApp::MouseHitTest::Top:
	{
		m_rubber_band->setGeometry(
			QRect(
				QPoint(
					m_rubber_band_left_top.x(), 
					top_new_position
				),
				m_rubber_band_right_bottom
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::Left:
	{
		m_rubber_band->setGeometry(
			QRect(
				QPoint(
					left_new_position,
					m_rubber_band_left_top.y()
				),
				m_rubber_band_right_bottom
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::Bottom:
	{
		m_rubber_band->setGeometry(
			QRect(
				m_rubber_band_left_top,
				QPoint(
					m_rubber_band_right_bottom.x(), 
					bottom_new_position
				)
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::Right:
	{
		m_rubber_band->setGeometry(
			QRect(
				m_rubber_band_left_top,
				QPoint(
					right_new_position,
					m_rubber_band_right_bottom.y()
				)
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::LeftTop:
	{
		m_rubber_band->setGeometry(
			QRect(
				QPoint(
					left_new_position,
					top_new_position
				),
				m_rubber_band_right_bottom
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::LeftBottom:
	{
		m_rubber_band->setGeometry(
			QRect(
				QPoint(
					left_new_position,
					m_rubber_band_left_top.y()
				),
				QPoint(
					m_rubber_band_right_bottom.x(),
					bottom_new_position
				)
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::RightBottom:
	{
		m_rubber_band->setGeometry(
			QRect(
				m_rubber_band_left_top,
				QPoint(
					right_new_position,
					bottom_new_position
				)
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::RightTop:
	{
		m_rubber_band->setGeometry(
			QRect(
				QPoint(
					m_rubber_band_left_top.x(),
					top_new_position
				),
				QPoint(
					right_new_position,
					m_rubber_band_right_bottom.y()
				)
			)
		);
		break;
	}
	case ToDoApp::MouseHitTest::Move:
	{
		move(ev->globalPosition().toPoint() - m_dragPosition);
		ev->accept();
		break;
	}
	}
}

void ToDoApp::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) 
	{
		//m_dragPosition = QPoint();
		ev->accept();
		if (m_is_rubber_band_show)
		{
			m_is_rubber_band_show = false;
			m_rubber_band->hide();

			this->setGeometry(m_rubber_band->geometry());
		}

		if (m_mouse_hit_test == MouseHitTest::Move)
		{
			setCursor(Qt::ArrowCursor);
			m_mouse_hit_test = MouseHitTest::Normal;
		}
	}
}

void ToDoApp::resizeEvent(QResizeEvent* event)
{
	QSize app_size = event->size();

	QRect to_do_data_add = m_to_do_data_add->geometry();
	m_to_do_data_add->setGeometry(
		app_size.width() - to_do_data_add.width() - 10, to_do_data_add.y(),
		to_do_data_add.width(), to_do_data_add.height()
	);

	QRect list_view_rect = m_list_view->geometry(); 
	m_list_view->setGeometry(
		list_view_rect.x(), list_view_rect.y(), 
		app_size.width() - 2, app_size.height() - list_view_rect.y() - 1
	);
}

void ToDoApp::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	// 设置抗锯齿
	painter.setRenderHint(QPainter::Antialiasing);

	// 设置背景颜色
	QColor appColor(100, 100, 100, 100); // 白色背景
	painter.setBrush(appColor);
	painter.setPen(appColor);

	// 设置圆角矩形
	QRect rect = this->rect();
	int radius = 5; // 圆角半径
	painter.drawRoundedRect(rect, radius, radius);

	painter.save();
	
	painter.setPen(QColor(255, 255, 255, 170));
	painter.drawText(
		1, 0, this->width() - 2, m_top_box_height,
		Qt::AlignCenter, QTime::currentTime().toString("hh:mm:ss")
	);

	painter.setPen(QColor(255, 255, 255, 150));
	painter.setFont(m_now_year_font);
	painter.drawText(
		10, m_date_info_y, 
		this->width(), m_date_info_box_height / 2,
		Qt::AlignLeft | Qt::AlignVCenter,
		QDate::currentDate().toString("yyyy")
	);

	painter.setPen(QColor(255, 255, 255, 170));
	painter.setFont(m_now_mouth_day_font);
	painter.drawText(
		15, m_date_info_y + m_date_info_box_height / 2,
		this->width(), m_date_info_box_height / 2,
		Qt::AlignLeft | Qt::AlignVCenter,
		QDate::currentDate().toString("MM 月 dd 日 dddd")
	);

	painter.restore();
}

void ToDoApp::mouseHitTest(QMouseEvent* ev)
{
	if (m_is_rubber_band_show) { return; }
	if (m_mouse_hit_test == MouseHitTest::Move) { return; }
	enum RegionMask {
		client = 0b0000,
		left = 0b0001,
		right = 0b0010,
		top = 0b0100,
		bottom = 0b1000,
	};
	QPoint pos = ev->pos();
	QRect rect = this->rect();
	const auto result =
		left * (pos.x() <= 5) |
		right * (pos.x() >= (rect.width() - 5)) |
		top * (pos.y() <= 5) |
		bottom * (pos.y() >= (rect.height() - 5));

	switch (result)
	{
	case client:
	{
		if (ev->buttons() & Qt::MouseButton::LeftButton)
		{
			setCursor(Qt::SizeAllCursor);
			m_mouse_hit_test = MouseHitTest::Move;
		}
		else 
		{
			setCursor(Qt::ArrowCursor);
			m_mouse_hit_test = MouseHitTest::Normal;
		}
		break;
	}
	case left:
	{
		setCursor(Qt::SizeHorCursor);
		m_mouse_hit_test = MouseHitTest::Left;
		break;
	}
	case right:
	{
		setCursor(Qt::SizeHorCursor);
		m_mouse_hit_test = MouseHitTest::Right;
		break;
	}
	case top:
	{
		setCursor(Qt::SizeVerCursor);
		m_mouse_hit_test = MouseHitTest::Top;
		break;
	}
	case bottom:
	{
		setCursor(Qt::SizeVerCursor);
		m_mouse_hit_test = MouseHitTest::Bottom;
		break;
	}
	case top | left:
	{
		setCursor(Qt::SizeFDiagCursor);
		m_mouse_hit_test = MouseHitTest::LeftTop;
		break;
	}
	case bottom | right:
	{
		setCursor(Qt::SizeFDiagCursor);
		m_mouse_hit_test = MouseHitTest::RightBottom;
		break;
	}
	case top | right:
	{
		setCursor(Qt::SizeBDiagCursor);
		m_mouse_hit_test = MouseHitTest::RightTop;
		break;
	}
	case bottom | left:
	{
		setCursor(Qt::SizeBDiagCursor); 
		m_mouse_hit_test = MouseHitTest::LeftBottom;
		break;
	}
	}
}

void ToDoApp::updataData()
{
	using namespace std::chrono;

	static std::chrono::steady_clock::time_point m_last_tick; 
	// 取得当前时间 
	steady_clock::time_point frameStart = steady_clock::now();
	duration<float> delta = duration<float>(frameStart - m_last_tick);

	this->update();
	m_last_tick = frameStart;
}
