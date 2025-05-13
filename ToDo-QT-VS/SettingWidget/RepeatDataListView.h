#pragma once

#include <QListView>

class RepeatDataListView : public QListView
{
public:
	RepeatDataListView(QWidget* parent = nullptr);

protected:
	void resizeEvent(QResizeEvent* event) override;
	void setClickedFunc(std::function<void(const QModelIndex&)> func);
protected:
	void itemClicked(const QModelIndex& index);
protected:
	std::function<void(const QModelIndex&)> m_clicked_func;
	int m_delete_box_width = 20;
	int m_delete_png_width = 15;
};