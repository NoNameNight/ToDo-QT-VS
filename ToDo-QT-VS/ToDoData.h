#pragma once

#include <stdint.h>
#include <string>

class ToDoData
{
public:
	//enum class DeadlineType : char
	//{
	//	Day,
	//	Time
	//};
public:
	int64_t data_ptr = 0;
	int64_t info_ptr = 0;
	int64_t text_ptr = 0;
	//std::string uuid = "";
	std::string thing = "";
	bool is_finished = false;
	int64_t create_date_time = 0;
	//DeadlineType deadline_type = DeadlineType::Day;
	int64_t deadline_date = 0;    // 截止日期
	int64_t deadline_time = 0;    // 截止日期那天的时间
	int64_t finished_date_time = 0;

	void setThing(
		std::string info_file_path, std::string text_file_path,
		std::string thing
	);
	void setIsFinished(std::string info_file_path, bool flag);
	void setFinishedTime(std::string info_file_path, int64_t time);

	void deleteThis(std::string data_file_path);

	bool operator<(const ToDoData& other) const;
public:
	static const int text_ptr_offset;
	static const int is_finished_offset;
	static const int create_date_time_offset;
	//static const int deadline_type_offset;
	static const int deadline_date_offset;
	static const int deadline_time_offset;
	static const int finished_date_time_offset;

	static const int this_size;
};