#pragma once

#include <QWidget>
#include <functional>

class NNNWidget : public QWidget
{
public:
	NNNWidget(QWidget* parent = nullptr) :
		QWidget(parent)
	{
	}
	void setResizeFunc(std::function<void(QResizeEvent*)> func)
	{
		m_resize_func = func;
	}
protected:
	void resizeEvent(QResizeEvent* ev) override
	{
		QWidget::resizeEvent(ev);
		if (m_resize_func) { m_resize_func(ev); }
	}
private:
	std::function<void(QResizeEvent*)> m_resize_func;
};