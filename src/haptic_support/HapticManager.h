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

int		HapticInitPhantom(DynamicWorld* world);						//init haptic loop
void	StopHapticLoopPhantom();					//stop haptic loop
void	GetCurrentForce(float* f);					//copy force to f
float	GetScale();
int		getms();


int getms() {
	return ms;
}

float GetScale() {
	return hapticScale;
}
int HapticInitPhantom(DynamicWorld *world)
{
	currentTool = new HapticTools();
	currentTool->world = world;
	hHD = hdInitDevice(HD_DEFAULT_DEVICE);		// Initialize the default haptic device.
	hdEnable(HD_FORCE_OUTPUT);					// Start the servo scheduler and enable forces.

	hdStartScheduler();
	//hSphereCallback = 
	hdScheduleSynchronous(ConfigurationOptimCallback, 0, HD_MAX_SCHEDULER_PRIORITY);

	//printf("Press ESC to quit.\n\n");
	g_mutex = CreateMutex(NULL, FALSE, "mutex_for_all_particals");
	ReleaseMutex(g_mutex);
	return 1;
}

void StopHapticLoopPhantom()
{
	hdStopScheduler();
	//hdUnschedule(hSphereCallback);
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

	currentTool->ForceBefore(device_stateR);					//��λ����Ϣת�浽m_trans��m_rotate�У�������m_qH�����ڼ���Լ��								
	if(currentTool->world != nullptr){
		//std::cout << "dynamic"<< std::endl;
		currentTool->ForceCompute(force);		//������ײ�����Լ����������
	}
	//currentTool->world->updatePyhsics();

	
	//sent the force out
	hdMakeCurrentDevice(hHD);
	//hdSetFloatv(HD_CURRENT_FORCE, force);
	hdEndFrame(hdGetCurrentDevice());
	ms = clock() - start;
	return HD_CALLBACK_CONTINUE;
}

void GetCurrentForce(float* f) {
	f[0] = force[0];
	f[1] = force[1];
	f[2] = force[2];
}



#endif // !HAPRICMANAGER





