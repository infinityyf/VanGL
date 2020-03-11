/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  HapticMouse.cpp

Description: 

  Emulates mouse input based on the motion and button presses of the haptic
  device. Also interprets the windows environment and generates haptic
  constraints and force effects during motion.

******************************************************************************/

#include "HapticMouseAfx.h"
#include "HapticMouseManager.h"
#include "HapticMouseUtil.h"
#include "Win32MouseEmulation.h"

#include <HDU/hduMath.h>
#include <HLU/hlu.h>

#include <assert.h>

namespace HapticMouse
{

static HapticMouseManager *gManager = 0;

// The mouse is typically controlled as a relative motion device. However,
// we control it with absolute motion input. This threshold allows us to
// avoid fighting between relative motion input and our absolute motion input 
const int HapticMouseManager::m_kAbsoluteMotionThreshold = 20;

/******************************************************************************

******************************************************************************/
HapticMouseManager *HapticMouseManager::instance()
{
    if(!gManager)
    {
        gManager = new HapticMouseManager;
    }

    return gManager;
}

/******************************************************************************

******************************************************************************/
HapticMouseManager::HapticMouseManager() : 
    m_bIsInitialized(false),
    m_hWnd(NULL),
    m_hHLRC(NULL),
    m_bIsActive(false),
    m_effectId(0),
    m_bUseAmbientEffect(true),
    m_bUseInsideViewport(false),
    m_bUseOutsideViewport(true),
    m_bSyncMouseInput(false),
    m_lastButtonMask(0)
{
}

/******************************************************************************

******************************************************************************/
HapticMouseManager::~HapticMouseManager()
{
    if (isInitialized())
        shutdown();
}

/******************************************************************************

******************************************************************************/
bool HapticMouseManager::initialize(HHLRC hHLRC, 
                                    const char *pClassName, 
                                    const char *pWindowName)
{
    SafeLock lock(m_lock);

    // Don't proceed if the haptic mouse is already active
    if (isInitialized())
        return false;

    if (!setWindow(pClassName, pWindowName))
        return false;

    if (!setContext(hHLRC))
        return false;

    HLContextScope ctx(m_hHLRC);

    HLboolean b1Down, b2Down;
    hlGetBooleanv(HL_BUTTON1_STATE, &b1Down);
    hlGetBooleanv(HL_BUTTON2_STATE, &b2Down);
    m_lastButtonMask |= b1Down ? LEFT_MOUSE_BUTTON : 0;
    m_lastButtonMask |= b2Down ? RIGHT_MOUSE_BUTTON : 0;

    // Register for motion events and set the force effect
    hlAddEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY, 
                       HL_COLLISION_THREAD, motionEventCB, this);
    hlAddEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY, 
                       HL_CLIENT_THREAD, motionEventCB, this);
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY, 
                       HL_COLLISION_THREAD, buttonEventCB, this);
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, 
                       HL_COLLISION_THREAD, buttonEventCB, this);
    hlAddEventCallback(HL_EVENT_2BUTTONDOWN, HL_OBJECT_ANY,
                       HL_COLLISION_THREAD, buttonEventCB, this);
    hlAddEventCallback(HL_EVENT_2BUTTONUP, HL_OBJECT_ANY,
                       HL_COLLISION_THREAD, buttonEventCB, this);

    m_bIsInitialized = true; 
    m_bIsActive = false;
    
    return true;
}

