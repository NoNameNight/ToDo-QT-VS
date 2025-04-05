#include "ToDoApp.h"

#include <QMouseEvent>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardItem>

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QCalendarWidget>
#include <QDateTimeEdit>

#include <QTimer>
#include <QDate>

class NNNDateTimeEditor : public QDateTimeEdit
{
public:
	NNNDateTimeEditor(QWidget* parent = nullptr) :
		QDateTimeEdit(parent)
	{
		this->setDate(QDate::currentDate());
		this->setDisplayFormat("yyyy/MM/dd");
		this->setMinimumDate(QDate::currentDate());
		this->setCalendarPopup(true);
	}
};

ToDoApp::ToDoApp(QWidget* parent) :
	QWidget(parent)
{ 
	this->setWindowFlag(Qt::FramelessWindowHint);
	this->setWindowFlag(Qt::Tool);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setMouseTracking(true);
	m_rubber_band = new QRubberBand(QRubberBand::Rectangle);
	this->setMinimumSize(QSize(302, 186));
	//this->setObjectName("todowidget");
	//this->setStyleSheet("QWidget #todowidget { border-radius: 10px;background: white;}");

	if (Config::instance()->isTopMost())
	{ this->setWindowFlag(Qt::WindowStaysOnTopHint); }
	if(Config::instance()->isMouseTransparent())
	{ this->setWindowFlag(Qt::WindowTransparentForInput); }
	this->setGeometry(
		Config::instance()->windowX(), Config::instance()->windowY(),
		Config::instance()->windowWidth(), Config::instance()->windowHeight()
	);

	this->initToDoDataAddDialog();

	//m_setting_widget->show(); 

	// 创建 QListView 对象
	{
		m_list_view = new ToDoListView(this);
		m_list_view->setGeometry(
			1, m_list_view_y, 
			this->width() - 2, this->height() - m_list_view_y - 1
		);

		QStandardItemModel* model = new QStandardItemModel();

		m_list_view->setModel(model);

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
		m_to_do_data_add->setStyleSheet("QPushButton { background-color: transparent; }");
		connect(
			m_to_do_data_add, &QPushButton::clicked,
			this, &ToDoApp::showToDoDataAddDialog
		);
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

void ToDoApp::changeTopMost()
{
	bool _is_top_most = this->windowFlags() & Qt::WindowStaysOnTopHint;
	Config::instance()->setIsTopMose(!_is_top_most); 
	this->setWindowFlag(Qt::WindowStaysOnTopHint, !_is_top_most);
	this->show(); 
}

void ToDoApp::changeMouseTransparent()
{
	bool _is_mouse_transparent = 
		this->windowFlags() & Qt::WindowTransparentForInput; 
	Config::instance()->setIsMouseTransparent(!_is_mouse_transparent);
	this->setWindowFlag(Qt::WindowTransparentForInput, !_is_mouse_transparent);
	this->show();
}

void ToDoApp::addToDoListItem(int64_t info_ptr, int64_t text_ptr, std::string thing, bool is_finished, int64_t create_time, ToDoData::DeadlineType deadline_type, int64_t deadline_date, int64_t deadline_time, int64_t finished_time)
{
	QStandardItem* _item = new QStandardItem();
	ToDoData* _data = new ToDoData();
	_data->text_ptr = text_ptr;
	_data->info_ptr = info_ptr;
	_data->thing = thing;
	_data->is_finished = is_finished;
	_data->create_time = create_time;
	_data->deadline_type = deadline_type;
	_data->deadline_date = deadline_date;
	_data->deadline_time = deadline_time;
	_data->finished_time = finished_time;
	_item->setData(reinterpret_cast<std::uintptr_t>(_data), Qt::UserRole + 1);

	QStandardItemModel* list_view_model =
		static_cast<QStandardItemModel*>(m_list_view->model());
	int _idx = 0;
	while (_idx < list_view_model->rowCount())
	{
		QStandardItem* item = list_view_model->item(_idx);
		if (item)
		{
			QVariant var = item->data(Qt::UserRole + 1);
			ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());
			if (*_data < *itemData)
			{
				break;
			}
		}
		_idx++;
	}
	list_view_model->insertRow(_idx, _item);
}

void ToDoApp::addToDoListItem(std::string thing, bool is_finished, ToDoData::DeadlineType deadline_type, int64_t deadline_date, int64_t deadline_time, int64_t finished_time)
{
	int64_t create_time = QDateTime::currentDateTime().toMSecsSinceEpoch();

	GlobalVariables* gv = GlobalVariables::instance();
	uint64_t text_ptr = 0;
	uint64_t info_ptr = 0;

	{
		std::ofstream text_ofs;
		text_ofs.open(gv->getTextFilePath(), std::ios::binary | std::ios::app);
		text_ofs.seekp(0, std::ios::end);
		text_ptr = text_ofs.tellp();
		size_t text_size = thing.size();
		char text_size_char[8] = { };
		memcpy(text_size_char, &text_size, 8);
		text_ofs.write(text_size_char, 8);
		text_ofs.write(thing.data(), thing.size());
		text_ofs.close();
	}
	{
		std::ofstream info_ofs;
		info_ofs.open(gv->getInfoFilePath(), std::ios::binary | std::ios::app);
		info_ofs.seekp(0, std::ios::end);
		info_ptr = info_ofs.tellp();
		char _info[128] = { };
		memcpy(_info + ToDoData::text_ptr_offset, &text_ptr, 8);
		memcpy(_info + ToDoData::is_finished_offset, &is_finished, 1);
		memcpy(_info + ToDoData::create_time_offset, &create_time, 8);
		char deadline_type_char = static_cast<char>(deadline_type);
		memcpy(_info + ToDoData::deadline_type_offset, &deadline_type_char, 1);
		memcpy(_info + ToDoData::deadline_date_offset, &deadline_date, 8);
		memcpy(_info + ToDoData::deadline_time_offset, &deadline_time, 8);
		memcpy(_info + ToDoData::finished_time_offset, &finished_time, 8);
		info_ofs.write(_info, 128);
		info_ofs.close();
	}
	{
		std::ofstream data_ofs;
		data_ofs.open(gv->getDataFilePath(), std::ios::binary | std::ios::app);
		char _data[64] = { };
		memcpy(_data, &info_ptr, 8);
		data_ofs.write(_data, 64);
		data_ofs.close();
	}

	addToDoListItem(
		info_ptr, text_ptr, 
		thing, is_finished, 
		create_time, deadline_type, 
		deadline_date, deadline_time, finished_time
	);

	//QStandardItem* _item = new QStandardItem();
	//ToDoData* _data = new ToDoData();
	//_data->text_ptr = text_ptr;
	//_data->info_ptr = info_ptr;
	//_data->thing = thing;
	//_data->is_finished = is_finished;
	//_data->create_time = create_time;
	//_data->deadline_type = deadline_type;
	//_data->deadline_date = deadline_date;
	//_data->deadline_time = deadline_time;
	//_data->finished_time = finished_time;
	//_item->setData(reinterpret_cast<std::uintptr_t>(_data), Qt::UserRole + 1);


	//QStandardItemModel* list_view_model = 
	//	static_cast<QStandardItemModel*>(m_list_view->model());
	//int _idx = 0; 
	//while (_idx < list_view_model->rowCount())
	//{
	//	QStandardItem* item = list_view_model->item(_idx);
	//	if (item)
	//	{
	//		QVariant var = item->data(Qt::UserRole + 1);
	//		ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());
	//		if (*_data < *itemData)
	//		{
	//			break;
	//		}
	//	}
	//	_idx++;
	//}
	//list_view_model->insertRow(_idx, _item);
}

void ToDoApp::mousePressEvent(QMouseEvent* ev)
{
	if (ev->buttons() & Qt::MouseButton::LeftButton) 
	{
		m_drag_position = ev->globalPosition().toPoint() - frameGeometry().topLeft();
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

		if (m_mouse_hit_test == MouseHitTest::Normal)
		{
			QRect _setting_png_rect(
				this->width() - m_setting_png_width - m_setting_png_padding, 5,
				m_setting_png_width, m_setting_png_width
			);
			QPoint _mouse = this->mapFromGlobal(QCursor::pos());
			if (_setting_png_rect.contains(_mouse))
			{
				this->showSettingWidget();
			}
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
		QPoint _now_pos = ev->globalPosition().toPoint() - m_drag_position;
		this->move(_now_pos);
		Config::instance()->setWindowX(_now_pos.x());
		Config::instance()->setWindowY(_now_pos.y());
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
	Config::instance()->setWindowWidth(app_size.width());
	Config::instance()->setWindowHeight(app_size.height());

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

	painter.save();
	QRect _setting_png_rect(
		this->width() - m_setting_png_width - m_setting_png_padding, 5,
		m_setting_png_width, m_setting_png_width
	);
	painter.drawPixmap(
		_setting_png_rect, *GlobalVariables::instance()->setting_png
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

	GlobalVariables* gv = GlobalVariables::instance();
	for (ToDoRepeatData* _data : gv->repeat_data_list)
	{
		int64_t now_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
		if (_data->isNeedRepeat(now_time))
		{
			_data->setLastAddTime(gv->getRepeatInfoFilePath(), now_time);
			int64_t deadline_time = QDateTime(QDate::currentDate(), QTime(0, 0, 0))
				.addMSecs(_data->duration).addDays(-1).toMSecsSinceEpoch();
			this->addToDoListItem(
				_data->thing, false, ToDoData::DeadlineType::Day, deadline_time, 0, 0
			);
		}
	}

	this->update();
	m_last_tick = frameStart;
}

void ToDoApp::initToDoDataAddDialog()
{
	m_to_do_data_add_dialog = new QDialog(this);
	m_to_do_data_add_dialog->setWindowTitle("Add");

	QFormLayout* form = new QFormLayout(m_to_do_data_add_dialog);
	form->setLabelAlignment(Qt::AlignRight);

	// thing
	QString thing = QString("thing: ");
	QLineEdit* thing_box = new QLineEdit(m_to_do_data_add_dialog);
	form->addRow(thing, thing_box);
	//m_to_do_data_add_dialog->show();
	// deadline
	QString deadline = QString("deadline: ");
	QComboBox* deadline_box = new QComboBox(m_to_do_data_add_dialog);
	deadline_box->addItem("今天");
	deadline_box->addItem("明天");
	deadline_box->addItem("自定义");
	form->addRow(deadline, deadline_box);
	// detail deadline
	QString detail_deadline = QString("detail deadline: ");
	NNNDateTimeEditor* detail_deadline_box = 
		new NNNDateTimeEditor(m_to_do_data_add_dialog);
	detail_deadline_box->setEnabled(false);
	form->addRow(detail_deadline, detail_deadline_box);

	connect(deadline_box, &QComboBox::currentIndexChanged, this,
		[this, deadline_box, detail_deadline_box]() {
			if (deadline_box->currentText() == "今天")
			{
				detail_deadline_box->setDate(QDate::currentDate());
				detail_deadline_box->setEnabled(false);
			}
			if (deadline_box->currentText() == "明天")
			{
				detail_deadline_box->setDate(QDate::currentDate().addDays(1));
				detail_deadline_box->setEnabled(false);
			}
			if (deadline_box->currentText() == "自定义")
			{
				detail_deadline_box->setEnabled(true);
			}
		}
	);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, this
	);
	QObject::connect(buttonBox, &QDialogButtonBox::accepted,
		m_to_do_data_add_dialog,
		[this, thing_box, deadline_box, detail_deadline_box]() {
			this->addToDoListItem(
				thing_box->text().toStdString(), false,
				ToDoData::DeadlineType::Day, 
				QDateTime(
					detail_deadline_box->date(), QTime(0, 0, 0)
				).toMSecsSinceEpoch(),
				0, 0
			);

			m_to_do_data_add_dialog->hide();
			thing_box->setText("");
			deadline_box->setCurrentText("今天");
			detail_deadline_box->setDate(QDate::currentDate()); 
			detail_deadline_box->setEnabled(false); 
		}
	);
	QObject::connect(
		buttonBox, &QDialogButtonBox::rejected,
		m_to_do_data_add_dialog,
		[this]() {
			m_to_do_data_add_dialog->hide();
		}
	);

	form->addRow(buttonBox);
}