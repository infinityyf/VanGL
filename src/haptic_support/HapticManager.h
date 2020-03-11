#pragma once
#ifndef HAPRICMANAGER
#define HAPRICMANAGER
#include <Windows.h>
#include <cstdio>
#include <cassert>

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>
#include <vector>
#include <set>
#include <time.h>
#include <iostream>
#include "HapticTool.h"

HDCallbackCode HDCALLBACK ConfigurationOptimCallback(void* data);

static HapticTools*		currentTool = NULL;			//haptic tool pointer
HANDLE					g_mutex;					//thread sync
static HHD				hHD;
HDSchedulerHandle		hSphereCallback;			//callback handle
static int				framecount = 0;
static float			hapticScale = 0.01f;			//scale of coord scal
static float			force[3];
static float			m_graphicTrans[16];			//graphic tool trans
static int				ms;

int		HapticInitPhantom();						//init haptic loop
void	StopHapticLoopPhantom();					//stop haptic loop
void	GetProxyToolTrans(float* t);				//copy m_graphicTrans to t
void	GetCurrentForce(float* f);					//copy force to f
float	GetScale();
int		getms();


int getms() {
	return ms;
}

float GetScale() {
	return hapticScale;
}
int HapticInitPhantom()
{
	currentTool = new HapticTools();

	hHD = hdInitDevice(HD_DEFAULT_DEVICE);		// Initialize the default haptic device.
	hdEnable(HD_FORCE_OUTPUT);					// Start the servo scheduler and enable forces.

	hdStartScheduler();

	// Application loop - schedule our call to the main callback.
	hSphereCallback = hdScheduleAsynchronous(ConfigurationOptimCallback, 0, HD_DEFAULT_SCHEDULER_PRIORITY);

	//printf("Press ESC to quit.\n\n");
	g_mutex = CreateMutex(NULL, FALSE, "mutex_for_all_particals");
	ReleaseMutex(g_mutex);
	return 1;
}

void StopHapticLoopPhantom()
{
	hdStopScheduler();
	hdUnschedule(hSphereCallback);
	hdDisableDevice(hHD);
	return;
}

HDCallbackCode HDCALLBACK ConfigurationOptimCallback(void* data)
{
	if (framecount < 1000) {
		framecount++;
		return HD_CALLBACK_CONTINUE;
	}
	clock_t start = clock();									//start counting
	hdBeginFrame(hdGetCurrentDevice());
	float device_stateR[16];
	hdGetFloatv(HD_CURRENT_TRANSFORM, device_stateR);
	device_stateR[12] *= hapticScale;							//get device position
	device_stateR[13] *= hapticScale;
	device_stateR[14] *= hapticScale;

	currentTool->ForceBefore(device_stateR);					//将位置信息转存到m_trans和m_rotate中，并设置m_qH（用于计算约束								
	currentTool->ForceCompute(force);							//计算碰撞，求解约，计算受力
	currentTool->ForceAfter(m_graphicTrans);					//记录上一次的移动和旋转


	//sent the force out
	hdMakeCurrentDevice(hHD);
	//hdSetFloatv(HD_CURRENT_FORCE, force);
	hdEndFrame(hdGetCurrentDevice());
	ms = clock() - start;
	return HD_CALLBACK_CONTINUE;
}
//获取优化后工具的位姿
void GetProxyToolTrans(float* t) {
	memcpy(t, m_graphicTrans, 16 * sizeof(float));
}

void GetCurrentForce(float* f) {
	f[0] = force[0];
	f[1] = force[1];
	f[2] = force[2];
}


#endif // !HAPRICMANAGER





