#pragma once

#include <QListView>

class ToDoListView : public QListView
{
public:
	ToDoListView(QWidget* parent = nullptr);

	void resizeEvent(QResizeEvent* event) override;

	void itemClicked(const QModelIndex& index);
	void itemDoubleClicked(const QModelIndex& index) {}
};