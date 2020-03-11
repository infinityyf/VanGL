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

#ifndef HapticMouse_H_
#define HapticMouse_H_

#include <windows.h>
#include <GL/gl.h>
#include <HL/hl.h>

#ifdef __cplusplus
extern "C" {
#endif

HLboolean hmInitializeMouse(HHLRC hHLRC, 
                            const char *pClassName, 
                            const char *pWindowName);

HLboolean hmInitializeMouseWindow(HHLRC hHLRC, HWND hWnd);

void hmShutdownMouse();

HLboolean hmIsMouseInitialized();
HLboolean hmIsMouseActive();

/* Haptic mouse capabilities */
typedef unsigned int HMenum;

/* Renders an ambient force effect while haptic mouse is active */
#define HM_MOUSE_AMBIENT_EFFECT         0x0001

/* Control whether haptic mouse is active while inside the viewport */
#define HM_MOUSE_INSIDE_VIEWPORT        0x0002

/* Control whether haptic mouse is active while outside the viewport */
#define HM_MOUSE_OUTSIDE_VIEWPORT       0x0003

/* Control whether to use the sync input mode for emulating mouse input
   This is useful for cases when mouse input and haptic input need to
   be processed in tandem. */
#define HM_MOUSE_SYNC_INPUT             0x0004

void hmEnableMouse(HMenum cap);
void hmDisableMouse(HMenum cap);
HLboolean hmIsMouseEnabled(HMenum cap);

void hmSetMouseWindow(HWND hWnd);

void hmSetMouseTransforms(const GLdouble modelMatrix[16],
                          const GLdouble projMatrix[16],
                          const GLint viewport[4]);

void hmProjectModelToScreen(const HLdouble modelPt[3],                            
                            HLint screenPt[2]);

HHLRC hmGetMouseContext();
HWND hmGetMouseWindow();

void hmRenderMouseScene();

#ifdef __cplusplus
}
#endif

#endif // HapticMouse_H_

