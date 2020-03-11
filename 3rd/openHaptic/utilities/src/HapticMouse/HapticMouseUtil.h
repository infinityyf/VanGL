/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  HapticMouseUtil.h

Description: 

******************************************************************************/

#ifndef HapticMouseUtil_H_
#define HapticMouseUtil_H_

#include <HL/hl.h>

struct HLContextScope
{
    HLContextScope(HHLRC hHLRC)
    {
        m_hPrev = hlGetCurrentContext();
        hlMakeCurrent(hHLRC);
    }

    ~HLContextScope()
    {
        hlMakeCurrent(m_hPrev);
    }

    HHLRC m_hPrev;
};

#endif // HapticMouseUtil_H_