bool HapticMouseManager::initialize(HHLRC hHLRC, 
                                    HWND hWnd)
{
    SafeLock lock(m_lock);

    // Don't proceed if the haptic mouse is already active
    if (isInitialized())
        return false;

    if (!setWindow(hWnd))
        return false;

    if (!setContext(hHLRC))
        return false;

    HLContextScope ctx(m_hHLRC);

    // Register for motion events and set the force effect
    hlAddEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY, 
                       HL_COLLISION_THREAD, motionEventCB, this);
    hlAddEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY, 
                       HL_CLIENT_THREAD, motionEventCB, this);
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY, 
                       HL_COLLISION_THREAD, buttonEventCB, this);
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, 
                       HL_COLLISION_THREAD, buttonEventCB, this);
    hlAddEventCallback(HL_EVENT_2BUTTONDOWN, HL_OBJECT_ANY,
                       HL_COLLISION_THREAD, buttonEventCB, this);
    hlAddEventCallback(HL_EVENT_2BUTTONUP, HL_OBJECT_ANY,
                       HL_COLLISION_THREAD, buttonEventCB, this);

    m_bIsInitialized = true; 
    m_bIsActive = false;
    
    return true;
}
/******************************************************************************

******************************************************************************/
void HapticMouseManager::shutdown()
{
    SafeLock lock(m_lock);

    // Don't proceed if the haptic mouse is not active
    if (!isInitialized())
        return;

    HLContextScope ctx(m_hHLRC);

    // Register for motion events and set the force effect
    hlRemoveEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY, 
                          HL_COLLISION_THREAD, motionEventCB);
    hlRemoveEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY, 
                          HL_CLIENT_THREAD, motionEventCB);
    hlRemoveEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY,
                          HL_COLLISION_THREAD, buttonEventCB);
    hlRemoveEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY,
                          HL_COLLISION_THREAD, buttonEventCB);
    hlRemoveEventCallback(HL_EVENT_2BUTTONDOWN, HL_OBJECT_ANY,
                          HL_COLLISION_THREAD, buttonEventCB);
    hlRemoveEventCallback(HL_EVENT_2BUTTONUP, HL_OBJECT_ANY,
                          HL_COLLISION_THREAD, buttonEventCB);

    // Cleanup the effect id
    if (hlIsEffect(m_effectId))
    {
        hlDeleteEffects(m_effectId, 1);
        m_effectId = 0;    
    }

    m_bIsInitialized = false;
    m_bIsActive = false;
}

/******************************************************************************

******************************************************************************/
bool HapticMouseManager::setWindow(HWND hWnd)
{
    SafeLock lock(m_lock);

    if (m_hWnd != hWnd)
    {    
        if (m_hWnd != NULL)
        {
            // if we swap windows while the button is down
            // we will miss the button up so force a button
            // up here
            forceButtonRelease();
        }

        m_hWnd = hWnd;

        updateViewportToScreen();
    }

    return true;
}

/******************************************************************************

******************************************************************************/
bool HapticMouseManager::setWindow(
    const char *pClassName, const char *pWindowName)
{
    HWND hWnd = FindWindow(pClassName, pWindowName);
    if (hWnd != NULL)
    {
        return setWindow(hWnd);
    }

    return false;
}

/******************************************************************************

******************************************************************************/
bool HapticMouseManager::setContext(HHLRC hHLRC)
{
    if (hHLRC != NULL)
    {
        m_hHLRC = hHLRC;
        return true;
    }

    return false;
}

/******************************************************************************

******************************************************************************/
void HapticMouseManager::setTransforms(
    const GLdouble modelMatrix[16], 
    const GLdouble projMatrix[16], 
    const GLint viewport[4])
{
    SafeLock lock(m_lock);

    memcpy(m_modelMatrix, modelMatrix, sizeof(GLdouble) * 16);
    memcpy(m_projMatrix, projMatrix, sizeof(GLdouble) * 16);
    memcpy(m_viewport, viewport, sizeof(GLint) * 4);
    
    // Update the model to window transform that we'll use in computing the
    // position of the mouse cursor on the screen
    hluModelToWindowTransform(
        modelMatrix, projMatrix, viewport,
        m_modelToViewport);

    // Also need to update the window to screen transform, since the
    // viewport dimensions may have changed
    updateViewportToScreen();
}

/******************************************************************************

******************************************************************************/
void HapticMouseManager::projectModelToScreen(const HLdouble modelPt[3],
                                              POINT &portPt, POINT &screenPt)
{
    hduVector3Dd cursorPosModel(modelPt[0], modelPt[1], modelPt[2]);

    // Project the a point from model coordinates onto the window
    hduVector3Dd cursorPosPort = cursorPosModel * m_modelToViewport;
    hduVector3Dd cursorPosScreen = cursorPosPort * m_viewportToScreen;
        
    portPt.x = hduRound<LONG>(cursorPosPort[0]);
    portPt.y = hduRound<LONG>(cursorPosPort[1]);    

    // Clamp to the result to the dimensions of the screen
    const int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    screenPt.x = hduClamp<LONG>(
        hduRound<LONG>(cursorPosScreen[0]), 0, nScreenWidth - 1);
    screenPt.y = hduClamp<LONG>(
        hduRound<LONG>(cursorPosScreen[1]), 0, nScreenHeight - 1);    
}

/******************************************************************************

******************************************************************************/
void HapticMouseManager::enable(HMenum cap)
{
    SafeLock lock(m_lock);

    switch (cap)
    {
        case HM_MOUSE_AMBIENT_EFFECT: 
            m_bUseAmbientEffect = true;
            break;

        case HM_MOUSE_INSIDE_VIEWPORT:
            m_bUseInsideViewport = true;
            break;

        case HM_MOUSE_OUTSIDE_VIEWPORT:
            m_bUseOutsideViewport = true;
            break;

        case HM_MOUSE_SYNC_INPUT:
            m_bSyncMouseInput = true;
            break;
    }
}

