#include "SettingWidget.h"
#include "Util.h"

#include "SettingWidget/SettingRepeatTaskWidget.h"
#include "SettingWidget/SettingShortcutKeysWidget.h"
#include "SettingWidget/SettingMainUIColorWidget.h"

#include "NNNWidget.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStringListModel>
#include <QResizeEvent>

class SettingChoiceDelegate : public QStyledItemDelegate
{
public:
	SettingChoiceDelegate(QObject*parent = nullptr):
		QStyledItemDelegate(parent)
	{ }
	QWidget* createEditor(
		QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override
	{
		return nullptr;
	}
	QSize sizeHint(
		const QStyleOptionViewItem& option, const QModelIndex& index
	) const override
	{
		return QSize(option.rect.width(), 40);
	}
	void paint(
		QPainter* painter,
		const QStyleOptionViewItem& option, const QModelIndex& index
	) const override
	{
		painter->save();
		QRect real_rect(
			option.rect.x() + 5, option.rect.y() + 5,
			option.rect.width() - 10, option.rect.height() - 5
		);
		// bg draw
		{
			if (option.state.testFlag(QStyle::State_Selected))
			{
				painter->setPen(QColor(170, 170, 170, 100));
				painter->setBrush(QColor(170, 170, 170, 100));
				painter->drawRoundedRect(real_rect, 5, 5);
			}
			else if (option.state.testFlag(QStyle::State_MouseOver))
			{
				painter->setPen(QColor(100, 100, 100, 100));
				painter->setBrush(QColor(100, 100, 100, 100));
				painter->drawRoundedRect(real_rect, 5, 5);
			}
		}

		// text draw
		{
			QString data = index.data(Qt::DisplayRole).toString();
			QFontMetrics metrics(painter->font());
			QString text_text = metrics.elidedText(
				data,
				Qt::ElideRight, real_rect.width()
			);
			painter->setPen(QColor(255, 255, 255, 200));
			painter->drawText(
				real_rect,
				Qt::AlignCenter | Qt::ElideRight,
				text_text
			);
		}
		painter->restore();
		//QStyledItemDelegate::paint(painter, option, index);
	}
};

class SettingChoiceViewList : public QListView
{
public:
	SettingChoiceViewList(QWidget* parent = nullptr) :
		QListView(parent)
	{
		connect(
			this, &SettingChoiceViewList::clicked, 
			this, &SettingChoiceViewList::itemClicked
		);

		this->setItemDelegate(new SettingChoiceDelegate(this));
		this->setStyleSheet("QListView { background-color: transparent; }"); 
	}

	void setClickedFunc(std::function<void(const QModelIndex&)> func)
	{
		m_clicked_func = func;
	}
protected:
	void itemClicked(const QModelIndex& index)
	{
		if (m_clicked_func)
		{
			m_clicked_func(index);
		}
	}
private:
	std::function<void(const QModelIndex&)> m_clicked_func;
};

SettingWidget::SettingWidget(QWidget* parent) :
	QDialog(parent)
{
	this->resize(800, 600);
	this->setMinimumSize(800, 600);
	m_choice_list = new SettingChoiceViewList(this);
	m_choice_list->setGeometry( 
		m_choice_list_padding, m_choice_list_padding,
		165, this->size().height() - 2 * m_choice_list_padding
	);
	static_cast<SettingChoiceViewList*>(m_choice_list)->setClickedFunc(
		[this](const QModelIndex& index) {
			std::string detail_name = index.data().toString().toStdString();
			if (m_current_detail) { m_current_detail->hide(); }
			m_current_detail = m_detail_list[detail_name];
			m_current_detail->resize(
				this->width() - 3 * m_choice_list_padding - 165,
				this->size().height() - 2 * m_choice_list_padding
			);
			m_current_detail->show();
		}
	);
	QStringListModel* model = new QStringListModel();
	QStringList list;
	list.append("重复任务");
	list.append("快捷键");
	list.append("主界面颜色");
	model->setStringList(list);
	m_choice_list->setModel(model);
	m_choice_list->setCurrentIndex(model->index(0)); 

	// 创建重复任务的设置主页面
	m_detail_list["重复任务"] = new SettingRepeatTaskWidget(this);
	m_detail_list["快捷键"] = new SettingShortcutKeysWidget(this);
	m_detail_list["主界面颜色"] = new SettingMainUIColorWidget(this);

	for (auto _it = m_detail_list.begin(); _it != m_detail_list.end(); ++_it)
	{
	}

	m_current_detail = m_detail_list["重复任务"];
	m_current_detail->show();
}

void SettingWidget::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);
	m_choice_list->resize(
		165, this->size().height() - 2 * m_choice_list_padding
	);
	m_current_detail->resize(
		this->width() - 3 * m_choice_list_padding - 165, 
		this->size().height() - 2 * m_choice_list_padding
	);
}
