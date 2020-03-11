/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  HapticMouse.h

Description: 

  Emulates mouse input based on the motion and button presses of the haptic
  device. Also interprets the windows environment and generates haptic
  constraints and force effects during motion.

******************************************************************************/

#include "HapticMouseAfx.h"
#include "HapticMouseManager.h"

#include <HapticMouse/HapticMouse.h>

using namespace HapticMouse;

/******************************************************************************

******************************************************************************/
HLboolean hmInitializeMouse(HHLRC hHLRC, 
                            const char *pClassName, 
                            const char *pWindowName)
{
    return HapticMouseManager::instance()->initialize(
        hHLRC, pClassName, pWindowName) ? HL_TRUE : HL_FALSE;
}

/******************************************************************************

******************************************************************************/
HLboolean hmInitializeMouseWindow(HHLRC hHLRC, HWND hWnd)
{
    return HapticMouseManager::instance()->initialize(
        hHLRC, hWnd) ? HL_TRUE : HL_FALSE;
}

/******************************************************************************

******************************************************************************/
void hmShutdownMouse()
{
    HapticMouseManager::instance()->shutdown();
}


/******************************************************************************

******************************************************************************/
HLboolean hmIsMouseInitialized()
{
    return HapticMouseManager::instance()->isInitialized() ? HL_TRUE : HL_FALSE;
}

/******************************************************************************

******************************************************************************/
HLboolean hmIsMouseActive()
{
    return HapticMouseManager::instance()->isActive() ? HL_TRUE : HL_FALSE;
}

/******************************************************************************

******************************************************************************/
HHLRC hmGetMouseContext()
{
    return HapticMouseManager::instance()->getContext();
}

/******************************************************************************

******************************************************************************/
HWND hmGetMouseWindow()
{
    return HapticMouseManager::instance()->getWindow();
}

/******************************************************************************

******************************************************************************/
void hmSetMouseTransforms(
    const GLdouble modelMatrix[16], 
    const GLdouble projMatrix[16], 
    const GLint viewport[4])
{
    HapticMouseManager::instance()->setTransforms(
        modelMatrix, projMatrix, viewport);
}

/******************************************************************************

******************************************************************************/
void hmProjectModelToScreen(const HLdouble modelPt[3], HLint screenPt[2])
{
    POINT portPt, scrPt;
    HapticMouseManager::instance()->projectModelToScreen(modelPt, portPt, scrPt);
    
    screenPt[0] = scrPt.x;
    screenPt[1] = scrPt.y;
}

/******************************************************************************

******************************************************************************/
void hmRenderMouseScene()
{
    HapticMouseManager::instance()->render();
}

/******************************************************************************

******************************************************************************/
void hmEnableMouse(HMenum cap)
{
    HapticMouseManager::instance()->enable(cap);        
}

/******************************************************************************

******************************************************************************/
void hmDisableMouse(HMenum cap)
{
    HapticMouseManager::instance()->disable(cap);        
}

/******************************************************************************

******************************************************************************/
HLboolean hmIsMouseEnabled(HMenum cap)
{
    return HapticMouseManager::instance()->isEnabled(cap) ? HL_TRUE : HL_FALSE;
}
/******************************************************************************

******************************************************************************/
void hmSetMouseWindow(HWND hWnd)
{
    HapticMouseManager::instance()->setWindow(hWnd);
}