/******************************************************************************

******************************************************************************/
void HapticMouseManager::disable(HMenum cap)
{
    SafeLock lock(m_lock);

    switch (cap)
    {
        case HM_MOUSE_AMBIENT_EFFECT: 
            m_bUseAmbientEffect = false;
            break;

        case HM_MOUSE_INSIDE_VIEWPORT:
            m_bUseInsideViewport = false;
            break;

        case HM_MOUSE_OUTSIDE_VIEWPORT:
            m_bUseOutsideViewport = false;
            break;

        case HM_MOUSE_SYNC_INPUT:
            m_bSyncMouseInput = false;;
            break;
    }    
}

/******************************************************************************

******************************************************************************/
bool HapticMouseManager::isEnabled(HMenum cap)
{
    SafeLock lock(m_lock);

    switch (cap)
    {
        case HM_MOUSE_AMBIENT_EFFECT: return m_bUseAmbientEffect;
        case HM_MOUSE_INSIDE_VIEWPORT: return m_bUseInsideViewport;
        case HM_MOUSE_OUTSIDE_VIEWPORT: return m_bUseOutsideViewport;
        case HM_MOUSE_SYNC_INPUT: return m_bSyncMouseInput;
    }

    return false;
}

/******************************************************************************

******************************************************************************/
void HapticMouseManager::render()
{
    SafeLock lock(m_lock);

    HLContextScope ctx(m_hHLRC);

    // Check if we can update the active state based just on the
    // inside/outside state
    if (m_bUseInsideViewport && m_bUseOutsideViewport)
    {
        updateActiveState(true);
    }
    else if (!m_bUseInsideViewport && !m_bUseOutsideViewport)
    {
        updateActiveState(false);
    }    

    // Check if an effect needs to be started or stopped
    bool bEffectStarted = hlIsEffect(m_effectId) == HL_TRUE;
    bool bEffectRequested = m_bIsActive && m_bUseAmbientEffect;

    if (!bEffectStarted && bEffectRequested)
    {
        assert(m_effectId == 0);
        m_effectId = hlGenEffects(1);
        
        hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.3);
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.1);
        hlStartEffect(HL_EFFECT_FRICTION, m_effectId);
    }
    else if (bEffectStarted && !bEffectRequested)
    {
        hlStopEffect(m_effectId);
        hlDeleteEffects(m_effectId, 1);
        m_effectId = 0;
    }
}

/******************************************************************************

******************************************************************************/
void HapticMouseManager::updateActiveState(const POINT &mousePtPort,
                                           const POINT &mousePtScreen)
{
    // Don't change the mouse emulation state while the mouse is captured
    HWND hCaptureWnd = NULL;
    if (isMouseCaptured(m_hWnd, hCaptureWnd))
        return;

    // Don't change the mouse emulation state while the window is inactive
    if (!isForegroundWindow(m_hWnd))
        return;

    bool bInsideWindow = isPointInWindow(m_hWnd, mousePtScreen);

    // Check if the cursor position is inside or outside of the viewport
    bool bInsideViewport =         
        bInsideWindow &&
        mousePtPort.x >= m_viewport[0] &&
        mousePtPort.x < m_viewport[0] + m_viewport[2] &&
        mousePtPort.y >= m_viewport[1] &&
        mousePtPort.y < m_viewport[1] + m_viewport[3];
    
    // Don't proceed if mouse emulation is not allowed for the cursor position
    bool bAllowMouseActive = m_bUseInsideViewport && bInsideViewport ||
                             m_bUseOutsideViewport && !bInsideViewport;                                

    // Call through to a secondary routine that maintains the activate
    // and deactivate flags
    updateActiveState(bAllowMouseActive);
}


/******************************************************************************

******************************************************************************/
void HapticMouseManager::updateActiveState(bool bAllowMouseActive)
{
    // Check if a transition has occurred that requires starting or stopping
    // mouse emulation
    if (bAllowMouseActive && !m_bIsActive)
    {
        updateViewportToScreen();
    
        // Update the last mouse X & Y to be at the current cursor location
        ::GetCursorPos(&m_lastMousePt);

        m_bIsActive = true;
    }
    else if (!bAllowMouseActive && m_bIsActive)
    {
        // Ensure that any depressed buttons are released
        forceButtonRelease();
    
        hideMouseCursor();

        m_bIsActive = false;
    }
}


