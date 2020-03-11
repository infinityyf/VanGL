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

#ifndef HapticMouseManager_H_
#define HapticMouseManager_H_

#include <HDU/hduMatrix.h>
#include "CriticalSectionLock.h"

namespace HapticMouse
{

class HapticMouseManager
{
public:

    static HapticMouseManager *instance();

    HapticMouseManager();
    ~HapticMouseManager();

    bool initialize(HHLRC hHLRC, 
                    const char *pClassName, 
                    const char *pWindowName);

    bool initialize(HHLRC hHLRC, HWND hWnd);

    void shutdown();

    bool isInitialized() const { return m_bIsInitialized; }
    bool isActive() const { return m_bIsActive; }

    HWND getWindow() const { return m_hWnd; }
    HHLRC getContext() const { return m_hHLRC; }

    void setTransforms(
        const GLdouble modelMatrix[16], 
        const GLdouble projMatrix[16], 
        const GLint viewport[4]);

    void projectModelToScreen(const HLdouble modelPt[3], POINT &portPt, POINT &screenPt);

    void render();

    void enable(HMenum cap);
    void disable(HMenum cap);
    bool isEnabled(HMenum cap);

    bool setWindow(HWND hWnd);

protected:

    bool setWindow(const char *pClassName, const char *pWindowName);

    bool setContext(HHLRC hHLRC);

    void updateViewportToScreen();

private:

    void updateActiveState(const POINT &mousePtPort, 
                           const POINT &mousePtScreen);
    void updateActiveState(bool bAllowMouseActive);

    static void HLCALLBACK motionEventCB(
        HLenum event, HLuint object, HLenum thread,
        HLcache *cache, void *userdata);

    static void HLCALLBACK buttonEventCB(
        HLenum event, HLuint object, HLenum thread,
        HLcache *cache, void *userdata);

private:

    bool allowSyncMouseInput() const;

    void forceButtonRelease();

    static const int m_kAbsoluteMotionThreshold;

    HWND m_hWnd;
    bool m_bIsInitialized;

    HHLRC m_hHLRC;

    CriticalSectionLock m_lock;

    GLdouble m_modelMatrix[16];
    GLdouble m_projMatrix[16];
    GLint m_viewport[4];

    hduMatrix m_modelToViewport;
    hduMatrix m_viewportToScreen;

    POINT m_lastMousePt;
    DWORD m_lastButtonMask;

    HLuint m_effectId;

    bool m_bIsActive;

    bool m_bUseAmbientEffect;
    bool m_bUseInsideViewport;
    bool m_bUseOutsideViewport;
    bool m_bSyncMouseInput;    
};

} // namespace HapticMouse

#endif // HapticMouseManager_H_
