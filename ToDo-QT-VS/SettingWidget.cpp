#include "SettingWidget.h"
#include "TitleDivider.h"
#include "Util.h"

#include <functional>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QLineEdit>
#include <QStringListModel>
#include <QResizeEvent>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QMessageBox>

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

class RepeatDataDelegate : public QStyledItemDelegate
{
private:
	enum class RepeatNowRepeatType : char
	{
		None = '0',
		Thing,
		Repeat,
		Duration
	};
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
	) const override
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

		QRect thing_box_rect(
			option.rect.x() + m_thing_left_padding, option.rect.y(),
			option.rect.width() - m_repeat_box_width - 
			m_thing_left_padding - m_duration_box_width - m_delete_box_width,
			option.rect.height()
		);
		QRect repeat_box_rect(
			option.rect.right() - m_repeat_box_width - 
			m_duration_box_width - m_delete_box_width,
			option.rect.top(),
			m_repeat_box_width, option.rect.height()
		);
		QRect duration_box_rect(
			option.rect.right() - m_duration_box_width - m_delete_box_width,
			option.rect.top(),
			m_duration_box_width, option.rect.height()
		);
		int delete_png_hpadding = (m_delete_box_width - m_delete_png_width) / 2;
		int delete_png_vpadding = (option.rect.height() - m_delete_png_width) / 2;
		QRect delete_png_rect(
			option.rect.right() - m_delete_box_width + delete_png_hpadding,
			option.rect.top() + delete_png_vpadding,
			m_delete_png_width, m_delete_png_width
		);
		QFontMetrics metrics(painter->font());
		QString text_text = metrics.elidedText(
			_data->thing.data(),
			Qt::ElideRight, thing_box_rect.width() 
		);
		painter->save();
		painter->drawText(
			thing_box_rect, Qt::AlignLeft | Qt::AlignVCenter, 
			text_text
		);
		painter->drawText(
			repeat_box_rect, Qt::AlignCenter,
			QString::fromStdString(_data->getRepeatTypeString())
		);
		int64_t _duration_ms = _data->duration;
		int _duration_day = _duration_ms / 1000 / 60 / 60 / 24;
		painter->drawText(
			duration_box_rect, Qt::AlignCenter,
			QString::fromStdString(std::to_string(_duration_day) + "天")
		);
		painter->drawPixmap(delete_png_rect, *GlobalVariables::instance()->delete_png);
		painter->restore();
	}
	QWidget* createEditor(
		QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override
	{
		ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
			index.data(Qt::UserRole + 1).value<uintptr_t>()
		);

		int mouse_x = parent->mapFromGlobal(QCursor::pos()).x();
		int mouse_y = parent->mapFromGlobal(QCursor::pos()).y();

		QRect thing_box_rect(
			option.rect.x(), option.rect.y(),
			option.rect.width() - m_repeat_box_width - 
			m_duration_box_width - m_delete_box_width,
			option.rect.height()
		);
		if (thing_box_rect.contains(mouse_x, mouse_y)) 
		{
			QWidget* _thing_box =
				QStyledItemDelegate::createEditor(parent, option, index);
			_thing_box->setWhatsThis("thing_box");
			return _thing_box;
		}

		QRect repeat_box_rect(
			option.rect.right() - m_repeat_box_width -
			m_duration_box_width - m_delete_box_width,
			option.rect.top(),
			m_repeat_box_width, option.rect.height()
		);
		if (repeat_box_rect.contains(mouse_x, mouse_y))
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

		QRect duration_box_rect(
			option.rect.right() - m_duration_box_width - m_delete_box_width,
			option.rect.top(),
			m_duration_box_width, option.rect.height()
		);
		if (duration_box_rect.contains(mouse_x, mouse_y))
		{
			QSpinBox* _duration_box = new QSpinBox(parent);
			_duration_box->setMinimum(1);
			_duration_box->setSuffix(" 天");  // 后缀
			_duration_box->setWhatsThis("duration_box");
			_duration_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
			return _duration_box;
		}
		
		return nullptr;
	}
	void updateEditorGeometry(
		QWidget* editor, const QStyleOptionViewItem& option, 
		const QModelIndex& index
	) const override
	{
		QString _now_repeat_type = editor->whatsThis();
		if (_now_repeat_type == "thing_box")
		{
			editor->setStyleSheet("border-width:0;border-style:outset;");
			QRect editor_rect(
				option.rect.x() + m_thing_left_padding, option.rect.y() + 1,
				option.rect.width() - m_repeat_box_width - m_thing_left_padding -
				m_duration_box_width,
				option.rect.height() - 2
			);
			editor->setGeometry(editor_rect);
		}
		else if (_now_repeat_type == "repeat_box")
		{
			editor->setStyleSheet("border-width:0;"); 
			QRect _repeat_rect(
				option.rect.right() - m_repeat_box_width -
				m_duration_box_width - m_delete_box_width,
				option.rect.top(),
				m_repeat_box_width, option.rect.height()
			);
			editor->setGeometry(_repeat_rect);
		}
		else if (_now_repeat_type == "duration_box")
		{
			editor->setStyleSheet("border-width:0;padding-right:0;");
			//editor->setStyleSheet("QSpinBox::up-button {width: 0;}");
			//editor->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button { width: 0; }");
			QRect _duration_rect(
				option.rect.right() - m_duration_box_width - m_delete_box_width,
				option.rect.top(),
				m_duration_box_width, option.rect.height()
			);
			editor->setGeometry(_duration_rect);
			editor->updateGeometry();
			editor->update();
			editor->parentWidget()->updateGeometry();
		}
	}
	void setEditorData(QWidget* editor, const QModelIndex& index) const override
	{
		QStyledItemDelegate::setEditorData(editor, index);

		QString _now_repeat_type = editor->whatsThis();
		ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
			index.data(Qt::UserRole + 1).value<uintptr_t>()
		);
		if (_now_repeat_type == "thing_box")
		{
			QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
			lineEdit->setText(QString::fromStdString(_data->thing));
		}
		else if (_now_repeat_type == "repeat_box")
		{
			QComboBox* _repeat_box = static_cast<QComboBox*>(editor);
			_repeat_box->setCurrentIndex(static_cast<int>(_data->repeat_type));
		}
		else if (_now_repeat_type == "duration_box")
		{
			QSpinBox* _duration_box = static_cast<QSpinBox*>(editor);
			_duration_box->setValue(_data->duration / 1000 / 60 / 60 / 24);
		}
	}

	void setModelData(
		QWidget* editor, QAbstractItemModel* model, const QModelIndex& index
	) const override
	{
		ToDoRepeatData* _data = reinterpret_cast<ToDoRepeatData*>(
			index.data(Qt::UserRole + 1).value<uintptr_t>()
			);

		GlobalVariables* gv = GlobalVariables::instance();
		QString _now_repeat_type = editor->whatsThis();

		if (_now_repeat_type == "thing_box")
		{
			QLineEdit* lineBox = static_cast<QLineEdit*>(editor);
			_data->setThing(
				gv->getRepeatInfoFilePath(), gv->getRepeatTextFilePath(),
				lineBox->text().toStdString()
			);
		}
		else if (_now_repeat_type == "repeat_box")
		{
			QComboBox* _repeat_box = static_cast<QComboBox*>(editor);
			_data->setRepeatType(
				gv->getRepeatInfoFilePath(),
				_repeat_box->currentData().value<ToDoRepeatData::RepeatType>()
			);
		}
		else if (_now_repeat_type == "duration_box")
		{
			QSpinBox* _duration_box = static_cast<QSpinBox*>(editor);
			_data->setDuration(
				gv->getRepeatInfoFilePath(), 
				static_cast<int64_t>(_duration_box->value()) * 24 * 60 * 60 * 1000
			);
		}
	}
