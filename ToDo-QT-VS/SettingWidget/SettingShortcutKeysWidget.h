#pragma once

#include "NNNWidget.h"

class SettingShortcutKeysWidget : public NNNWidget
{
public:
	SettingShortcutKeysWidget(QWidget* parent = nullptr);
private:
	int m_padding = 20;
};

