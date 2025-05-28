#include "RepeatDataListView.h"
#include "GlobalVariables.h"

#include "Util.h"
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QEvent>
#include <QMouseEvent>

#include <QMessageBox>
#include <QAbstractButton>

class RepeatDataDelegate : public QStyledItemDelegate
{
public:
	RepeatDataDelegate(QObject* parent = nullptr) :
		QStyledItemDelegate(parent)
	{ }
	QSize sizeHint(
		const QStyleOptionViewItem& option, const QModelIndex& index
	) const override
	{
		return QSize(option.rect.width(), 40);
	}
	void paint(
		QPainter* painter,
		const QStyleOptionViewItem& option, const QModelIndex& index
	) const override;
	QWidget* createEditor(
		QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;
	void updateEditorGeometry(
		QWidget* editor, const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(
		QWidget* editor, QAbstractItemModel* model, const QModelIndex& index
	) const override;
	bool editorEvent(
		QEvent* event, QAbstractItemModel* model,
		const QStyleOptionViewItem& option, const QModelIndex& index
	) override;

	void setRepeatBoxWidth(int width)
	{
		m_repeat_box_width = width;
	}
	void setThingLeftPadding(int padding)
	{
		m_thing_left_padding = padding;
	}
	void setDurationBoxWidth(int width)
	{
		m_duration_box_width = width;
	}
	void setDeadlineTimeBoxWidth(int width)
	{
		m_deadline_time_box_width = width;
	}
	void setDeleteBoxWidth(int width)
	{
		m_delete_box_width = width;
	}
	void setDeletePngWidth(int width)
	{
		m_delete_box_width = width;
	}
public:
	QRect getThingBoxRect(QRect option_rect) const
	{
		return 	QRect(
			option_rect.x() + m_thing_left_padding, option_rect.y(),
			option_rect.width() - m_repeat_box_width -
			m_thing_left_padding - m_duration_box_width -
			m_delete_box_width - m_deadline_time_box_width - 
			m_detail_repeat_date_width,
			option_rect.height()
		);
	}
	QRect getRepeatBoxRect(QRect option_rect) const
	{
		return 	QRect(
			option_rect.right() - m_repeat_box_width -
			m_duration_box_width - m_delete_box_width -
			m_deadline_time_box_width - m_detail_repeat_date_width,
			option_rect.top(),
			m_repeat_box_width, option_rect.height()
		);
	}
	QRect getDetailRepeatDateRect(QRect option_rect) const
	{
		return 	QRect(
			option_rect.right() - m_repeat_box_width -
			m_duration_box_width - m_delete_box_width -
			m_deadline_time_box_width ,
			option_rect.top(),
			m_detail_repeat_date_width, option_rect.height()
		);
	}
	QRect getDurationBoxRect(QRect option_rect) const
	{
		return 	QRect(
			option_rect.right() - m_duration_box_width -
			m_delete_box_width - m_deadline_time_box_width,
			option_rect.top(),
			m_duration_box_width, option_rect.height()
		);
	}
	QRect getDeadlineTimeBoxRect(QRect option_rect) const
	{
		return QRect(
			option_rect.right() - m_deadline_time_box_width -
			m_delete_box_width,
			option_rect.top(),
			m_deadline_time_box_width, option_rect.height()
		);
	}
	QRect getDeletePngRect(QRect option_rect) const
	{
		int delete_png_hpadding = (m_delete_box_width - m_delete_png_width) / 2; 
		int delete_png_vpadding = (option_rect.height() - m_delete_png_width) / 2; 
		return QRect(
			option_rect.right() - m_delete_box_width + delete_png_hpadding,
			option_rect.top() + delete_png_vpadding,
			m_delete_png_width, m_delete_png_width
		);
	}
private:
	int m_repeat_box_width = 55;
	int m_detail_repeat_date_width = 60;
	int m_thing_left_padding = 10;
	int m_duration_box_width = 60;
	int m_deadline_time_box_width = 70;
	int m_delete_box_width = 20;
	int m_delete_png_width = 15;
};
void RepeatDataDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
		index.data(Qt::UserRole + 1).value<uintptr_t>()
		);

