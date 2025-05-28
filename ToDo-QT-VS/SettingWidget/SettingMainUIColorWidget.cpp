#include "SettingMainUIColorWidget.h"

#include "TitleDivider.h"
#include "GlobalVariables.h"
#include "UIColorManager.h"
#include "Util.h"
#include <QResizeEvent>
#include <QFormLayout>
#include <QScrollArea>
#include <QPushButton>

class NUIColorChangeDialog;


class NUIColorLabel : public QLabel
{
public:
	NUIColorLabel(std::string ui_color_name, QColorDialog* dialog, QWidget* parent);

	void setColor(const QColor& color, bool is_save_file = true);

	//QColor color() const { return UIColor2QColor(m_color); }
	QColor color() const 
	{ 
		return UIColorManager::instance()->getColor(m_ui_color_name);
	}
protected:
	void mousePressEvent(QMouseEvent* event) override;

	void paintEvent(QPaintEvent* event) override;

private:
	//Config::UIColor* m_color = nullptr;
	std::string m_ui_color_name;
	QColorDialog* m_color_change_dialog = nullptr;
};
class NUIColorChangeDialog : public QColorDialog
{
public:
	NUIColorChangeDialog(QWidget* parent = nullptr) :
		QColorDialog(parent)
	{
		connect(this, &QColorDialog::currentColorChanged, this,
			[this](const QColor& color) {
				if (m_current_color_label_ptr)
				{
					m_current_color_label_ptr->setColor(color, false);
					m_current_color_label_ptr->update();
				}
			}
		);
		connect(this, &QColorDialog::rejected, this,
			[this]() {
				if (m_current_color_label_ptr)
				{
					m_current_color_label_ptr->setColor(m_last_color, false);
					m_current_color_label_ptr->update();
					m_current_color_label_ptr = nullptr;
				}
			}
		);

		connect(this, &QColorDialog::accepted, this, 
			[this]() {
				QColor _color = this->currentColor();
				if (m_current_color_label_ptr)
				{
					m_current_color_label_ptr->setColor(_color, true);
					m_current_color_label_ptr->update();
					m_current_color_label_ptr = nullptr;
				}
			}
		);
	}

	void setCurrentColorPtr(NUIColorLabel* ptr)
	{
		if (m_current_color_label_ptr)
		{
			m_current_color_label_ptr->setColor(m_last_color, false);
			m_current_color_label_ptr->update();
		}
		m_current_color_label_ptr = ptr;
		m_last_color = m_current_color_label_ptr->color();
	}

protected:
	QColor m_last_color;
	NUIColorLabel* m_current_color_label_ptr = nullptr;
};

NUIColorLabel::NUIColorLabel(std::string ui_color_name,
	QColorDialog* dialog, QWidget* parent) :
	m_ui_color_name(ui_color_name), m_color_change_dialog(dialog),
	QLabel(parent)
{
	setAlignment(Qt::AlignCenter);
	setToolTip("点击选择颜色");
	this->resize(30, 20);
	this->setFixedWidth(30);
}
void NUIColorLabel::setColor(const QColor& color, bool is_save_file)
{
	//m_color->setA(color.alpha(), is_save_file);
	//m_color->setB(color.blue(), is_save_file);
	//m_color->setG(color.green(), is_save_file);
	//m_color->setR(color.red(), is_save_file);
	UIColorManager::instance()->setColor(m_ui_color_name, color);
	if (is_save_file)
	{
		UIColorManager::instance()->saveToFile();
	}
	if (GlobalVariables::instance()->update_main_window_ui)
	{
		GlobalVariables::instance()->update_main_window_ui();
	}
}
void NUIColorLabel::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (!m_color_change_dialog->isVisible())
		{
			m_color_change_dialog->show();
		}
		static_cast<NUIColorChangeDialog*>(m_color_change_dialog)->
			setCurrentColorPtr(this);
		m_color_change_dialog->setCurrentColor(this->color());
	}
	QLabel::mousePressEvent(event);
}
void NUIColorLabel::paintEvent(QPaintEvent* event)
{
	QPainter painter = QPainter(this);
	//if (m_color)
	//{
	//	painter.fillRect(this->rect(), UIColor2QColor(m_color));
	//}
	QColor _color = UIColorManager::instance()->getColor(m_ui_color_name);
	if (_color.isValid())
	{
		painter.fillRect(this->rect(), _color);
	}
	QLabel::paintEvent(event);
}
////////////////////////////////////////////////////////////////////////////////////////////////

