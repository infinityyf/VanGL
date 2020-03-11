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

#include "HapticMouseAfx.h"
#include "Win32MouseEmulation.h"

#include <HDU/hduMath.h>
#include <assert.h>

namespace HapticMouse
{

namespace
{

inline DWORD composeButtonMask(DWORD dwButtonMask)
{

    // Check if the mouse buttons should be swapped
    if (GetSystemMetrics(SM_SWAPBUTTON))
    {        
        DWORD dwTempMask = 0;

        if ((dwButtonMask & LEFT_MOUSE_BUTTON) != 0)
        {
            dwTempMask |= RIGHT_MOUSE_BUTTON;
        }

        if ((dwButtonMask & RIGHT_MOUSE_BUTTON) != 0)
        {
            dwTempMask |= LEFT_MOUSE_BUTTON;
        }

        if (dwTempMask)
        {
            dwButtonMask = dwTempMask;
        }
    }

    return dwButtonMask;
}

inline WPARAM composeMouseFlags(DWORD dwButtonMask)
{
    WPARAM flags = 0;

    if ( ::GetAsyncKeyState(VK_CONTROL) & 0x8000 ) 
    {
        flags |= MK_CONTROL;
    }

    if ( ::GetAsyncKeyState(VK_SHIFT) & 0x8000 ) 
    {
        flags |= MK_SHIFT;
    }

    if ((dwButtonMask & LEFT_MOUSE_BUTTON) != 0)
    {
        flags |= MK_LBUTTON;
    }

    if ((dwButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
    {
        flags |= MK_MBUTTON;
    }

    if ((dwButtonMask & RIGHT_MOUSE_BUTTON) != 0)
    {
        flags |= MK_RBUTTON;
    }

    return flags;
}

inline LPARAM composeMousePosition(const POINT &pt)
{
    return MAKELPARAM(pt.x, pt.y);
}

} // anonymous namespace 

// The following functions use the general SendInput facility
// for emulating global mouse input. This is the default mode
// that HapticMouse uses for simulating mouse input
void mouseMove(const POINT &pt)
{
    INPUT mouseInput;
    memset(&mouseInput, 0, sizeof INPUT);

    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.dwExtraInfo = HAPTIC_MOUSE_EVENT;

    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Make sure the coordinates are clamped in the range [0, MAX]
    float nx = hduClamp((float) pt.x / nScreenWidth, 0.0f, 1.0f);
    float ny = hduClamp((float) pt.y / nScreenHeight, 0.0f, 1.0f);

    // SendInput requires normalized mouse coordinates
    mouseInput.mi.dx = (LONG) (nx * 65535 + 0.5);
    mouseInput.mi.dy = (LONG) (ny * 65535 + 0.5);
    mouseInput.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
        
    int nRet = SendInput(1, &mouseInput, sizeof mouseInput);
    assert(nRet);
}

void mouseButtonDown(DWORD dwButtonMask)
{
    INPUT mouseInput;
    memset(&mouseInput, 0, sizeof INPUT);

    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.dwExtraInfo = HAPTIC_MOUSE_EVENT;
    mouseInput.mi.dwFlags = 0;

    dwButtonMask = composeButtonMask(dwButtonMask);

    if (dwButtonMask == 0)
        return;

    // Check if we need to simulate a left mouse button
    // Also ensure that the left mouse button hasn't already been depressed
    if ((dwButtonMask & LEFT_MOUSE_BUTTON) != 0)
    {
        mouseInput.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
    }

    // Check if we need to simulate a middle mouse button
    // Also ensure that the middle mouse button hasn't already been depressed
    if ((dwButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
    {
        mouseInput.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
    }

    // Check if we need to simulate a right mouse button
    // Also ensure that the right mouse button hasn't already been depressed
    if ((dwButtonMask & RIGHT_MOUSE_BUTTON) != 0)
    {
        mouseInput.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;        
    }

    int nRet = SendInput(1, &mouseInput, sizeof mouseInput);
    assert(nRet);
}

void mouseButtonUp(DWORD dwButtonMask)
{
    INPUT mouseInput;
    memset(&mouseInput, 0, sizeof INPUT);

    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.dwExtraInfo = HAPTIC_MOUSE_EVENT;
    mouseInput.mi.dwFlags = 0;

    // Check if the mouse buttons should be swapped
    if (GetSystemMetrics(SM_SWAPBUTTON))
    {        
        DWORD dwTempMask = 0;

        if ((dwButtonMask & LEFT_MOUSE_BUTTON) != 0)
        {
            dwTempMask |= RIGHT_MOUSE_BUTTON;
        }

        if ((dwButtonMask & RIGHT_MOUSE_BUTTON) != 0)
        {
            dwTempMask |= LEFT_MOUSE_BUTTON;
        }

        if (dwTempMask)
        {
            dwButtonMask = dwTempMask;
        }
    }

    // Check if we need to simulate a left mouse button
    // Also ensure that the left mouse button hasn't already been depressed
    if ((dwButtonMask & LEFT_MOUSE_BUTTON) != 0)
    {
        mouseInput.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
    }

    // Check if we need to simulate a middle mouse button
    // Also ensure that the middle mouse button hasn't already been depressed
    if ((dwButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
    {
        mouseInput.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
    }

    // Check if we need to simulate a right mouse button
    // Also ensure that the right mouse button hasn't already been depressed
    if ((dwButtonMask & RIGHT_MOUSE_BUTTON) != 0)
    {
        mouseInput.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;        
    }

    int nRet = SendInput(1, &mouseInput, sizeof mouseInput);
    assert(nRet);
}

// The following functions use PostMessage instead of SendInput
// in order to achieve synchronous posting the windows message queue

void mouseMove(HWND hWnd, DWORD dwButtonMask, const POINT &pt)
{
    UINT uMsg = WM_MOUSEMOVE;
    WPARAM wParam = composeMouseFlags(dwButtonMask);
    LPARAM lParam = composeMousePosition(pt);

    POINT screenPt = pt;
    ::ClientToScreen(hWnd, &screenPt);
    ::SetCursorPos(screenPt.x, screenPt.y);

    BOOL bSuccess = ::PostMessage(hWnd, uMsg, wParam, lParam);
    assert(bSuccess);
}

void mouseButtonDown(HWND hWnd, DWORD dwButtonMask, const POINT &pt)
{
    UINT uMsg;
    dwButtonMask = composeButtonMask(dwButtonMask);
    WPARAM wParam = composeMouseFlags(dwButtonMask);
    LPARAM lParam = composeMousePosition(pt);
       
    if (dwButtonMask == 0)
        return;

    // Check if we need to simulate a left mouse button
    // Also ensure that the left mouse button hasn't already been depressed
    if ((dwButtonMask & LEFT_MOUSE_BUTTON) != 0)
    {
        uMsg = WM_LBUTTONDOWN;
    }
    else if ((dwButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
    {
        uMsg = WM_MBUTTONDOWN;
    }
    else if ((dwButtonMask & RIGHT_MOUSE_BUTTON) != 0)
    {
        uMsg = WM_RBUTTONDOWN;
    }    

    BOOL bSuccess = ::PostMessage(hWnd, uMsg, wParam, lParam);
    assert(bSuccess);
}

void mouseButtonUp(HWND hWnd, DWORD dwButtonMask, const POINT &pt)
{
    UINT uMsg;
    dwButtonMask = composeButtonMask(dwButtonMask);
    WPARAM wParam = composeMouseFlags(dwButtonMask);
    LPARAM lParam = composeMousePosition(pt);
       
    if (dwButtonMask == 0)
        return;

    // Check if we need to simulate a left mouse button
    // Also ensure that the left mouse button hasn't already been depressed
    if ((dwButtonMask & LEFT_MOUSE_BUTTON) != 0)
    {
        uMsg = WM_LBUTTONUP;
    }
    else if ((dwButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
    {
        uMsg = WM_MBUTTONUP;
    }
    else if ((dwButtonMask & RIGHT_MOUSE_BUTTON) != 0)
    {
        uMsg = WM_RBUTTONUP;
    }    

    BOOL bSuccess = ::PostMessage(hWnd, uMsg, wParam, lParam);
    assert(bSuccess);
}

// Determine if the mouse is presently being captured. This call can be
// made outside of the GUI thread
bool isMouseCaptured(HWND hWnd, HWND &hCaptureWnd)
{
    bool bIsCaptured = false;

    DWORD processId = GetWindowThreadProcessId(hWnd, NULL);

    GUITHREADINFO threadInfo;
    threadInfo.cbSize = sizeof GUITHREADINFO;

    if (GetGUIThreadInfo(processId, &threadInfo))
    {
        bIsCaptured = bIsCaptured || (threadInfo.hwndCapture != 0);

        bIsCaptured = bIsCaptured || ((threadInfo.flags & 
            (GUI_INMENUMODE | GUI_INMOVESIZE |
            GUI_POPUPMENUMODE | GUI_SYSTEMMENUMODE)) != 0);

        hCaptureWnd = threadInfo.hwndCapture;
    }
    else
    {
        hCaptureWnd = NULL;
        //DisplayLastError("Failed to get GUITHREADINFO");
    }

    return bIsCaptured;
}


// Checks if the x, y screen coordinate location is 
bool isPointInWindow(HWND hWnd, const POINT &pt)
{    
    if (::WindowFromPoint(pt) == NULL)
    {
        return false;
    }

    HWND hRootOwner = ::GetAncestor(hWnd, GA_ROOTOWNER);
    if (hRootOwner == hWnd)
    {
        return true;
    }
    else
    {
        // Following test accepts drop down list boxes on dialogs 
        // and popup error dialogs which don't pass the above
        // GetAncestor test for some reason.
        POINT clientPt = { pt.x, pt.y };
        BOOL bNoError = ::ScreenToClient(hWnd, &clientPt);
        assert(bNoError);

        // Get the child window from this point
        HWND hChild = ::ChildWindowFromPoint(hWnd, clientPt);
        return hChild != NULL;
    }
}

// Checks if the HWND is a foreground window
bool isForegroundWindow(HWND hWnd)
{
    HWND hForeGround = ::GetForegroundWindow();
	if(hForeGround == hWnd)
		return true;

	HWND hRootOwner = ::GetAncestor(hWnd, GA_ROOTOWNER);
	if(hForeGround == hRootOwner)
		return true;
	return false;
}

// Windows allows us to temporarily hide the cursor when we know that we don't
// want it displayed. This is typically used when haptic mouse transitions
// from 2D cursor to 3D cursor
void hideMouseCursor()
{
    ::SetCursor(NULL);
}

} // namespace HapticMouse
