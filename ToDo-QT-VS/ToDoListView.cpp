#include "ToDoListView.h"
#include "Util.h"

#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QPainter>
#include <QDateTime>

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
private:
	int ok_container = 40;
	int time_container = 75;
};


void ToDoDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();
	//qDebug() << option.rect;

	QVariant var = index.data(Qt::UserRole + 1);
	ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());

	/// background draw
	{
		painter->setPen(QColor(100, 100, 100, 80));
		bool is_item_out = 
			itemData->deadline_date + itemData->deadline_time <
			QDateTime::currentDateTime().toMSecsSinceEpoch();
		if (is_item_out && !itemData->is_finished)
		{
			painter->setPen(QColor(255, 0, 0, 80));
		}
		painter->drawLine(option.rect.topLeft(), option.rect.topRight());
		painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

		if (option.state.testFlag(QStyle::State_MouseOver))
		{
			painter->setBrush(QColor(100, 100, 100, 100));
			if (is_item_out && !itemData->is_finished)
			{
				painter->setBrush(QColor(200, 0, 0, 100));
			}
		}
		else
		{
			painter->setBrush(QColor(0, 0, 0, 0));
			if (is_item_out && !itemData->is_finished)
			{
				painter->setBrush(QColor(100, 0, 0, 100));
			}
		}
		painter->drawRect(option.rect);
	}


	/// is ok draw
	{
		int ok_rect_width = 14;
		int ok_rect_offset = (ok_container - ok_rect_width) / 2;
		QRect ok_rect(
			option.rect.x() + ok_rect_offset,
			option.rect.y() + ok_rect_offset,
			ok_rect_width, ok_rect_width
		);
		if (itemData->is_finished)
		{
			painter->setPen(QColor(100, 100, 100, 100));
			painter->setBrush(QColor(100, 100, 100, 150));
		}
		else
		{
			painter->setPen(QColor(100, 100, 100, 100));
			painter->setBrush(QColor(200, 200, 200, 50));
		}
		//QPoint circle_center(option.rect.x() + 20, option.rect.y() + 20);
		//painter->drawEllipse(ok_rect); 
		painter->drawRoundedRect(ok_rect, 2, 2);

		if (itemData->is_finished)
		{
			painter->setPen(QColor(200, 200, 200, 255));
			painter->drawText(
				option.rect.x() + 10, option.rect.y() + 10,
				20, 20, Qt::AlignCenter, "✔"
			);
		}
	}
	/// text draw
	{
		//QRect text_rect = option.rect;
		//text_rect.setX(text_rect.x() + 40);
		QRect text_rect(
			option.rect.x() + ok_container, option.rect.y(),
			option.rect.width() - ok_container - time_container,
			option.rect.height()
		);

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
		painter->setPen(QColor(255, 255, 255, 200));
		painter->drawText(
			text_rect,
			Qt::AlignLeft | Qt::AlignVCenter | Qt::ElideRight,
			text_text
		);
	}
	// time draw
	{
		QRect time_rect(
			option.rect.width() - time_container + 5, option.rect.y(),
			time_container - 5, option.rect.height()
		);
		painter->setPen(QColor(255, 255, 255, 200));
		QString time_text = 
			QDateTime::fromMSecsSinceEpoch(itemData->deadline_date)
			.toString("yyyy/MM/dd");

		painter->drawText(
			time_rect,
			Qt::AlignLeft | Qt::AlignVCenter | Qt::ElideRight,
			time_text
		);
	}

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
		option.rect.x() + ok_container, option.rect.y(),
		option.rect.width() - ok_container - time_container, 
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
		option.rect.x() + ok_container, option.rect.y() + 1,
		option.rect.width() - ok_container - time_container, 
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

	ToDoDelegate* _delegate = new ToDoDelegate;
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
	QVariant var = index.data(Qt::UserRole + 1);
		
	ToDoData* itemData = reinterpret_cast<ToDoData*>(var.value<std::uintptr_t>());

	QRect item_rect = this->visualRect(index);
	int ok_rect_width = 14;
	int of_rect_offset = (40 - ok_rect_width) / 2;
	QRect ok_rect(
		item_rect.x() + of_rect_offset,
		item_rect.y() + of_rect_offset,
		ok_rect_width, ok_rect_width
	);

	int mouse_x = this->mapFromGlobal(QCursor::pos()).x();
	int mouse_y = this->mapFromGlobal(QCursor::pos()).y();
	if (mouse_x >= ok_rect.left() && mouse_x <= ok_rect.right() &&
		mouse_y >= ok_rect.top() && mouse_y <= ok_rect.bottom()
		)
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
		QStandardItemModel* list_view_model = 
			static_cast<QStandardItemModel*>(this->model());
		QStandardItem* item = list_view_model->itemFromIndex(index);
		list_view_model->takeRow(index.row());

		GlobalVariables* gv = GlobalVariables::instance();
		std::ofstream ofs;
		ofs.open(
			gv->getInfoFilePath(),
			std::ios::binary | std::ios::out | std::ios::in
		);

		ofs.seekp(itemData->info_ptr + itemData->is_finished_offset);
		ofs.write((char*)(&itemData->is_finished), 1);
		ofs.seekp(itemData->info_ptr + itemData->finished_date_time_offset);
		ofs.write((char*)(&itemData->finished_date_time), 8);

		ofs.close();
		addItemToModel(item, itemData, list_view_model);
	}
}
