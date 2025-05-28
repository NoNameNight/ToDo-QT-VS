#pragma once

#include <stdint.h>
#include <string>

class ToDoRepeatData
{
public:
	enum class RepeatType : char
	{
		EveryDay,
		EveryWeek,
		EveryMonth,
		EveryYear
	};
public:
	int64_t id;
	RepeatType repeat_type;
	int32_t detail_repeat_date; // 第几天重复
	std::string task = "";
	int64_t create_date_time = 0;
	int64_t last_add_date = 0;
	int64_t duration = 0;
	int64_t deadline_time = 0;   // 结束那天的时间

	std::string getRepeatTypeString() const
	{
		return this->getRepeatTypeString(this->repeat_type);
	}
	std::string getRepeatTypeString(RepeatType type) const;

	void setTask(std::string task);
	void setRepeatType(RepeatType type);
	void setDetailRepeateDate(int32_t time);
	void setLastAddDate(int64_t time);
	void setDuration(int64_t duration);
	void setDeadlineTime(int64_t time);
	void deleteThis();

	//判断time这天是满足repeat_date
	bool isRepeatDate(int64_t time) const;

	bool isNeedRepeat(int64_t now_time) const;
	int64_t getNeedRepeatDate() const;
public:
};