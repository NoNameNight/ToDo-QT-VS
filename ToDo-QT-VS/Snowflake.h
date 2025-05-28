#pragma once

#include <cstdlib>
#include <mutex>
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>

//#define LOG(x) { std::cout << __LINE__ << " " << __FILE__ << " "<< std::string(x) << "\n"; }
#define LOG(x) do{ \
time_t now = time(nullptr); \
struct tm ltm;\
localtime_s(&ltm, &now); \
std::cout << "[" \
<< 1900 + ltm.tm_year << "-"\
<< 1 + ltm.tm_mon << "-"\
<< ltm.tm_mday << " "\
<< std::setw(2) << std::setfill('0') << ltm.tm_hour << ":"\
<< std::setw(2) << std::setfill('0') << ltm.tm_min << ":"\
<< std::setw(2) << std::setfill('0') <<  ltm.tm_sec\
<< "]" \
<< __LINE__ << " " << __FILE__ << "  " << std::string(x) << "\n";\
}while(0);

class Snowflake
{
public:
	Snowflake()
	{
	}
	static Snowflake* instance()
	{
		static Snowflake sf;
		return &sf;
	}
	bool init()
	{
		if (m_can_use) { return false; }
		std::unique_lock<std::mutex> ulk(m_mutex);
		if (m_can_use)
		{
			ulk.unlock();
			return false;
		}
		unsigned int m_workid_offset = m_sequence_bits;
		unsigned int m_timestamp_offset = m_workid_offset + m_workid_bits;

		m_max_sequence = 1ULL << m_sequence_bits; // 生成序列的(最大值+1)
		m_max_timestamp = 1ULL << m_timestamp_bits; // 生成序列的(最大值+1)
		m_sequence_mask = ULLONG_MAX ^ (ULLONG_MAX << m_sequence_bits); // 生成序列的掩码

		ulk.unlock();
		m_can_use = true;

		return true;
	}

	bool setWorkid(uint64_t id)
	{
		if (m_can_use) { return false; }
		m_workeid = id;

		return true;
	}
	bool setStartTimestamp(uint64_t start_timestamp)
	{
		if (m_can_use) { return false; }
		m_start_timestamp = start_timestamp;

		return true;
	}
	bool setSignBits(unsigned int sign_bits)
	{
		if (m_can_use) { return false; }
		m_sign_bits = sign_bits;

		return true;
	}
	bool setTimestampBits(unsigned int timestamp_bits)
	{
		if (m_can_use) { return false; }
		m_timestamp_bits = timestamp_bits;
		return true;
	}
	bool setWorkidBits(unsigned int workid_bits)
	{
		if (m_can_use) { return false; }
		m_workid_bits = workid_bits;
		return true;
	}
	bool setSequenceBits(unsigned int sequence_bits)
	{
		if (m_can_use) { return false; }
		m_sequence_bits = sequence_bits;
		return true;
	}

	uint64_t getTimestamp() const
	{
		auto t = std::chrono::time_point_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now()
		);
		return static_cast<uint64_t>(t.time_since_epoch().count());
	}
	uint64_t tilNextMillis() const
	{
		uint64_t _now = getTimestamp();
		while (_now <= m_last_timestamp)
		{
			_now = getTimestamp();
		}
		return _now;
	}

	uint64_t getId()
	{
		if (!m_can_use) { return 0; }
		uint64_t sequence = 0;
		std::unique_lock<std::mutex> ulk(m_mutex);

		uint64_t now_timestamp = getTimestamp();

		while (now_timestamp < m_last_timestamp)
		{
			LOG(std::to_string(now_timestamp) + " timeClockError\n");
			now_timestamp = getTimestamp();
		}
		if (now_timestamp != m_last_timestamp)
		{
			m_sequence = 0;
		}
		sequence = m_sequence;
		if (sequence >= m_max_sequence) { now_timestamp = tilNextMillis(); m_sequence = 0; sequence = 0; }
		m_sequence += 1;
		ulk.unlock();



		m_last_timestamp = now_timestamp;
		if (now_timestamp >= m_max_timestamp)
		{
			throw std::runtime_error("timestamp out");
		}
		//printf("%010x\n", ((now_timestamp) << m_timestamp_offset));
		//std::cout << (m_workeid << m_workid_offset) << "\n";
		return ((now_timestamp - m_start_timestamp) << m_timestamp_offset) |
			(m_workeid << m_workid_offset) |
			sequence;
	}
private:
	bool m_can_use = false;

	unsigned int m_sign_bits = 1; // 符号位所占位数
	unsigned int m_timestamp_bits = 41; // 时间戳 id 所占的位数
	unsigned int m_workid_bits = 10; // 工作机器 id 所占的位数
	unsigned int m_sequence_bits = 12; // 序列所占的位数

	unsigned int m_workid_offset = m_sequence_bits;
	unsigned int m_timestamp_offset = m_workid_offset + m_workid_bits;

	uint64_t m_start_timestamp = 0; // 开始时间截
	uint64_t m_workeid = 0; // 工作机器id
	uint64_t m_sequence = 0; // 序列号
	uint64_t m_last_timestamp = 0;

	uint64_t m_max_sequence = 1ULL << m_sequence_bits; // 生成序列的(最大值+1)
	uint64_t m_max_timestamp = 1ULL << m_timestamp_bits; // 生成序列的(最大值+1)
	uint64_t m_sequence_mask = ULLONG_MAX ^ (ULLONG_MAX << m_sequence_bits); // 生成序列的掩码

	std::mutex m_mutex;
};