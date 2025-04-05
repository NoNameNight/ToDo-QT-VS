#pragma once

#include <QMessageBox>
#include <QSharedMemory>

class NSharedMemory
{
public:
	NSharedMemory(const char* name)
		: m_sharedMemory(name)
	{
		if (m_sharedMemory.attach())
		{
			QMessageBox::warning(nullptr, "Error", "An instance of the application is already running.");
		}
		else
		{
			m_sharedMemory.create(1); // 创建1byte大小的共享内存段
			m_have_create = true;
		}
	}
	~NSharedMemory()
	{
		if (m_have_create)
		{
			m_sharedMemory.detach();
		}
	}

	bool canUse() const
	{
		return m_have_create;
	}

private:
	QSharedMemory m_sharedMemory;
	bool m_have_create = false;
};