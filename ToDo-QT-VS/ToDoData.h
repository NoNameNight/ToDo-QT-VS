#pragma once

#include <stdint.h>
#include <string>

class ToDoData
{
public:
	//ToDoData(int64_t id);
	//ToDoData(int64_t id, int64_t create_date_time, 
	//	int64_t deadline_date, int64_t deadline_time, std::string task);

public:
	int64_t id = 0;
	std::string task = "";
	bool is_finished = false;
	int64_t create_date_time = 0;
	int64_t deadline_date = 0;    // 截止日期
	int64_t deadline_time = 0;    // 截止日期那天的时间
	int64_t finished_date_time = 0;

	void setTask(std::string task);
	void setIsFinished(bool flag);
	void setFinishedDateTime(int64_t time);

	void deleteThis();

	bool operator<(const ToDoData& other) const;
};