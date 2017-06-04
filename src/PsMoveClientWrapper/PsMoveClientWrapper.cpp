// This is the main DLL file.
#pragma once
#include "stdafx.h"
#include "../PSMoveFreepieBridge/FreepieMoveClient.h"

#include "PsMoveClientWrapper.h"



PsMoveClientWrapper::PsMoveServiceClient::PsMoveServiceClient()
{
	freepieMoveClient = new FreepieMoveClient();
}

int PsMoveClientWrapper::PsMoveServiceClient::Run()
{
	int32_t controllerCount = 1;
	PSMControllerID controllerIDs[4];
	int32_t freepieIndicies[4] = { 0, 1, 2, 3 };
	PSMTrackingColorType bulbColors[4] = { PSMTrackingColorType_MaxColorTypes, PSMTrackingColorType_MaxColorTypes, PSMTrackingColorType_MaxColorTypes, PSMTrackingColorType_MaxColorTypes };
	bool bRun = true;
	bool bExitWithPSMoveService = false;

	freepieMoveClient->run(controllerCount, controllerIDs, bulbColors, freepieIndicies, true);
	return 0;
}



