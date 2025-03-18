
#include "ToDoApp.h"

#include <iostream>
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	ToDoApp w; 
	w.setGeometry(50, 20, 302, 186);
	// 设置鼠标事件穿透
	//w.setWindowFlag(Qt::WindowTransparentForInput);
	w.show(); 

	return a.exec();
}


//#include <cstdint> 
//#include <QApplication>  
//#include <QListView>  
//#include <QStringListModel>  
//#include <QStyledItemDelegate>
//#include <QPainter>
//#include <QStandardItemModel>
//#include <QMetaType>
//#include <QLineEdit>
//
//
//class TempData
//{
//public:
//	TempData() : x(0), y(0) {}
//	TempData(int x, int y) :
//		x(x), y(y)
//	{ }
//	TempData(const TempData&) = default;
//	~TempData() = default;
//	int x, y;
//};
//
//class ThingData
//{
//public:
//	std::string thing = "";
//	bool is_finished = false;
//};
//
//class CustomListView : public QListView
//{
//public:
//	CustomListView(QWidget* parent = nullptr) :
//		QListView(parent)
//	{ 
//		connect(this, &CustomListView::clicked, this, &CustomListView::itemClicked); 
//		connect(this, &CustomListView::doubleClicked, this, &CustomListView::itemDoubleClicked);
//	}
//
//
//	void itemClicked(const QModelIndex& index)
//	{
//		QVariant var = index.data(Qt::UserRole + 1);
//		ThingData* itemData = reinterpret_cast<ThingData*>(var.value<std::uintptr_t>());
//
//		QRect item_rect = this->visualRect(index);
//		int ok_rect_width = 14;
//		int of_rect_offset = (40 - ok_rect_width) / 2;
//		QRect ok_rect(
//			item_rect.x() + of_rect_offset,
//			item_rect.y() + of_rect_offset,
//			ok_rect_width, ok_rect_width
//		);
//
//		int mouse_x = this->mapFromGlobal(QCursor::pos()).x();
//		int mouse_y = this->mapFromGlobal(QCursor::pos()).y();
//		if (mouse_x >= ok_rect.left() && mouse_x <= ok_rect.right() &&
//			mouse_y >= ok_rect.top() && mouse_y <= ok_rect.bottom()
//			)
//		{
//			itemData->is_finished = !itemData->is_finished;
//			// 获取模型并发射 dataChanged 信号
//			QAbstractItemModel* model = this->model();
//			if (model) {
//				emit model->dataChanged(index, index);
//			}
//		}
//	}
//	void itemDoubleClicked(const QModelIndex& index) {}
//
//};
//
//class CustomDelegate : public QStyledItemDelegate 
//{
//public:
//	void paint(
//		QPainter* painter, 
//		const QStyleOptionViewItem& option, const QModelIndex& index
//	) const override
//	{
//		painter->save();
//		//painter->setBrush(QColor(Qt::red));
//		//painter->drawRect(option.rect);
//
//		/// background draw
//		{
//			painter->setPen(QColor(100, 100, 100, 100));
//			painter->drawLine(option.rect.topLeft(), option.rect.topRight());
//			painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
//
//			if (option.state.testFlag(QStyle::State_MouseOver))
//			{
//				painter->setBrush(QColor(100, 100, 100, 100));
//			}
//			else
//			{
//				painter->setBrush(QColor(0, 0, 0, 0));
//			}
//			painter->drawRect(option.rect);
//		}
//
//		QVariant var = index.data(Qt::UserRole + 1);
//		ThingData* itemData = reinterpret_cast<ThingData*>(var.value<std::uintptr_t>());
//
//		/// is ok draw
//		{
//			int ok_rect_width = 14;
//			int of_rect_offset = (40 - ok_rect_width) / 2;
//			QRect ok_rect(
//				option.rect.x() + of_rect_offset, 
//				option.rect.y() + of_rect_offset, 
//				ok_rect_width, ok_rect_width
//			);
//			if (itemData->is_finished)
//			{
//				painter->setPen(QColor(100, 100, 100, 100));
//				painter->setBrush(QColor(100, 100, 100, 150)); 
//			}
//			else
//			{
//				painter->setPen(QColor(100, 100, 100, 100));
//				painter->setBrush(QColor(200, 200, 200, 50)); 
//			}
//			//QPoint circle_center(option.rect.x() + 20, option.rect.y() + 20);
//			//painter->drawEllipse(ok_rect); 
//			painter->drawRoundedRect(ok_rect, 2, 2);
//
//			if (itemData->is_finished)
//			{
//				painter->setPen(QColor(200, 200, 200, 255));
//				painter->drawText(
//					option.rect.x() + 10, option.rect.y() + 10, 
//					20, 20, Qt::AlignCenter, "✔"
//				);
//			}
//		}
//		/// text draw
//		{
//			//QRect text_rect = option.rect;
//			//text_rect.setX(text_rect.x() + 40);
//			QRect text_rect(
//				option.rect.x() + 40, option.rect.y(),
//				option.rect.width() - 40, option.rect.height()
//			);
//
//			if (itemData->is_finished)
//			{
//				QFont font = painter->font();
//				font.setStrikeOut(true);  // 启用删除线
//				painter->setFont(font);
//			}
//
//			QFontMetrics metrics(painter->font());
//			QString text_text = metrics.elidedText(
//				(std::to_string(index.row()) + itemData->thing + "11111111111111111111111111111111111111111111111111111111").data(),
//				Qt::ElideRight, text_rect.width()
//			);
//			painter->setPen(QColor(200, 200, 200, 200));
//			painter->drawText(
//				text_rect,
//				Qt::AlignLeft | Qt::AlignVCenter | Qt::ElideRight,
//				text_text
//			);
//		}
//
//		painter->restore();
//		//QStyledItemDelegate::paint(painter, option, index);
//	}
//	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
//	{
//		return QSize(option.rect.width(), 40); 
//	}
//	QWidget* createEditor(
//		QWidget* parent, const QStyleOptionViewItem& option,
//		const QModelIndex& index
//	) const override
//	{
//		return QStyledItemDelegate::createEditor(parent, option, index);
//	}
//	void updateEditorGeometry(
//		QWidget* editor, const QStyleOptionViewItem& option, 
//		const QModelIndex& index
//	) const override
//	{
//		editor->setStyleSheet("border-width:0;border-style:outset;"); 
//		QRect editor_rect(
//			option.rect.x() + 40, option.rect.y() + 1, 
//			option.rect.width() - 40, option.rect.height() - 2
//		);
//		editor->setGeometry(editor_rect);
//	}
//	void setEditorData(QWidget* editor, const QModelIndex& index) const override
//	{
//		QStyledItemDelegate::setEditorData(editor, index);
//		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
//		QVariant var = index.data(Qt::UserRole + 1);
//		ThingData* itemData = reinterpret_cast<ThingData*>(var.value<std::uintptr_t>());
//
//		lineEdit->setText(itemData->thing.data());
//	}
//	void setModelData(
//		QWidget* editor, QAbstractItemModel* model, 
//		const QModelIndex& index
//	) const override
//	{
//		QVariant var = index.data(Qt::UserRole + 1); 
//		ThingData* itemData = reinterpret_cast<ThingData*>(var.value<std::uintptr_t>()); 
//
//		QLineEdit* lineBox = static_cast<QLineEdit*>(editor);
//		QString str = lineBox->text();
//		
//		itemData->thing = str.toStdString();
//	}
//};
//
//int main(int argc, char* argv[]) 
//{
//	QApplication app(argc, argv);
//
//	CustomListView listView;
//
//	QStandardItemModel* model = new QStandardItemModel(); 
//	//QStandardItem* item1 = new QStandardItem("Item 1"); 
//	//QStandardItem* item2 = new QStandardItem("Item 2"); 
//	//item1->setData(reinterpret_cast<std::uintptr_t>(new TempData(12, 12)), Qt::UserRole + 1);
//	//item2->setData(reinterpret_cast<std::uintptr_t>(new TempData(24, 24)), Qt::UserRole + 1);
//	//model->appendRow(item1);
//	//model->appendRow(item2);
//	for (int i = 0; i < 10; ++i)
//	{
//		QStandardItem* _item = new QStandardItem();
//		ThingData* _data = new ThingData();
//
//		_data->is_finished = false;
//		if (!i) { _data->is_finished = true; }
//		_data->thing = "thing | " + std::to_string(i);
//		_item->setData(reinterpret_cast<std::uintptr_t>(_data), Qt::UserRole + 1);
//		model->appendRow(_item);
//	}
//	listView.setModel(model);
//
//	// 设置选择模式为单选  
//	listView.setSelectionMode(QAbstractItemView::SingleSelection);
//	QObject::connect(&listView, &QListView::clicked, [](const QModelIndex& index) {
//		qDebug() << "Item clicked:" << index.data().toString();
//		});
//
//	// 使用自定义委托  
//	CustomDelegate* delegate = new CustomDelegate;
//	listView.setItemDelegate(delegate);
//
//	listView.setDragEnabled(true);
//	listView.setAcceptDrops(true);
//	listView.setDropIndicatorShown(true);
//	listView.setDefaultDropAction(Qt::MoveAction);
//
//	listView.show();
//
//	return app.exec();
//}