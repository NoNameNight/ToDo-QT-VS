#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QApplication>
#include "Util.h"

class HotkeySetWidget : public QWidget
{
public:
	HotkeySetWidget(QWidget* parent, AppInfoData::HotkeyData* data) :
		QWidget(parent), m_data(data)
	{
		QLabel* _warning_icon = new QLabel(this);

		this->setMinimumHeight(30);
		this->setMinimumWidth(250);
		{
			QComboBox* _func_key_box = new QComboBox(this);
			_func_key_box->setGeometry(0, 0, 140, 30);

			enum FuncKeyStatue : uint32_t
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
				[parent, _func_key_box, data, _warning_icon]() {
					int _statue = _func_key_box->currentData().value<int>();
					bool _last_ctrl = data->isCtrl();
					bool _last_alt = data->isAlt();
					bool _last_shift = data->isShift();
					data->setIsCtrl(_statue & FuncKeyStatue::Ctrl);
					data->setIsAlt(_statue & FuncKeyStatue::Alt);
					data->setIsShift(_statue & FuncKeyStatue::Shift);
					data->hotkey()->setRegistered(false);

					bool success = data->hotkey()->setShortcut(
						QKeySequence(
							QString::fromStdString(data->getHotkeyString())
						), true
					);

					if (!success)
					{
						data->setIsCtrl(_last_ctrl);
						data->setIsAlt(_last_alt);
						data->setIsShift(_last_shift);
						data->hotkey()->setShortcut(
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

					if (data->hotkey()->isRegistered())
					{
						_warning_icon->hide();
					}
					else
					{
						_warning_icon->show();
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
				[parent, _char_box, data, _warning_icon]() {
					_char_box->clearFocus();
					char _last_char = data->hotkeyChar();
					data->setHotkeyChar(_char_box->text().toStdString()[0]);
					data->hotkey()->setRegistered(false);

					bool success = data->hotkey()->setShortcut(
						QKeySequence(
							QString::fromStdString(data->getHotkeyString())
						), true
					);

					if (!success)
					{
						data->setHotkeyChar(_last_char);
						data->hotkey()->setShortcut(
							QKeySequence(
								QString::fromStdString(data->getHotkeyString())
							), true
						);
						_char_box->setText(
							QString::fromStdString(std::string(&_last_char, 1))
						);
						QMessageBox::warning(parent, "错误", "快捷键已被注册");
					}

					if (data->hotkey()->isRegistered())
					{
						_warning_icon->hide();
					}
					else
					{
						_warning_icon->show();
					}
				}
			);
		}
		{
			_warning_icon->setPixmap(
				QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(16, 16)
			); // 设置图标大小16x16像素 
			_warning_icon->setToolTip(tr("热键已被注册")); // 添加悬停提示
			_warning_icon->move(210, 15 - 8);
			_warning_icon->hide();
			if (!data->hotkey()->isRegistered())
			{
				_warning_icon->show();
			}
		}
	}
protected:
private:
	AppInfoData::HotkeyData* m_data;
};