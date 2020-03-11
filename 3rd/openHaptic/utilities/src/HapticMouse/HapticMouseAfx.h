/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  HapticMouseAfx.h

Description: 

  Precompiled header

*******************************************************************************/

#ifdef WIN32

// identifier was truncated to '255' characters in the debug information
#pragma warning( disable: 4786 )

// conversion from 'HDdouble' to 'const LONG', possible loss of data
#pragma warning( disable: 4244 )

#include <windows.h>

/*
The Winuser.h header file has a discrepancy in its use of the WINVER define.
The documentation for GetGUIThreadInfo and GetTitleBarInfo state that they
are supported under Windows NT, yet Winuser.h will only declare the
necessary symbols if WINVER >= 0x0500, which is indicative of Win98/Win2K
only code. I opened a Microsoft tech support case for this issue
(Case ID number: SRX010214601635). The tech support agent acknowledged the
discrepancy and suggested that I just redeclare the necessary symbols from
Winuser.h for WINVER < 0x0500.
*/

#if (WINVER < 0x0500)

#if !defined(_USER32_)
#define WINUSERAPI DECLSPEC_IMPORT
#else
#define WINUSERAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// These declarations are needed for GetGUIThreadInfo
typedef struct tagGUITHREADINFO
{
    DWORD   cbSize;
    DWORD   flags;
    HWND    hwndActive;
    HWND    hwndFocus;
    HWND    hwndCapture;
    HWND    hwndMenuOwner;
    HWND    hwndMoveSize;
    HWND    hwndCaret;
    RECT    rcCaret;
} GUITHREADINFO, *PGUITHREADINFO;

#define GUI_CARETBLINKING   0x00000001
#define GUI_INMOVESIZE      0x00000002
#define GUI_INMENUMODE      0x00000004
#define GUI_SYSTEMMENUMODE  0x00000008
#define GUI_POPUPMENUMODE   0x00000010

WINUSERAPI BOOL WINAPI
GetGUIThreadInfo(
    DWORD          idThread,
    PGUITHREADINFO pgui);

// These declarations are needed for GetTitleBarInfo
#define CCHILDREN_TITLEBAR              5

typedef struct tagTITLEBARINFO
{
    DWORD cbSize;
    RECT  rcTitleBar;
    DWORD rgstate[CCHILDREN_TITLEBAR+1];
} TITLEBARINFO, *PTITLEBARINFO, *LPTITLEBARINFO;

BOOL WINAPI
GetTitleBarInfo(
    HWND hwnd,
    PTITLEBARINFO pti
);

/*
 * The "real" ancestor window
 */
#define     GA_MIC          1
#define     GA_PARENT       1
#define     GA_ROOT         2
#define     GA_ROOTOWNER    3
#define     GA_MAC          4

HWND
WINAPI
GetAncestor(
    HWND hwnd,
    UINT gaFlags
);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // WINVER < 0x0500

#endif // WIN32

#include <GL/gl.h>
#include <HL/hl.h>

#include <HapticMouse/HapticMouse.h>

/*****************************************************************************/