/******************************************************************************

******************************************************************************/
void HLCALLBACK HapticMouseManager::motionEventCB(
    HLenum event, HLuint object, HLenum thread,
    HLcache *cache, void *userdata)
{
    HapticMouseManager *pThis = static_cast<HapticMouseManager *>(userdata);

    SafeLock lock(pThis->m_lock);

    // Get the proxy position from the cache
    hduVector3Dd cursorPosModel;
    hlCacheGetDoublev(cache, HL_PROXY_POSITION, cursorPosModel);       

    POINT mousePtPort;
    POINT mousePtScreen;
    pThis->projectModelToScreen(cursorPosModel, mousePtPort, mousePtScreen);

    if (thread == HL_CLIENT_THREAD)
    {
        // Check if we can update the active state based just on the
        // inside/outside state
        if (pThis->m_bUseInsideViewport && pThis->m_bUseOutsideViewport)
        {
            pThis->updateActiveState(true);
        }
        else if (!pThis->m_bUseInsideViewport && !pThis->m_bUseOutsideViewport)
        {
            pThis->updateActiveState(false);
        }    
        else
        {
            // Check if we are allowed to submit mouse emulation input
            pThis->updateActiveState(mousePtPort, mousePtScreen);
        }
    }
    else if (thread == HL_COLLISION_THREAD && pThis->isActive())
    {        
        // Send the mouse emulation input
        int kMotionThreshold = 1;

        POINT pt;
        BOOL bNoError = ::GetCursorPos(&pt);
        assert(bNoError);

        // Check if the cursor has moved away from the last commanded position
        if (hduAbsValue(pThis->m_lastMousePt.x - pt.x) > kMotionThreshold ||
            hduAbsValue(pThis->m_lastMousePt.y - pt.y) > kMotionThreshold)
        {
            // Use a larger change threshold, since the cursor is obviously
            // being controlled by another input device
            kMotionThreshold = m_kAbsoluteMotionThreshold;
        }

        // Check if the newly commanded position is greater than the motion
        // threshold away from the last commanded position
        if (hduAbsValue(pThis->m_lastMousePt.x - mousePtScreen.x) > kMotionThreshold ||
            hduAbsValue(pThis->m_lastMousePt.y - mousePtScreen.y) > kMotionThreshold)
        {
            if (pThis->m_bSyncMouseInput && pThis->allowSyncMouseInput())
            {
                // Need to transform the screen coordinate back into window client coords
                POINT mousePtWin = mousePtScreen;
                ::ScreenToClient(pThis->m_hWnd, &mousePtWin);

                mouseMove(pThis->m_hWnd, pThis->m_lastButtonMask, mousePtWin);          
            }
            else
            {
                mouseMove(mousePtScreen);                 
            }

            pThis->m_lastMousePt = mousePtScreen;
        }
    }
}


