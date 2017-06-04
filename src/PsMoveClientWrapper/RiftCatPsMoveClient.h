// MODIFUED version of https://bitbucket.org/hawkinse/psmovefreepiebridge 

#pragma once
#include "../../thirdparty/headers/PSMoveService/PSMoveClient_CAPI.h"
#include <chrono>
#include <windows.h>
#include "PsMoveServiceResult.h"

class RiftCatPsMoveClient
{
public:
	RiftCatPsMoveClient();
	~RiftCatPsMoveClient();

	int run(int32_t controllerCount, PSMControllerID controllerIDs[], PSMTrackingColorType bulbColors[], int32_t freepieIndicies[], bool sendSensorData = true);

	void handle_client_psmove_event(PSMEventMessage::eEventType event_type);

	void handle_acquire_controller(PSMResult resultCode, PSMControllerID trackedControllerIndex);

	bool startup();

	PsMoveServiceResult^ update();

	void shutdown();
private:
	bool m_keepRunning = true;
	PSMController *controller_views[4] = { nullptr, nullptr, nullptr, nullptr };
	std::chrono::milliseconds last_report_fps_timestamp;
	PSMRequestID start_stream_request_ids[4] = { -1, -1, -1, -1 };
	PSMControllerID* trackedControllerIDs;
	int32_t* trackedFreepieIndicies;
	PSMTrackingColorType* trackedBulbColors;
	int32_t trackedControllerCount = 1;
	bool m_sendSensorData = true;

	void init_controller_views();
	void free_controller_views();

};

