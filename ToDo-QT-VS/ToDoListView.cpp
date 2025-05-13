#include "GlobalVariables.h"
#include "UIColorManager.h"
#include "ToDoListView.h"
#include "ToDoData.h"
#include "Util.h"

#include <QStandardItemModel>
#include <QLineEdit>
#include <QPainter>
#include <QDateTime>
#include <QMessageBox>
#include <QAbstractButton> 

class ToDoDelegate : public QStyledItemDelegate
{
public:
	void paint(
		QPainter* painter,
		const QStyleOptionViewItem& option, const QModelIndex& index
	) const override;
	QSize sizeHint(
		const QStyleOptionViewItem& option, const QModelIndex& index
	) const override
	{
		return QSize(option.rect.width(), 40);
	}
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
		QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index
	) const override;

	void setOkBoxWidth(int width)
	{
		m_ok_box_width = width;
	}
	void setOkRectWidth(int width)
	{
		m_ok_rect_width = width;
	}
	void setTimeBoxWidth(int width)
	{
		m_time_box_width = width;
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
			option_rect.x() + m_ok_box_width, option_rect.y(), 
			option_rect.width() - m_ok_box_width - 
			m_time_box_width - m_delete_box_width, 
			option_rect.height()
		);
	}
	QRect getOkRectRect(QRect option_rect) const
	{
		int ok_rect_offset = (m_ok_box_width - m_ok_rect_width) / 2;
		return 	QRect(
			option_rect.x() + ok_rect_offset,
			option_rect.y() + ok_rect_offset,
			m_ok_rect_width, m_ok_rect_width
		);
	}
	QRect getTimeBoxRect(QRect option_rect) const
	{
		return QRect(
			option_rect.width() - m_time_box_width - m_delete_box_width + 5,
			option_rect.y(),
			m_time_box_width - 5, option_rect.height()
		);
	}
	QRect getDeletePngRect(QRect option_rect) const
	{ 
		int delete_png_hpadding = (m_delete_box_width - m_delete_png_width) / 2;
		int delete_png_vpadding = (option_rect.height() - m_delete_png_width) / 2;
		return QRect(
			option_rect.width() - m_delete_box_width + delete_png_hpadding,
			option_rect.y() + delete_png_vpadding,
			m_delete_png_width, m_delete_png_width
		);
	}
private:
	int m_ok_box_width = 40;
	int m_ok_rect_width = 14; //复选框的宽度
	int m_time_box_width = 75;
	int m_delete_box_width = 20;
	int m_delete_png_width = 15;
};


void ToDoDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();

	QVariant var = index.data(Qt::UserRole + 1);
	ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());
	QColor _temp_pen_color;
	QColor _temp_brush_color;

	UIColorManager* _ui_color_manager = UIColorManager::instance();

	/// background draw
	{
		//_temp_pen_color = QColor(100, 100, 100, 80);
		_temp_pen_color = _ui_color_manager->getColor("main_task_border");
		bool is_item_out = 
			itemData->deadline_date + itemData->deadline_time <
			QDateTime::currentDateTime().toMSecsSinceEpoch();
		if (is_item_out && !itemData->is_finished)
		{
			//_temp_pen_color = QColor(255, 0, 0, 80);
			_temp_pen_color = 
				_ui_color_manager->getColor("main_task_border_out");
		}
		painter->setPen(_temp_pen_color); 
		painter->drawLine(option.rect.topLeft(), option.rect.topRight());
		painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

		if (option.state.testFlag(QStyle::State_MouseOver))
		{
			//_temp_brush_color = QColor(100, 100, 100, 100);
			_temp_brush_color = 
				_ui_color_manager->getColor("main_task_bg_hover"); 
			if (is_item_out && !itemData->is_finished)
			{
				//_temp_brush_color = QColor(200, 0, 0, 100);
				_temp_brush_color =
					_ui_color_manager->getColor("main_task_bg_out_hover");
			}
			painter->setBrush(_temp_brush_color);
		}
		else
		{
			//_temp_brush_color = QColor(0, 0, 0, 0); 
			_temp_brush_color = _ui_color_manager->getColor("main_task_bg");
			if (is_item_out && !itemData->is_finished)
			{
				//_temp_brush_color = QColor(100, 0, 0, 100);
				_temp_brush_color =
					_ui_color_manager->getColor("main_task_bg_out");
			}
			painter->setBrush(_temp_brush_color);
		}
		painter->drawRect(option.rect);
	}


	/// is ok draw
	{
		QRect ok_rect = getOkRectRect(option.rect);
		_temp_pen_color = _ui_color_manager->getColor("checkbox_border");
		if (itemData->is_finished)
		{
			_temp_brush_color = _ui_color_manager->getColor("checkbox_bg_ok");
		}
		else
		{
			_temp_brush_color = _ui_color_manager->getColor("ckeckbox_bg");
		}
		painter->setPen(_temp_pen_color);
		painter->setBrush(_temp_brush_color);
		painter->drawRoundedRect(ok_rect, 2, 2);

		if (itemData->is_finished)
		{
			_temp_pen_color = _ui_color_manager->getColor("checkbox_check");
			painter->setPen(_temp_pen_color);
			painter->drawText(
				option.rect.x() + 10, option.rect.y() + 10,
				20, 20, Qt::AlignCenter, "✔"
			);
		}
	}
	/// text draw
	{
		QRect text_rect = getThingBoxRect(option.rect);

		if (itemData->is_finished)
		{
			QFont font = painter->font();
			font.setStrikeOut(true);  // 启用删除线
			painter->setFont(font);
		}

		QFontMetrics metrics(painter->font());
		QString text_text = metrics.elidedText(
			itemData->thing.data(),
			Qt::ElideRight, text_rect.width()
		);
		_temp_pen_color = _ui_color_manager->getColor("main_task_task");
		painter->setPen(_temp_pen_color);
		painter->drawText(
			text_rect,
			Qt::AlignLeft | Qt::AlignVCenter | Qt::ElideRight,
			text_text
		);
	}
	// time draw
	{
		QRect time_rect = getTimeBoxRect(option.rect);
		_temp_pen_color = _ui_color_manager->getColor("main_task_date");
		painter->setPen(_temp_pen_color);
		QString time_text = 
			QDateTime::fromMSecsSinceEpoch(itemData->deadline_date)
			.toString("yyyy/MM/dd");

		painter->drawText(
			time_rect,
			Qt::AlignLeft | Qt::AlignVCenter | Qt::ElideRight,
			time_text
		);
	}
	QRect delete_png_rect = getDeletePngRect(option.rect);
	painter->drawPixmap(delete_png_rect, *GlobalVariables::instance()->delete_png);

	painter->restore();
	//QStyledItemDelegate::paint(painter, option, index);
}

QWidget* ToDoDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QVariant var = index.data(Qt::UserRole + 1);
	ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());
	if (itemData->is_finished) { return nullptr; }
	int mouse_x = parent->mapFromGlobal(QCursor::pos()).x(); 
	int mouse_y = parent->mapFromGlobal(QCursor::pos()).y(); 
	QRect valid_rect(
		option.rect.x() + m_ok_box_width, option.rect.y(),
		option.rect.width() - m_ok_box_width - m_time_box_width, 
		option.rect.height()
	);
	bool is_mouse_in_valid = valid_rect.contains(mouse_x, mouse_y);
	if (!is_mouse_in_valid) { return nullptr; }

	return QStyledItemDelegate::createEditor(parent, option, index);
}

void ToDoDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setStyleSheet("border-width:0;border-style:outset;");
	QRect editor_rect(
		option.rect.x() + m_ok_box_width, option.rect.y() + 1,
		option.rect.width() - m_ok_box_width - m_time_box_width, 
		option.rect.height() - 2
	);
	editor->setGeometry(editor_rect);
}

void ToDoDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QStyledItemDelegate::setEditorData(editor, index);
	QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
	QVariant var = index.data(Qt::UserRole + 1);
	ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());

	lineEdit->setText(itemData->thing.data());
}

void ToDoDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QVariant var = index.data(Qt::UserRole + 1);
	ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());

	QLineEdit* lineBox = static_cast<QLineEdit*>(editor);
	QString str = lineBox->text();

	itemData->thing = str.toStdString();
}



ToDoListView::ToDoListView(QWidget* parent) :
	QListView(parent)
{
	connect(this, &ToDoListView::clicked, this, &ToDoListView::itemClicked);
	connect(this, &ToDoListView::doubleClicked, this, &ToDoListView::itemDoubleClicked);

	m_delegate = new ToDoDelegate;
	ToDoDelegate* _delegate = static_cast<ToDoDelegate*>(m_delegate);
	this->setItemDelegate(_delegate);

	this->setStyleSheet("QListView { background-color: transparent; }");
	this->setCursor(Qt::ArrowCursor);
}

void ToDoListView::resizeEvent(QResizeEvent* event)
{
	QListView::resizeEvent(event);
	doItemsLayout(); // 重新计算和布局 item
	updateGeometry(); // 更新布局
	update();        // 触发重绘
}

static void addItemToModel(QStandardItem* item, ToDoData* item_data, QStandardItemModel* model)
{
	int _idx = 0;
	while (_idx < model->rowCount())
	{
		QStandardItem* item = model->item(_idx);
		if (item)
		{
			QVariant var = item->data(Qt::UserRole + 1);
			ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());
			if (*item_data < *itemData)
			{
				break;
			}
		}
		_idx++;
	}
	model->insertRow(_idx, item);
}

void ToDoListView::itemClicked(const QModelIndex& index)
{
	//QStandardItemModel* model = static_cast<QStandardItemModel*>(this->model());
	//QStandardItem* item = model->itemFromIndex(index);
	//item->data(Qt::UserRole + 1); 
	QVariant _var = index.data(Qt::UserRole + 1);
		
	ToDoData* itemData = reinterpret_cast<ToDoData*>(_var.value<std::uintptr_t>());

	QRect _item_rect = this->visualRect(index);
	QRect _ok_rect = 
		static_cast<ToDoDelegate*>(m_delegate)->getOkRectRect(_item_rect);
	QRect _delete_png_rect =
		static_cast<ToDoDelegate*>(m_delegate)->getDeletePngRect(_item_rect);

	int _mouse_x = this->mapFromGlobal(QCursor::pos()).x();
	int _mouse_y = this->mapFromGlobal(QCursor::pos()).y();
	
	if(_ok_rect.contains(_mouse_x, _mouse_y))
	{
		itemData->is_finished = !itemData->is_finished;
		if (itemData->is_finished)
		{
			itemData->finished_date_time = 
				QDateTime::currentDateTime().toMSecsSinceEpoch();
		}
		else
		{
			itemData->finished_date_time = 0;
		}
		// 获取模型并发射 dataChanged 信号
		QAbstractItemModel* model = this->model();
		if (model) {
			emit model->dataChanged(index, index);
		}
		QStandardItemModel* _list_view_model = 
			static_cast<QStandardItemModel*>(this->model());
		QStandardItem* _item = _list_view_model->itemFromIndex(index);
		_list_view_model->takeRow(index.row());

		GlobalVariables* gv = GlobalVariables::instance();
		std::ofstream _ofs;
		_ofs.open(
			gv->getInfoFilePath(),
			std::ios::binary | std::ios::out | std::ios::in
		);

		_ofs.seekp(itemData->info_ptr + itemData->is_finished_offset);
		_ofs.write((char*)(&itemData->is_finished), 1);
		_ofs.seekp(itemData->info_ptr + itemData->finished_date_time_offset);
		_ofs.write((char*)(&itemData->finished_date_time), 8);

		_ofs.close();
		addItemToModel(_item, itemData, _list_view_model);
	}
	else if (_delete_png_rect.contains(_mouse_x, _mouse_y))
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

			itemData->deleteThis(
				GlobalVariables::instance()->getDataFilePath()
			);
			QStandardItemModel* list_view_model =
				static_cast<QStandardItemModel*>(this->model());
			list_view_model->takeRow(index.row()); 
		}
	}
}
