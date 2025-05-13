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
	int64_t data_ptr = 0;
	int64_t info_ptr = 0;
	int64_t text_ptr = 0;
	RepeatType repeat_type;
	int32_t detail_repeat_date; // 第几天重复
	std::string thing = "";
	int64_t last_repeat_date = 0;
	int64_t duration = 0;
	int64_t deadline_time = 0;   // 结束那天的时间

	std::string getRepeatTypeString() const
	{
		return this->getRepeatTypeString(this->repeat_type);
	}
	std::string getRepeatTypeString(RepeatType type) const;

	void setThing(
		std::string info_file_path, std::string text_file_path,
		std::string thing
	);
	void setRepeatType(std::string info_file_path, RepeatType type);
	void setDetailRepeateDate(std::string info_file_path, int32_t time);
	void setLastRepeatDate(std::string info_file_path, int64_t time);
	void setDuration(std::string info_file_path, int64_t duration);
	void setDeadlineTime(std::string info_file_path, int64_t time);
	void deleteThis(std::string data_file_path);

	//判断time这天是满足repeat_date
	bool isRepeatDate(int64_t time) const;

	bool isNeedRepeat(int64_t now_time) const;
	int64_t getNeedRepeatDate() const;
public:
	static const int text_ptr_offset;
	static const int repeat_type_offset;
	static const int last_add_date_time_offset;
	static const int duration_offset;
	static const int deadline_time_offset;
	static const int detail_repeat_date_offset;
	//static const int this_offset;
	static const int this_size;
};