#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPainter>

class TitleDivider : public QWidget 
{
public:
	explicit TitleDivider(
		const QString& title = "", 
		QWidget* parent = nullptr
	) :
		QWidget(parent), m_title_label(new QLabel(title, this))
	{ 
		setColor(m_color);
		m_title_label->setAlignment(Qt::AlignCenter);
		this->resize(this->size().width(), m_title_label->sizeHint().height());
		m_title_label->move(30, 0);
		//this->setStyleSheet("background-color: red;");
		//m_title_label->setStyleSheet("background-color: green;");
	}

	void paintEvent(QPaintEvent* event)override
	{
		QWidget::paintEvent(event);
		QPainter painter(this);
		QRect this_rect = this->rect();
		painter.setPen(m_color);
		painter.drawLine(
			0, this_rect.height() / 2, 25, this_rect.height() / 2
		);
		painter.drawLine(
			35 + m_title_label->sizeHint().width(), this_rect.height() / 2, 
			this_rect.width(), this_rect.height() / 2
		);
	}

	// 设置标题文本
	void setTitle(const QString& title) 
	{
		m_title_label->setText(title);
	}
	// 设置颜色
	void setColor(const QColor& color)
	{
		m_color = color;
		QPalette palette = m_title_label->palette(); 
		palette.setColor(QPalette::WindowText, m_color);  // 设置文本颜色 
		m_title_label->setPalette(palette);  // 应用调色板 
	}
	void setWidth(int width)
	{
		this->resize(width, this->size().height());
	}
private:
	QColor m_color = QColor(150, 150, 150, 150);
	QLabel* m_title_label = nullptr;
};