private:
	int m_repeat_box_width = 80; 
	int m_thing_left_padding = 10;
	int m_duration_box_width = 60;
	int m_delete_box_width = 20;
	int m_delete_png_width = 15;
	RepeatNowRepeatType m_now_repeat_type = RepeatNowRepeatType::None;
};

class RepeatDataListView : public QListView
{
public:
	RepeatDataListView(QWidget* parent = nullptr) :
		QListView(parent)
	{
		connect(
			this, &RepeatDataListView::clicked,
			this, &RepeatDataListView::itemClicked
		);

		this->setItemDelegate(new RepeatDataDelegate(this));
		//this->setStyleSheet("QListView { background-color: transparent; }");
	}

protected:
	void resizeEvent(QResizeEvent* event) override
	{
		QListView::resizeEvent(event); 
		doItemsLayout(); // 重新计算和布局 item 
		updateGeometry(); // 更新布局 
		update();        // 触发重绘 
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
		QRect item_rect = this->visualRect(index);
		int delete_png_hpadding = (m_delete_box_width - m_delete_png_width) / 2;
		int delete_png_vpadding = (item_rect.height() - m_delete_png_width) / 2;
		QRect delete_png_rect(
			item_rect.right() - m_delete_box_width + delete_png_hpadding,
			item_rect.top() + delete_png_vpadding,
			m_delete_png_width, m_delete_png_width
		);
		QPoint mouse = this->mapFromGlobal(QCursor::pos());
		bool is_mouse_in_delete_png_rect = delete_png_rect.contains(mouse);
		if (is_mouse_in_delete_png_rect)
		{
			GlobalVariables* gv = GlobalVariables::instance();
			gv->deleteRepeatData(index.row());
			QStandardItemModel* list_view_model = 
				static_cast<QStandardItemModel*>(this->model()); 
			list_view_model->takeRow(index.row());
		}
	}
protected:
	std::function<void(const QModelIndex&)> m_clicked_func;
	int m_delete_box_width = 20;
	int m_delete_png_width = 15;
};

