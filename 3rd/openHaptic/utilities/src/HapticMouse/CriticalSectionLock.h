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

#ifndef CriticalSectionLock_H_
#define CriticalSectionLock_H_

class CriticalSectionLock
{
public:
    
    CriticalSectionLock();
    ~CriticalSectionLock();

    virtual void enter();
    virtual void leave();

private:

    CRITICAL_SECTION m_cs;
};

class SafeLock
{
public:

    SafeLock(CriticalSectionLock &cs) : m_cs(cs)
    {
        m_cs.enter();
    }

    ~SafeLock()
    {
        m_cs.leave();
    }

private:

    CriticalSectionLock &m_cs;
};

#endif // CriticalSectionLock_H_
