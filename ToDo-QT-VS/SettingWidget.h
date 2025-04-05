#pragma once

#include <QWidget>
#include <QDialog>
#include <QListView>

#include <unordered_map>

class SettingWidget : public QDialog
{
public:
	SettingWidget(QWidget* parent = nullptr);
protected:
	void resizeEvent(QResizeEvent* event) override;
private:
	QListView* m_choice_list = nullptr;
	int m_choice_list_padding = 20;
	QWidget* m_current_detail = nullptr;
	std::unordered_map<std::string, QWidget*> m_detail_list; 
};