/******************************************************************************
/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  CriticalSectionLock.h

Description: 

  Wrappers usage of an operating system synchronization primitive

******************************************************************************/

#include "HapticMouseAfx.h"
#include "CriticalSectionLock.h"

CriticalSectionLock::CriticalSectionLock()
{
    InitializeCriticalSection(&m_cs);
}

CriticalSectionLock::~CriticalSectionLock()
{
    DeleteCriticalSection(&m_cs);
}

void CriticalSectionLock::enter()
{
    EnterCriticalSection(&m_cs);
}
    
void CriticalSectionLock::leave()
{
    LeaveCriticalSection(&m_cs);
}
