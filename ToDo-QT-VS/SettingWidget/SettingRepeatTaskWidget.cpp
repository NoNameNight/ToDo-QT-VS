#include "SettingRepeatTaskWidget.h"
#include "GlobalVariables.h"

#include "RepeatDataListView.h"
#include "TitleDivider.h"
#include "Util.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QStandardItem>
#include <QResizeEvent>

SettingRepeatTaskWidget::SettingRepeatTaskWidget(QWidget* parent) :
	NNNWidget(parent)
{
	// 创建重复任务的设置主页面
	//m_detail_list["重复任务"] = new NNNWidget(this);
	{
		//QWidget* detail_item = m_detail_list["重复任务"];
		this->setGeometry(
			200, m_padding,
			this->width() - 3 * m_padding - 165,
			this->size().height() - 2 * m_padding
		);
		//detail_item->setStyleSheet("background-color: red;");
		int detail_item_padding = 20;

		TitleDivider* detail_top_td = new TitleDivider("任务添加", this);
		detail_top_td->setWidth(this->width());

		QFormLayout* detail_top_item = new QFormLayout(this);
		RepeatDataListView* detail_repeat_data_list_view =
			new RepeatDataListView(this);
		detail_repeat_data_list_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		//detail_top_item->setRowWrapPolicy(QFormLayout::WrapAllRows);
		detail_top_item->setLabelAlignment(Qt::AlignRight);
		detail_top_item->setContentsMargins(
			detail_item_padding, detail_item_padding,
			detail_item_padding, detail_item_padding
		);
		// 任务添加内容创建
		{
			QLineEdit* thing_input_edit = new QLineEdit(this);          //任务

			thing_input_edit->setStyleSheet(
				"border: 0;"
				"border-bottom: 1px solid #A0A0A0;"
				"background-color:transparent;"
			);
			detail_top_item->addRow("任务: ", thing_input_edit);

			QHBoxLayout* repeat_box_layout = new QHBoxLayout(this);
			repeat_box_layout->setSpacing(10);
			QComboBox* repeat_box = new QComboBox(this);                //重复
			repeat_box->setFixedWidth(80);
			repeat_box->addItem(
				"每天", static_cast<char>(ToDoRepeatData::RepeatType::EveryDay)
			);
			repeat_box->addItem(
				"每周", static_cast<char>(ToDoRepeatData::RepeatType::EveryWeek)
			);
			repeat_box->addItem(
				"每月", static_cast<char>(ToDoRepeatData::RepeatType::EveryMonth)
			);
			repeat_box->addItem(
				"每年", static_cast<char>(ToDoRepeatData::RepeatType::EveryYear)
			);
			//detail_top_item->addRow("重复: ", repeat_box);
			repeat_box_layout->addWidget(repeat_box);
			QSpinBox* detail_repeat_date_box = new QSpinBox(this);      //具体重复
			detail_repeat_date_box->setFixedWidth(90);
			detail_repeat_date_box->setMinimum(1);
			detail_repeat_date_box->setMaximum(1);
			detail_repeat_date_box->setPrefix("第 ");
			detail_repeat_date_box->setSuffix(" 天");  // 后缀
			detail_repeat_date_box->setEnabled(false);
			//detail_top_item->addRow("具体重复: ", detail_repeat_date_box);
			repeat_box_layout->addWidget(detail_repeat_date_box);
			detail_top_item->addRow("重复:", repeat_box_layout);
			connect(repeat_box, &QComboBox::currentIndexChanged, this,
				[this, repeat_box, detail_repeat_date_box]() {
					if (repeat_box->currentText() == "每天")
					{
						detail_repeat_date_box->setMinimum(1);
						detail_repeat_date_box->setMaximum(1);
						detail_repeat_date_box->setEnabled(false);
					}
					else if (repeat_box->currentText() == "每周")
					{
						detail_repeat_date_box->setMinimum(1);
						detail_repeat_date_box->setMaximum(7);
						detail_repeat_date_box->setEnabled(true);
					}
					else if (repeat_box->currentText() == "每月")
					{
						detail_repeat_date_box->setValue(1);
						detail_repeat_date_box->setEnabled(false);
					}
					else if (repeat_box->currentText() == "每年")
					{
						detail_repeat_date_box->setValue(1);
						detail_repeat_date_box->setEnabled(false);
					}
				}
			);

			QSpinBox* duration_box = new QSpinBox(this);                //持续时间
			duration_box->setFixedWidth(80);
			duration_box->setMinimum(1);
			duration_box->setSuffix(" 天");  // 后缀
			detail_top_item->addRow("持续时间: ", duration_box);

			NNNTimeEditor* deadline_time_box = new NNNTimeEditor(this); //结束时间
			deadline_time_box->setFixedWidth(120);
			deadline_time_box->setTime(QTime(23, 59, 59));
			detail_top_item->addRow("结束时间: ", deadline_time_box);

			QPushButton* add_button = new QPushButton("添加", this);    //添加
			add_button->setFixedWidth(80);
			connect(add_button, &QPushButton::clicked, this,   //绑定添加按钮
				[thing_input_edit, repeat_box, detail_repeat_data_list_view,
				duration_box, deadline_time_box, detail_repeat_date_box]() {
					GlobalVariables* gv = GlobalVariables::instance();
					int64_t _duration =
						static_cast<int64_t>(duration_box->value())
						* 24 * 60 * 60 * 1000;
					int64_t _deadline_time = deadline_time_box->time().msecsSinceStartOfDay();
					int32_t _detail_repeat_date = detail_repeat_date_box->value();
					gv->addRepeatData(
						thing_input_edit->text().toStdString(),
						static_cast<ToDoRepeatData::RepeatType>(
							repeat_box->currentData().value<char>()
							), _detail_repeat_date, _duration, _deadline_time
					);
					QStandardItem* _item = new QStandardItem();
					_item->setData(
						reinterpret_cast<uintptr_t>(
							gv->repeat_data_list[gv->repeat_data_list.size() - 1]
							),
						Qt::UserRole + 1
					);
					static_cast<QStandardItemModel*>(
						detail_repeat_data_list_view->model()
						)->appendRow(_item);

					thing_input_edit->setText("");
					repeat_box->setCurrentIndex(0);
					duration_box->setValue(1);
					deadline_time_box->setTime(QTime(23, 59, 59));
				}
			);
			detail_top_item->addRow(add_button);
		}
		int detail_repeat_data_td_y =
			detail_top_item->sizeHint().height();//detail_top_item_padding +
		TitleDivider* detail_repeat_data_td =
			new TitleDivider("重复任务", this);
		detail_repeat_data_td->setWidth(this->width());
		detail_repeat_data_td->move(
			detail_repeat_data_td->geometry().x(), detail_repeat_data_td_y
		);
		//RepeatDataListView* detail_repeat_data_list_view = new RepeatDataListView(detail_item); 
		detail_repeat_data_list_view->setModel(new QStandardItemModel());
		// 重复任务内容显示创建
		{
			QStandardItemModel* _model = static_cast<QStandardItemModel*>(
				detail_repeat_data_list_view->model()
				);
			GlobalVariables* gv = GlobalVariables::instance();
			for (size_t i = 0; i < gv->repeat_data_list.size(); ++i)
			{
				QStandardItem* _item = new QStandardItem();
				_item->setData(
					reinterpret_cast<uintptr_t>(gv->repeat_data_list[i]),
					Qt::UserRole + 1
				);
				_model->appendRow(_item);
			}
		}
		int detail_repeat_data_list_view_y =
			detail_repeat_data_td_y + detail_repeat_data_td->size().height() + 5;
		detail_repeat_data_list_view->setGeometry(
			detail_item_padding, detail_repeat_data_list_view_y,
			this->width(),
			this->height() - detail_repeat_data_list_view_y
		);

		static_cast<NNNWidget*>(this)->setResizeFunc(
			[detail_item_padding, detail_top_td, detail_repeat_data_td,
			detail_repeat_data_list_view](QResizeEvent* ev)
			{
				QSize _size = ev->size();
				detail_top_td->setWidth(_size.width());
				detail_repeat_data_td->setWidth(_size.width());
				detail_repeat_data_list_view->resize(
					_size.width() - 2 * detail_item_padding,
					_size.height() - detail_repeat_data_list_view->y()
				);
			}
		);
		this->hide();
	}
}