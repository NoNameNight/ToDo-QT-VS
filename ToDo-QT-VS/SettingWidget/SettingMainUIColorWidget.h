#pragma once

#include "NNNWidget.h"

#include <QColorDialog>

class SettingMainUIColorWidget : public NNNWidget
{
public:
	SettingMainUIColorWidget(QWidget* parent = nullptr);
private:
	int m_padding = 20;
	QColorDialog* m_color_dialog = nullptr;

	std::vector<std::function<void()>> m_ui_color_update_func_list;
};