/******************************************************************************

******************************************************************************/
void HLCALLBACK HapticMouseManager::buttonEventCB(
    HLenum event, HLuint object, HLenum thread,
    HLcache *cache, void *userdata)
{
    HapticMouseManager *pThis = static_cast<HapticMouseManager *>(userdata);

    SafeLock lock(pThis->m_lock);

    // Don't proceed if mouse emulation is not allowed
    if (!pThis->isActive())
        return;

    BOOL bButtonDown = false;
    DWORD dwButtonMask = 0;

    // Send the mouse emulation input
    if (event == HL_EVENT_1BUTTONDOWN)
    {
        dwButtonMask = LEFT_MOUSE_BUTTON;
        bButtonDown = true;
    }
    else if (event == HL_EVENT_1BUTTONUP)
    {
        dwButtonMask = LEFT_MOUSE_BUTTON;
        bButtonDown = false;
    }
    else if (event == HL_EVENT_2BUTTONDOWN)
    {
        // Check if the user is trying to emulate a middle button click
        if (::GetAsyncKeyState(VK_MENU) & 0x8000)
        {
            //dwButtonMask = MIDDLE_MOUSE_BUTTON;
            //bButtonDown = true;

            // Ignore this button message when ALT is depressed to fix a
            // bug with quick-view manipulations with 3ds max
            return;
        }
        else
        {
            dwButtonMask = RIGHT_MOUSE_BUTTON;
            bButtonDown = true;            
        }
    }
    else if (event == HL_EVENT_2BUTTONUP)
    {
        if ((pThis->m_lastButtonMask & RIGHT_MOUSE_BUTTON) != 0)
        {
            dwButtonMask = RIGHT_MOUSE_BUTTON;
            bButtonDown = false;
        }
        else if ((pThis->m_lastButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
        {
            dwButtonMask = MIDDLE_MOUSE_BUTTON;
            bButtonDown = false;
        }
    }
        
    if (pThis->m_bSyncMouseInput && pThis->allowSyncMouseInput())
    {
        // Get the proxy position from the cache
        hduVector3Dd cursorPosModel;
        hlCacheGetDoublev(cache, HL_PROXY_POSITION, cursorPosModel);       
        
        POINT mousePtPort;
        POINT mousePtScreen;
        pThis->projectModelToScreen(cursorPosModel, mousePtPort, mousePtScreen);
        
        // Need to transform the screen coordinate back into window client coords
        POINT mousePtWin = mousePtScreen;
        ::ScreenToClient(pThis->m_hWnd, &mousePtWin);

        if (bButtonDown)
        {
            pThis->m_lastButtonMask |= dwButtonMask;
            mouseButtonDown(pThis->m_hWnd, dwButtonMask, mousePtWin);
        }
        else
        {
            pThis->m_lastButtonMask &= ~dwButtonMask;
            mouseButtonUp(pThis->m_hWnd, dwButtonMask, mousePtWin);
        }
    }
    else
    {
        if (bButtonDown)
        {
            pThis->m_lastButtonMask |= dwButtonMask;
            mouseButtonDown(dwButtonMask);
        }
        else
        {
            pThis->m_lastButtonMask &= ~dwButtonMask;
            mouseButtonUp(dwButtonMask);
        }
    }

}

bool HapticMouseManager::allowSyncMouseInput() const
{
    HWND hCaptureWnd = NULL;
    
    if (isMouseCaptured(m_hWnd, hCaptureWnd))
    {
        return (hCaptureWnd == m_hWnd);
    }
    else
    {
        return true;
    }    
}

// Call this when making a change in the HapticMouse state to
// release any commanded button presses.
void HapticMouseManager::forceButtonRelease()
{
    // if we deactivate the mouse in the button down state, force the
    // mouse to release those buttons, and therefore abort whatever
    // client operation is in progress

    if (m_bSyncMouseInput && allowSyncMouseInput())
    {
        // Get the proxy position from the cache
        hduVector3Dd cursorPosModel;
        hlGetDoublev(HL_PROXY_POSITION, cursorPosModel);       
        
        POINT mousePtPort;
        POINT mousePtScreen;
        projectModelToScreen(cursorPosModel, mousePtPort, mousePtScreen);
        
        // Need to transform the screen coordinate back into window client coords
        POINT mousePtWin = mousePtScreen;
        ::ScreenToClient(m_hWnd, &mousePtWin);

        if ((m_lastButtonMask & LEFT_MOUSE_BUTTON) != 0)
        {
            m_lastButtonMask &= ~LEFT_MOUSE_BUTTON;
            mouseButtonUp(m_hWnd, LEFT_MOUSE_BUTTON, mousePtWin);            
        }

        if ((m_lastButtonMask & RIGHT_MOUSE_BUTTON) != 0)
        {
            m_lastButtonMask &= ~RIGHT_MOUSE_BUTTON;
            mouseButtonUp(m_hWnd, RIGHT_MOUSE_BUTTON, mousePtWin);            
        }

        if ((m_lastButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
        {
            m_lastButtonMask &= ~MIDDLE_MOUSE_BUTTON;
            mouseButtonUp(m_hWnd, MIDDLE_MOUSE_BUTTON, mousePtWin);
        }
    }
    else
    {
        if ((m_lastButtonMask & LEFT_MOUSE_BUTTON) != 0)
        {
            m_lastButtonMask &= ~LEFT_MOUSE_BUTTON;
            mouseButtonUp(LEFT_MOUSE_BUTTON);            
        }

        if ((m_lastButtonMask & RIGHT_MOUSE_BUTTON) != 0)
        {
            m_lastButtonMask &= ~RIGHT_MOUSE_BUTTON;
            mouseButtonUp(RIGHT_MOUSE_BUTTON);            
        }

        if ((m_lastButtonMask & MIDDLE_MOUSE_BUTTON) != 0)
        {
            m_lastButtonMask &= ~MIDDLE_MOUSE_BUTTON;
            mouseButtonUp(MIDDLE_MOUSE_BUTTON);
        }
    }
    
}

void HapticMouseManager::updateViewportToScreen()
{
    // Determine the origin of the window in screen coordinates
    POINT origin = { 0, 0 };
    ClientToScreen(m_hWnd, &origin);

    // The relationship between viewport and screen is a translation
    m_viewportToScreen = hduMatrix::createScale(hduVector3Dd(1, -1, 1)) *
                         hduMatrix::createTranslation(hduVector3Dd(
                         origin.x, origin.y + m_viewport[3], 0));
}

} // namespace HapticMouse
