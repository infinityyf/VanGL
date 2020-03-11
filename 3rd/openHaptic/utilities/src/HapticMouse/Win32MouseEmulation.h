/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  WindowsMouseEmulation.cpp

Description: 

  Emulates mouse input based on the motion and button presses of the haptic
  device. Also interprets the windows environment and generates haptic
  constraints and force effects during motion.

******************************************************************************/

#ifndef WindowMouseEmulation_H_
#define WindowMouseEmulation_H_

namespace HapticMouse
{

// All generated mouse events are tagged with this value
#define HAPTIC_MOUSE_EVENT     0x050565

enum MouseButton
{
    LEFT_MOUSE_BUTTON   = 0x01,
    MIDDLE_MOUSE_BUTTON = 0x02,
    RIGHT_MOUSE_BUTTON  = 0x04        
};

// Mouse input emulation using the Win32 SendInput API
void mouseMove(const POINT &pt);
void mouseButtonDown(DWORD dwButtonMask);
void mouseButtonUp(DWORD dwButtonMask);

// Mouse input emulation using PostMessage API
void mouseMove(HWND hWnd, DWORD dwButtonMask, const POINT &pt);
void mouseButtonDown(HWND hWnd, DWORD dwButtonMask, const POINT &pt);
void mouseButtonUp(HWND hWnd, DWORD dwButtonMask, const POINT &pt);

// Utility routines
bool isMouseCaptured(HWND hWnd, HWND &hCaptureWnd);
bool isPointInWindow(HWND hWnd, const POINT &pt);
bool isForegroundWindow(HWND hWnd);
void hideMouseCursor();

} // namespace HapticMouse

#endif // WindowMouseEmulation_H_