class HotkeySetWidget : public QWidget
{
public:
	HotkeySetWidget(QWidget* parent, Config::HotkeyData* data, QHotkey* hotkey) :
		QWidget(parent), m_data(data), m_hotkey(hotkey)
	{
		this->setMinimumHeight(30);
		this->setMinimumWidth(250);
		{
			QComboBox* _func_key_box = new QComboBox(this);
			_func_key_box->setGeometry(0, 0, 140, 30);

			static enum FuncKeyStatue
			{
				Ctrl = 0b1,
				Alt = 0b10,
				Shift = 0b100
			};
			_func_key_box->addItem("Ctrl", Ctrl);
			_func_key_box->addItem("Alt", Alt);
			_func_key_box->addItem("Ctrl+Alt", Ctrl | Alt);
			_func_key_box->addItem("Shift", Shift); 
			_func_key_box->addItem("Ctrl+Shift", Ctrl | Shift);
			_func_key_box->addItem("Alt+Shift", Alt | Shift);
			_func_key_box->addItem("Ctrl+Alt+Shift", Ctrl | Alt | Shift);

			_func_key_box->blockSignals(true);  // 防止递归触发
			_func_key_box->setCurrentIndex(_func_key_box->findData(
				(data->isCtrl() ? FuncKeyStatue::Ctrl : 0) |
				(data->isAlt() ? FuncKeyStatue::Alt : 0) |
				(data->isShift() ? FuncKeyStatue::Shift : 0)
			));
			_func_key_box->blockSignals(false);

			connect(_func_key_box, &QComboBox::currentIndexChanged, this,
				[parent, _func_key_box, data, hotkey]() {
					int _statue = _func_key_box->currentData().value<int>();
					bool _last_ctrl = data->isCtrl();
					bool _last_alt = data->isAlt();
					bool _last_shift = data->isShift();
					data->setIsCtrl(_statue & FuncKeyStatue::Ctrl);
					data->setIsAlt(_statue & FuncKeyStatue::Alt);
					data->setIsShift(_statue & FuncKeyStatue::Shift);
					hotkey->setRegistered(false);

					bool success = hotkey->setShortcut(
						QKeySequence(
							QString::fromStdString(data->getHotkeyString())
						), true
					);

					if (!success)
					{
						data->setIsCtrl(_last_ctrl);
						data->setIsAlt(_last_alt);
						data->setIsShift(_last_shift);
						hotkey->setShortcut(
							QKeySequence(
								QString::fromStdString(data->getHotkeyString())
							), true
						);
						_func_key_box->blockSignals(true);  // 防止递归触发
						_func_key_box->setCurrentIndex(_func_key_box->findData(
							(_last_ctrl ? FuncKeyStatue::Ctrl : 0) | 
							(_last_alt ? FuncKeyStatue::Alt : 0) | 
							(_last_shift ? FuncKeyStatue::Shift : 0)
						));
						_func_key_box->blockSignals(false);
						QMessageBox::warning(parent, "错误", "快捷键已被注册");
					}
				}
			);
		}
		{
			QLineEdit* _char_box = new QLineEdit(this);
			_char_box->setGeometry(155, 0, 40, 30);
			_char_box->setStyleSheet("QLineEdit { border-bottom:none }");
			QRegularExpressionValidator* _validator = new QRegularExpressionValidator(
				QRegularExpression("^[A-Za-z]$"),  // 正则表达式：仅匹配单个大写字母
				_char_box
			);
			_char_box->setValidator(_validator);
			_char_box->setMaxLength(1);
			_char_box->setAlignment(Qt::AlignCenter);
			_char_box->setText(QString::fromStdString(std::string(1, data->hotkeyChar())));
			connect(_char_box, &QLineEdit::textChanged, 
				[_char_box](const QString& text) {
					if (!text.isEmpty()) { _char_box->setText(text.toUpper()); }
				}
			);
			// 连接 returnPressed 信号
			connect(_char_box, &QLineEdit::returnPressed, 
				[parent, _char_box, data, hotkey]() {
					_char_box->clearFocus();
					char _last_char = data->hotkeyChar();
					data->setHotkeyChar(_char_box->text().toStdString()[0]);
					hotkey->setRegistered(false);

					bool success = hotkey->setShortcut(
						QKeySequence(
							QString::fromStdString(data->getHotkeyString())
						), true
					);

					if (!success)
					{
						data->setHotkeyChar(_last_char);
						hotkey->setShortcut( 
							QKeySequence( 
								QString::fromStdString(data->getHotkeyString()) 
							), true
						); 
						_char_box->setText(
							QString::fromStdString(std::string(&_last_char, 1))
						);
						QMessageBox::warning(parent, "错误", "快捷键已被注册");
					}
				}
			);
		}
	}
protected:
private:
	Config::HotkeyData* m_data;
	QHotkey* m_hotkey;
};

