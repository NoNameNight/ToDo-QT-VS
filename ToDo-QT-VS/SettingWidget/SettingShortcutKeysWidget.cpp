#include "SettingShortcutKeysWidget.h"
#include "GlobalVariables.h"

#include "TitleDivider.h"
#include "HotkeySetWidget.h"

#include <QFormLayout>
#include <QResizeEvent>

SettingShortcutKeysWidget::SettingShortcutKeysWidget(QWidget* parent) :
	NNNWidget(parent)
{
	//QWidget* detail_item = m_detail_list["快捷键"];
	this->setGeometry(
		200, m_padding,
		this->width() - 3 * m_padding - 165,
		this->size().height() - 2 * m_padding
	);
	//detail_item->setStyleSheet("QWidget { background-color: green; }");

	int detail_item_padding = 20;

	TitleDivider* detail_td = new TitleDivider("快捷键", this);
	detail_td->setWidth(this->width());

	QFormLayout* _hotkey_form = new QFormLayout(this);
	_hotkey_form->setLabelAlignment(Qt::AlignRight);
	_hotkey_form->setContentsMargins(
		detail_item_padding, detail_item_padding,
		detail_item_padding, detail_item_padding
	);
	HotkeySetWidget* _top_most = new HotkeySetWidget(
		this,
		AppInfoData::instance()->getHotkeyData("top_most")
	);
	_hotkey_form->addRow("窗口置顶: ", _top_most);

	HotkeySetWidget* _mouse_transparent = new HotkeySetWidget(
		this,
		AppInfoData::instance()->getHotkeyData("mouse_transparent")
	);
	_hotkey_form->addRow("鼠标穿透: ", _mouse_transparent);

	static_cast<NNNWidget*>(this)->setResizeFunc(
		[detail_item_padding, detail_td](QResizeEvent* ev)
		{
			QSize _size = ev->size();
			detail_td->setWidth(_size.width());
		}
	);
	this->hide();
}