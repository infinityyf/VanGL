#pragma once
#ifndef THREAD_H
#define THREAD_H

#include "windows.h"
#include <stdlib.h>

enum CThreadPriority
{
	CHAI_THREAD_PRIORITY_GRAPHICS,
	CHAI_THREAD_PRIORITY_HAPTICS
};

class cThread
{
public:
	cThread();
	~cThread();

	void set(void (*a_function)(void), CThreadPriority a_level);
	void setPriority(CThreadPriority a_level);
	CThreadPriority getPriority() { 
		return (m_priorityLevel); 
	}
protected:
	DWORD m_threadId;
	void* m_function;
	CThreadPriority m_priorityLevel;
};

cThread::cThread()
{
	m_function = 0;
	m_priorityLevel = CHAI_THREAD_PRIORITY_GRAPHICS;
}
cThread::~cThread()
{
}
void cThread::set(void (*a_function)(void), CThreadPriority a_level)
{
	// create thread
	CreateThread(
		0,
		0,
		(LPTHREAD_START_ROUTINE)(a_function),
		0,
		0,
		&m_threadId
	);
	// set thread priority level
	setPriority(a_level);
}

void cThread::setPriority(CThreadPriority a_level)
{
	m_priorityLevel = a_level;
	switch (m_priorityLevel)
	{
	case CHAI_THREAD_PRIORITY_GRAPHICS:
		SetThreadPriority(&m_threadId, THREAD_PRIORITY_NORMAL);
		break;

	case CHAI_THREAD_PRIORITY_HAPTICS:
		SetThreadPriority(&m_threadId, THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	}
}
#endif // !THREAD_H

