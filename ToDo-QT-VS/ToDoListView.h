#pragma once

#include <QListView>

class ToDoData
{
public:
	enum class DeadlineType
	{
		Day,
		Time
	};
public:
	std::string thing = "";
	bool is_finished = false;
	int64_t create_time = 0;
	int64_t deadline_date = 0;
	int64_t deadline_time = 0;
	DeadlineType deadline_type = DeadlineType::Day;
	int64_t finished_time = 0;

	bool operator<(const ToDoData& other) const
	{
		// 1. is_finished 为 false 的优先
		if (is_finished != other.is_finished) 
		{
			return !is_finished; // is_finished 为 false 的排在前面
		}
		// 2. deadline_date 小的优先
		if (deadline_date != other.deadline_date) 
		{
			return deadline_date < other.deadline_date;
		}
		// 3. deadline_type 为 Time 的优先
		if (deadline_type != other.deadline_type) 
		{
			return deadline_type == DeadlineType::Time; // Time 排在前面
		}
		// 4. deadline_time 小的优先
		if (deadline_time != other.deadline_time)
		{
			return deadline_time < other.deadline_time;
		}
		// 如果所有条件都相同，则默认返回 false
		return false;
	}
};

class ToDoListView : public QListView
{
public:
	ToDoListView(QWidget* parent = nullptr);

	void resizeEvent(QResizeEvent* event) override;

	void itemClicked(const QModelIndex& index);
	void itemDoubleClicked(const QModelIndex& index) {}
};