class NNNWidget : public QWidget
{
public:
	NNNWidget(QWidget* parent = nullptr) :
		QWidget(parent)
	{ }
	void setResizeFunc(std::function<void(QResizeEvent*)> func)
	{ m_resize_func = func; }
protected:
	void resizeEvent(QResizeEvent* ev) override 
	{
		QWidget::resizeEvent(ev); 
		if (m_resize_func) { m_resize_func(ev); } 
	}
private:
	std::function<void(QResizeEvent*)> m_resize_func;
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
	model->setStringList(list);
	m_choice_list->setModel(model);
	m_choice_list->setCurrentIndex(model->index(0)); 

	m_detail_list["重复任务"] = new NNNWidget(this);
	{
		QWidget* detail_item = m_detail_list["重复任务"];
		detail_item->setGeometry(
			200, m_choice_list_padding,
			this->width() - 3 * m_choice_list_padding - 165,
			this->size().height() - 2 * m_choice_list_padding
		);
		//detail_item->setStyleSheet("background-color: red;");
		int detail_item_padding = 20;

		TitleDivider* detail_top_td = new TitleDivider("任务添加", detail_item);
		detail_top_td->setWidth(detail_item->width());

		QFormLayout* detail_top_item = new QFormLayout(detail_item);
		RepeatDataListView* detail_repeat_data_list_view = 
			new RepeatDataListView(detail_item);
		//detail_top_item->setRowWrapPolicy(QFormLayout::WrapAllRows);
		detail_top_item->setLabelAlignment(Qt::AlignRight);
		detail_top_item->setContentsMargins(
			detail_item_padding, detail_item_padding,
			detail_item_padding, detail_item_padding
		);
		{
			QLineEdit* thing_input_edit = new QLineEdit(detail_item);
			thing_input_edit->setStyleSheet(
				"border: 0;"
				"border-bottom: 1px solid #A0A0A0;"
				"background-color:transparent;"
			);
			detail_top_item->addRow("任务: ", thing_input_edit);

			QComboBox* repeat_box = new QComboBox(detail_item);
			repeat_box->setFixedWidth(80);
			repeat_box->addItem(
				"每天", static_cast<char>(ToDoRepeatData::RepeatType::EveryDay)
			);
			repeat_box->addItem(
				"每周", static_cast<char>(ToDoRepeatData::RepeatType::EveryWeek)
			);
			repeat_box->addItem(
				"每月", static_cast<char>(ToDoRepeatData::RepeatType::EveryMonth)
			);
			repeat_box->addItem(
				"每年", static_cast<char>(ToDoRepeatData::RepeatType::EveryYear)
			);
			detail_top_item->addRow("重复: ", repeat_box);
			
			QSpinBox* duration_box = new QSpinBox(detail_item);
			duration_box->setFixedWidth(80);
			duration_box->setMinimum(1);
			duration_box->setSuffix(" 天");  // 后缀
			detail_top_item->addRow("持续时间: ", duration_box);

			QPushButton* add_button = new QPushButton("添加", detail_item);
			add_button->setFixedWidth(80);
			connect(add_button, &QPushButton::clicked, this,
				[thing_input_edit, repeat_box, detail_repeat_data_list_view, duration_box]() {
					GlobalVariables* gv = GlobalVariables::instance();
					int64_t _duration = 
						static_cast<int64_t>(duration_box->value()) 
						* 24 * 60 * 60 * 1000;
					gv->addRepeatData(
						thing_input_edit->text().toStdString(),
						static_cast<ToDoRepeatData::RepeatType>( 
							repeat_box->currentData().value<char>() 
						), _duration
					);
					QStandardItem* _item = new QStandardItem(); 
					_item->setData(
						reinterpret_cast<uintptr_t>(
							gv->repeat_data_list[gv->repeat_data_list.size() - 1]
						),
						Qt::UserRole + 1
					);
					static_cast<QStandardItemModel*>(
						detail_repeat_data_list_view->model()
					)->appendRow(_item);

					thing_input_edit->setText("");
					repeat_box->setCurrentIndex(0);
					duration_box->setValue(1);
				}
			);
			detail_top_item->addRow(add_button);
		}
		int detail_repeat_data_td_y = 
			 detail_top_item->sizeHint().height();//detail_top_item_padding +
		TitleDivider* detail_repeat_data_td = 
			new TitleDivider("重复任务", detail_item);
		detail_repeat_data_td->setWidth(detail_item->width());
		detail_repeat_data_td->move(
			detail_repeat_data_td->geometry().x(), detail_repeat_data_td_y
		);
		//RepeatDataListView* detail_repeat_data_list_view = new RepeatDataListView(detail_item); 
		detail_repeat_data_list_view->setModel(new QStandardItemModel());
		{
			QStandardItemModel* _model = static_cast<QStandardItemModel*>(
				detail_repeat_data_list_view->model()
			);
			GlobalVariables* gv = GlobalVariables::instance();
			for (size_t i = 0; i < gv->repeat_data_list.size(); ++i) 
			{
				QStandardItem* _item = new QStandardItem();
				_item->setData(
					reinterpret_cast<uintptr_t>(gv->repeat_data_list[i]),
					Qt::UserRole + 1
				);
				_model->appendRow(_item);
			}
		}
		int detail_repeat_data_list_view_y =
			detail_repeat_data_td_y + detail_repeat_data_td->size().height() + 5;
		detail_repeat_data_list_view->setGeometry(
			detail_item_padding, detail_repeat_data_list_view_y,
			detail_item->width(),
			detail_item->height() - detail_repeat_data_list_view_y
		);

		static_cast<NNNWidget*>(detail_item)->setResizeFunc(
			[detail_item_padding, detail_top_td, detail_repeat_data_td,
			detail_repeat_data_list_view](QResizeEvent* ev)
			{
				QSize _size = ev->size();
				detail_top_td->setWidth(_size.width());
				detail_repeat_data_td->setWidth(_size.width());
				detail_repeat_data_list_view->resize(
					_size.width() - 2 * detail_item_padding,
					_size.height() - detail_repeat_data_list_view->y()
				);
			}
		);
		detail_item->hide();
	}
	m_detail_list["快捷键"] = new NNNWidget(this);
	{
		QWidget* detail_item = m_detail_list["快捷键"];
		detail_item->setGeometry(
			200, m_choice_list_padding,
			this->width() - 3 * m_choice_list_padding - 165,
			this->size().height() - 2 * m_choice_list_padding
		);
		//detail_item->setStyleSheet("QWidget { background-color: green; }");

		int detail_item_padding = 20;

		TitleDivider* detail_td = new TitleDivider("快捷键", detail_item);
		detail_td->setWidth(detail_item->width());

		QFormLayout* _hotkey_form = new QFormLayout(detail_item);
		_hotkey_form->setLabelAlignment(Qt::AlignRight);
		_hotkey_form->setContentsMargins(
			detail_item_padding, detail_item_padding,
			detail_item_padding, detail_item_padding
		);
		HotkeySetWidget* _top_most = new HotkeySetWidget(
			detail_item,
			Config::instance()->getHotkeyData("top_most"),
			GlobalVariables::instance()->top_most_hotkey
		);
		_hotkey_form->addRow("窗口置顶: ", _top_most);
		
		HotkeySetWidget* _mouse_transparent = new HotkeySetWidget(
			detail_item,
			Config::instance()->getHotkeyData("mouse_transparent"),
			GlobalVariables::instance()->mouse_transparent_hotkey
		);
		_hotkey_form->addRow("鼠标穿透: ", _mouse_transparent);

		static_cast<NNNWidget*>(detail_item)->setResizeFunc( 
			[detail_item_padding, detail_td](QResizeEvent* ev)
			{
				QSize _size = ev->size();
				detail_td->setWidth(_size.width());
			}
		);
		detail_item->hide();
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