	painter->save();
	painter->setPen(QColor(100, 100, 100, 100));
	painter->drawLine(
		option.rect.left(), option.rect.top(),
		option.rect.right(), option.rect.top()
	);
	painter->drawLine(
		option.rect.left(), option.rect.bottom(),
		option.rect.right(), option.rect.bottom()
	);
	painter->restore();

	QRect _thing_box_rect = getThingBoxRect(option.rect);
	QRect _repeat_box_rect = getRepeatBoxRect(option.rect);
	QRect _detail_repeat_date_rect = getDetailRepeatDateRect(option.rect);
	QRect _duration_box_rect = getDurationBoxRect(option.rect);
	QRect _deadline_time_box_rect = getDeadlineTimeBoxRect(option.rect);
	QRect _delete_png_rect = getDeletePngRect(option.rect);
	QFontMetrics _metrics(painter->font());
	QString _text_text = _metrics.elidedText(
		_data->task.data(),
		Qt::ElideRight, _thing_box_rect.width()
	);
	painter->save();
	painter->drawText(
		_thing_box_rect, Qt::AlignLeft | Qt::AlignVCenter,
		_text_text
	);
	painter->drawText(
		_repeat_box_rect, Qt::AlignCenter,
		QString::fromStdString(_data->getRepeatTypeString())
	);
	painter->drawText(
		_detail_repeat_date_rect, Qt::AlignCenter,
		QString::fromStdString("第" + std::to_string(_data->detail_repeat_date) + "天")
	);
	int64_t _duration_ms = _data->duration;
	int _duration_day = _duration_ms / 1000 / 60 / 60 / 24;
	painter->drawText(
		_duration_box_rect, Qt::AlignCenter,
		QString::fromStdString(std::to_string(_duration_day) + "天")
	);
	painter->drawText(
		_deadline_time_box_rect, Qt::AlignCenter,
		QTime::fromMSecsSinceStartOfDay(_data->deadline_time).toString("hh:mm:ss")
	);
	painter->drawPixmap(_delete_png_rect, *GlobalVariables::instance()->delete_png);
	painter->restore();
}
QWidget* RepeatDataDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
		index.data(Qt::UserRole + 1).value<uintptr_t>()
		);

	int mouse_x = parent->mapFromGlobal(QCursor::pos()).x();
	int mouse_y = parent->mapFromGlobal(QCursor::pos()).y();

	QRect _thing_box_rect = getThingBoxRect(option.rect);
	if (_thing_box_rect.contains(mouse_x, mouse_y))
	{
		QWidget* _thing_box =
			QStyledItemDelegate::createEditor(parent, option, index);
		_thing_box->setWhatsThis("thing_box");
		return _thing_box;
	}

	QRect _repeat_box_rect = getRepeatBoxRect(option.rect);
	if (_repeat_box_rect.contains(mouse_x, mouse_y))
	{
		QComboBox* _repeat_box = new QComboBox(parent);
		_repeat_box->addItem(
			"每天", static_cast<char>(ToDoRepeatData::RepeatType::EveryDay)
		);
		_repeat_box->addItem(
			"每周", static_cast<char>(ToDoRepeatData::RepeatType::EveryWeek)
		);
		_repeat_box->addItem(
			"每月", static_cast<char>(ToDoRepeatData::RepeatType::EveryMonth)
		);
		_repeat_box->addItem(
			"每年", static_cast<char>(ToDoRepeatData::RepeatType::EveryYear)
		);
		_repeat_box->setWhatsThis("repeat_box");
		return _repeat_box;
	}

	QRect _detail_repeat_date_box_rect = getDetailRepeatDateRect(option.rect);
	if (_detail_repeat_date_box_rect.contains(mouse_x, mouse_y))
	{
		QSpinBox* _detail_repeat_date_box = new QSpinBox(parent);
		_detail_repeat_date_box->setPrefix("第 ");  // 前缀
		_detail_repeat_date_box->setMinimum(1);
		_detail_repeat_date_box->setSuffix(" 天");  // 后缀
		_detail_repeat_date_box->setWhatsThis("detail_repeat_date_box");
		_detail_repeat_date_box->setButtonSymbols(QAbstractSpinBox::NoButtons);

		ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
			index.data(Qt::UserRole + 1).value<uintptr_t>()
			);
		_detail_repeat_date_box->setMaximum(1);
		if (_data->repeat_type == ToDoRepeatData::RepeatType::EveryWeek)
		{
			_detail_repeat_date_box->setMaximum(7);
		}
		return _detail_repeat_date_box;
	}

	QRect _duration_box_rect = getDurationBoxRect(option.rect);
	if (_duration_box_rect.contains(mouse_x, mouse_y))
	{
		QSpinBox* _duration_box = new QSpinBox(parent);
		_duration_box->setMinimum(1);
		_duration_box->setSuffix(" 天");  // 后缀
		_duration_box->setWhatsThis("duration_box");
		_duration_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
		return _duration_box;
	}

	QRect _deadline_time_box_rect = getDeadlineTimeBoxRect(option.rect);
	if (_deadline_time_box_rect.contains(mouse_x, mouse_y))
	{
		NNNTimeEditor* _deadline_time_box = new NNNTimeEditor(parent);
		_deadline_time_box->setWhatsThis("deadline_time_box");
		return _deadline_time_box;
	}

	return nullptr;
}
void RepeatDataDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QString _now_repeat_type = editor->whatsThis();
	if (_now_repeat_type == "thing_box")
	{
		editor->setStyleSheet("border-width:0;border-style:outset;");
		QRect editor_rect = getThingBoxRect(option.rect);
		editor->setGeometry(editor_rect);
	}
	else if (_now_repeat_type == "repeat_box")
	{
		editor->setStyleSheet("border-width:0;");
		QRect _repeat_rect = getRepeatBoxRect(option.rect);
		editor->setGeometry(_repeat_rect);
	}
	else if (_now_repeat_type == "detail_repeat_date_box")
	{
		editor->setStyleSheet("border-width:0;padding-right:0;");
		QRect _duration_rect = getDetailRepeatDateRect(option.rect); 
		editor->setFixedWidth(m_detail_repeat_date_width);
		editor->setGeometry(_duration_rect);
		editor->updateGeometry();
		editor->update();
		editor->parentWidget()->updateGeometry();
	}
	else if (_now_repeat_type == "duration_box")
	{
		editor->setStyleSheet("border-width:0;padding-right:0;");
		//editor->setStyleSheet("QSpinBox::up-button {width: 0;}");
		//editor->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button { width: 0; }");
		QRect _duration_rect = getDurationBoxRect(option.rect);
		editor->setFixedWidth(m_duration_box_width);
		editor->setGeometry(_duration_rect);
		editor->updateGeometry();
		editor->update();
		editor->parentWidget()->updateGeometry();
	}
	else if (_now_repeat_type == "deadline_time_box")
	{
		QRect _deadline_time_rect = getDeadlineTimeBoxRect(option.rect);
		editor->setStyleSheet("border-width:0;padding-right:0;");
		static_cast<NNNTimeEditor*>(editor)->setButtonSymbols(
			QAbstractSpinBox::NoButtons
		); 
		editor->setGeometry(_deadline_time_rect);
		editor->setFixedWidth(m_deadline_time_box_width);
	}
}
void RepeatDataDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QStyledItemDelegate::setEditorData(editor, index);

	QString _now_repeat_type = editor->whatsThis();
	ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
		index.data(Qt::UserRole + 1).value<uintptr_t>()
		);
	if (_now_repeat_type == "thing_box")
	{
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		lineEdit->setText(QString::fromStdString(_data->task));
	}
	else if (_now_repeat_type == "repeat_box")
	{
		QComboBox* _repeat_box = static_cast<QComboBox*>(editor);
		_repeat_box->setCurrentIndex(static_cast<int>(_data->repeat_type));
	}
	else if (_now_repeat_type == "detail_repeat_date_box")
	{
		QSpinBox* _detail_repeat_date_box = static_cast<QSpinBox*>(editor);
		_detail_repeat_date_box->setValue(_data->detail_repeat_date);
	}
	else if (_now_repeat_type == "duration_box")
	{
		QSpinBox* _duration_box = static_cast<QSpinBox*>(editor);
		_duration_box->setValue(_data->duration / 1000 / 60 / 60 / 24);
	}
	else if (_now_repeat_type == "deadline_time_box")
	{
		NNNTimeEditor* _deadline_time_box =
			static_cast<NNNTimeEditor*>(editor);
		_deadline_time_box->setTime(
			QTime::fromMSecsSinceStartOfDay(_data->deadline_time)
		);
	}
}
void RepeatDataDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
		index.data(Qt::UserRole + 1).value<uintptr_t>()
		);

	GlobalVariables* gv = GlobalVariables::instance();
	QString _now_repeat_type = editor->whatsThis();

	if (_now_repeat_type == "thing_box")
	{
		QLineEdit* lineBox = static_cast<QLineEdit*>(editor);
		_data->setTask(lineBox->text().toStdString());
	}
	else if (_now_repeat_type == "repeat_box")
	{
		QComboBox* _repeat_box = static_cast<QComboBox*>(editor);
		_data->setRepeatType(
			_repeat_box->currentData().value<ToDoRepeatData::RepeatType>()
		);
	}
	else if (_now_repeat_type == "detail_repeat_date_box")
	{
		QSpinBox* _detail_repeat_date_box = static_cast<QSpinBox*>(editor);
		_data->setDetailRepeateDate( 
			static_cast<int64_t>(_detail_repeat_date_box->value())
		);
	}
	else if (_now_repeat_type == "duration_box")
	{
		QSpinBox* _duration_box = static_cast<QSpinBox*>(editor);
		_data->setDuration(
			static_cast<int64_t>(_duration_box->value()) * 24 * 60 * 60 * 1000
		);
	}
	else if (_now_repeat_type == "deadline_time_box")
	{
		NNNTimeEditor* _deadline_time_box =
			static_cast<NNNTimeEditor*>(editor);
		_data->setDeadlineTime(
			_deadline_time_box->time().msecsSinceStartOfDay()
		);
	}
}
bool RepeatDataDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (event->type() == QEvent::MouseButtonPress) 
	{
		auto* mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton) 
		{
			// 触发 editor 创建
			emit const_cast<QAbstractItemView*>(qobject_cast<const QAbstractItemView*>(option.widget))
				->edit(index);
			return true;
		}
	}
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}