SettingMainUIColorWidget::SettingMainUIColorWidget(QWidget* parent) :
	NNNWidget(parent)
{
	QScrollArea* _scroll_area = new QScrollArea(this);
	_scroll_area->setStyleSheet("QScrollArea { background: transparent; border: 0; }");
	_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QWidget* _scroll_content = new QWidget(_scroll_area); 
	_scroll_area->setWidget(_scroll_content);
	m_color_dialog = new NUIColorChangeDialog(this); 
	m_color_dialog->setOption(QColorDialog::ShowAlphaChannel, true); // 启用透明度
	//m_color_dialog->show();
	this->setGeometry(
		200, m_padding,
		this->width() - 3 * m_padding - 165,
		this->size().height() - 2 * m_padding
	);

	int detail_item_padding = 20;

	TitleDivider* detail_td = new TitleDivider("主界面颜色", this);
	detail_td->setWidth(this->width());

	QFormLayout* _ui_color_form = new QFormLayout(_scroll_content);
	_ui_color_form->setLabelAlignment(Qt::AlignCenter);
	_ui_color_form->setVerticalSpacing(10);
	_ui_color_form->setContentsMargins(
		detail_item_padding, 0,
		detail_item_padding, 0
	);

	{
		std::vector<std::pair<std::string, std::string>> _list =
		{
			{ "main_bg", "背景: " },
			{ "main_time", "时间: " },
			{ "main_year", "年份: " },
			{ "main_date", "日期: " },
			{ "main_checkbox_bg", "复选框背景: " },
			{ "main_checkbox_bg_ok", "复选框背景(选中): " },
			{ "main_checkbox_check", "复选框勾: " },
			{ "main_checkbox_border", "复选框边框: " },
			{ "main_task_border", "任务项边框: "},
			{ "main_task_border_out", "任务项边框(超时): " },
			{ "main_task_bg", "任务项背景: " },
			{ "main_task_bg_out", "任务项背景(超时): " },
			{ "main_task_bg_hover", "任务项背景(鼠标悬浮): " },
			{ "main_task_bg_out_hover", "任务项背景(超时,鼠标悬浮): " },
			{ "main_task_task", "任务项任务: " },
			{ "main_task_date", "任务项日期: " }
		};
		NUIColorLabel* _color_label = nullptr;
		for (const auto& it : _list)
		{
			_color_label = new NUIColorLabel(
				it.first, m_color_dialog, _scroll_content
			);
			_ui_color_form->addRow(
				QString::fromStdString(it.second), _color_label
			);
			m_ui_color_update_func_list.emplace_back(
				[_color_label]() {
					_color_label->update();
				}
			);
		}
		m_ui_color_update_func_list.emplace_back(
			[]() {
				if (GlobalVariables::instance()->update_main_window_ui)
				{
					GlobalVariables::instance()->update_main_window_ui(); 
				}
			}
		);
		QPushButton* reset2default = new QPushButton("重置颜色", _scroll_content);
		reset2default->setFixedWidth(80);
		reset2default->setFixedHeight(30);
		connect(reset2default, &QPushButton::clicked, this,
			[this]() {
				UIColorManager::instance()->setColorToDefault();
				for (auto _it = m_ui_color_update_func_list.begin();
					_it != m_ui_color_update_func_list.end(); ++_it)
				{
					(*_it)();
				}
			}
		);
		_ui_color_form->addRow(reset2default);
		_scroll_content->resize(_ui_color_form->sizeHint());
	}

	this->setResizeFunc(
		[detail_item_padding, detail_td, _scroll_area, _scroll_content]
		(QResizeEvent* ev)
		{
			QSize _size = ev->size();
			detail_td->setWidth(_size.width()); 
			_scroll_area->setGeometry(
				_scroll_area->x(), 20, _size.width(), _size.height() - 20
			);
			_scroll_content->resize(
				_size.width(), _scroll_content->height()
			);
		}
	);
	this->hide();
}