RepeatDataListView::RepeatDataListView(QWidget* parent) :
	QListView(parent)
{
	connect(
		this, &RepeatDataListView::clicked,
		this, &RepeatDataListView::itemClicked
	);

	RepeatDataDelegate* _delegate = new RepeatDataDelegate(this);
	//_delegate->setDeleteBoxWidth(m_delete_box_width);
	//_delegate->setDeletePngWidth(m_delete_png_width);
	this->setItemDelegate(_delegate);
	//this->setStyleSheet("QListView { background-color: transparent; }");
}

void RepeatDataListView::resizeEvent(QResizeEvent* event)
{
	QListView::resizeEvent(event);
	doItemsLayout(); // 重新计算和布局 item 
	updateGeometry(); // 更新布局 
	update();        // 触发重绘 
}

void RepeatDataListView::setClickedFunc(std::function<void(const QModelIndex&)> func)
{
	m_clicked_func = func;
}

void RepeatDataListView::itemClicked(const QModelIndex& index)
{
	if (m_clicked_func)
	{
		m_clicked_func(index);
	}
	RepeatDataDelegate* _delegate = static_cast<RepeatDataDelegate*>(itemDelegate());

	QRect _item_rect = this->visualRect(index);
	QRect _delete_png_rect =
		_delegate->getDeletePngRect(_item_rect);
	QPoint mouse = this->mapFromGlobal(QCursor::pos());
	bool is_mouse_in_delete_png_rect = _delete_png_rect.contains(mouse);
	if (is_mouse_in_delete_png_rect)
	{
		static bool _is_first = true;
		// 创建消息框并设置父窗口、标题和内容
		static QMessageBox _msg_box(this->parentWidget());
		if (_is_first)
		{
			_is_first = false;
			_msg_box.setWindowTitle("警告");
			_msg_box.setIcon(QMessageBox::Warning);
			_msg_box.setText("删除将无法恢复");
			_msg_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			_msg_box.button(QMessageBox::Ok)->setText("确认");
			_msg_box.button(QMessageBox::Cancel)->setText("取消");
		}
		int _ret = _msg_box.exec();

		if (_ret == QMessageBox::Ok)
		{
			GlobalVariables* gv = GlobalVariables::instance();
			gv->deleteRepeatData(index.row());
			QStandardItemModel* list_view_model =
				static_cast<QStandardItemModel*>(this->model());
			list_view_model->takeRow(index.row());
		}
	}
}