/*
* Edited from test_console_client.cpp in PSMoveService
*/

#include "stdafx.h"
//#include "../thirdparty/headers/FreePIE/freepie_io.h"
#include "../../thirdparty/headers/glm/glm.hpp"
#include "../../thirdparty/headers/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/headers/glm/gtc/quaternion.hpp"
#include "../../thirdparty/headers/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/headers/glm/gtx/euler_angles.hpp"

#include "RiftCatPsMoveClient.h"
#include "PsMoveServiceResult.h"

RiftCatPsMoveClient::RiftCatPsMoveClient()
{
}


typedef struct freepie_io_6dof_data
{
	float yaw;
	float pitch;
	float roll;

	float x;
	float y;
	float z;
} freepie_io_6dof_data;


int RiftCatPsMoveClient::run(int32_t controllerCount, PSMControllerID controllerIDs[], PSMTrackingColorType bulbColors[], int32_t freepieIndicies[], bool sendSensorData)
{
	// Attempt to start and run the client
	try
	{


		if (startup())
		{
			while (m_keepRunning)
			{
				update();

				Sleep(1);
			}
		}
		else
		{
			std::cerr << "Failed to startup the Freepie Move Client" << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	// Attempt to shutdown the client
	try
	{
		shutdown();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

void RiftCatPsMoveClient::handle_client_psmove_event(PSMEventMessage::eEventType event_type)
{
	switch (event_type)
	{
	case PSMEventMessage::PSMEvent_connectedToService:
		std::cout << "FreepieMoveClient - Connected to service" << std::endl;
		init_controller_views();
		break;
	case PSMEventMessage::PSMEvent_failedToConnectToService:
		std::cout << "FreepieMoveClient - Failed to connect to service" << std::endl;
		m_keepRunning = false;
		break;
	case PSMEventMessage::PSMEvent_disconnectedFromService:
		std::cout << "FreepieMoveClient - Disconnected from service" << std::endl;
		m_keepRunning = false;
		break;
	case PSMEventMessage::PSMEvent_opaqueServiceEvent:
		std::cout << "FreepieMoveClient - Opaque service event(%d). Ignored." << static_cast<int>(event_type) << std::endl;
		break;
	case PSMEventMessage::PSMEvent_controllerListUpdated:
		std::cout << "FreepieMoveClient - Controller list updated. Reinitializing controller views." << std::endl;
		free_controller_views();
		init_controller_views();
		break;
	case PSMEventMessage::PSMEvent_trackerListUpdated:
		std::cout << "FreepieMoveClient - Tracker list updated. Ignored." << std::endl;
		break;
	case PSMEventMessage::PSMEvent_hmdListUpdated:
		std::cout << "FreepieMoveClient - HMD list updated. Ignored." << std::endl;
		break;
	case PSMEventMessage::PSMEvent_systemButtonPressed:
		std::cout << "FreepieMoveClient - System button pressed. Ignored." << std::endl;
		break;
	default:
		std::cout << "FreepieMoveClient - unhandled event(%d)" << static_cast<int>(event_type) << std::endl;
		break;
	}
}

void RiftCatPsMoveClient::handle_acquire_controller(PSMResult resultCode, PSMControllerID trackedControllerIndex)
{
	if (resultCode == PSMResult_Success)
	{
		std::cout << "FreepieMoveClient - Acquired controller "
			<< controller_views[trackedControllerIndex]->ControllerID << std::endl;
	}
	else
	{
		std::cout << "FreepieMoveClient - failed to acquire controller " << std::endl;
		//m_keepRunning = false;
	}
}

bool RiftCatPsMoveClient::startup()
{
	bool success = true;

	trackedControllerCount = 1;
	trackedControllerIDs = new PSMControllerID[4]{ 0, -1, -1, -1 };
	trackedFreepieIndicies =  new int32_t[4]{ 0, 1, 2, 3 };
	trackedBulbColors = new PSMTrackingColorType[4]{ PSMTrackingColorType_MaxColorTypes, PSMTrackingColorType_MaxColorTypes, PSMTrackingColorType_MaxColorTypes, PSMTrackingColorType_MaxColorTypes };
	m_sendSensorData = false;

	bool bRun = true;
	bool bExitWithPSMoveService = false;

	// Attempt to connect to the server
	if (success)
	{
		if (PSM_InitializeAsync("localhost", "9512") == PSMResult_Error)
		{
			std::cout << "FreepieMoveClient - Failed to initialize the client network manager" << std::endl;
			success = false;
		}
	}

	if (success)
	{
		last_report_fps_timestamp =
			std::chrono::duration_cast< std::chrono::milliseconds >(
				std::chrono::system_clock::now().time_since_epoch());
	}

	return success;
}

PsMoveServiceResult^ RiftCatPsMoveClient::update()
{
	// Process incoming/outgoing networking requests
	PSM_UpdateNoPollMessages();

	// Poll events queued up by the call to PSM_UpdateNoPollMessages()
	PSMMessage message;
	while (PSM_PollNextMessage(&message, sizeof(message)) == PSMResult_Success)
	{
		switch (message.payload_type)
		{
		case PSMMessage::_messagePayloadType_Response:
			for (int i = 0; i < trackedControllerCount; i++)
			{
				if (start_stream_request_ids[i] != -1 &&
					message.response_data.request_id == start_stream_request_ids[i])
				{
					handle_acquire_controller(message.response_data.result_code, i);
					start_stream_request_ids[i] = -1;
				}
			}
			break;
		case PSMMessage::_messagePayloadType_Event:
			handle_client_psmove_event(message.event_data.event_type);
			break;
		}
	}

	//Button data must be outside of loop because it contains data for all tracked controllers!
	PsMoveServiceResult^ result = gcnew PsMoveServiceResult();

	result->Pitch = 0.f;
	result->Roll = 0.f;
	result->Yaw = 0.f;
	result->X = 0.f;
	result->Y = 0.f;
	result->Z = 0.f;

	for (int i = 0; i < trackedControllerCount; i++)
	{
		if (controller_views[i] && controller_views[i]->bValid && controller_views[i]->ControllerType == PSMController_Move)
		{
			std::chrono::milliseconds now =
				std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now().time_since_epoch());
			std::chrono::milliseconds diff = now - last_report_fps_timestamp;

			PSMPSMove moveView = controller_views[i]->ControllerState.PSMoveState;
			PSMPosef controllerPose = moveView.Pose;

			PSMQuatf normalizedQuat = PSM_QuatfNormalizeWithDefault(&controllerPose.Orientation, k_psm_quaternion_identity);
			//glm::quat glmOrientation = glm::quat(normalizedQuat.w, normalizedQuat.x, normalizedQuat.y, normalizedQuat.z);

			result->X = controllerPose.Position.x;
			result->Y = controllerPose.Position.y;
			result->Z = controllerPose.Position.z;

			//data.pitch = glm::pitch(glmOrientation);
			//data.yaw = glm::yaw(glmOrientation);
			//data.roll = glm::roll(glmOrientation);

			//Calcuate rotation here, glm library doesn't work for yaw
			//Both glm and this seem to work fine when each axis is independent, but issues when combined. 
			result->Yaw = std::atan2(2 * normalizedQuat.y * normalizedQuat.w - 2 * normalizedQuat.x * normalizedQuat.z, 1 - 2 * normalizedQuat.y * normalizedQuat.y - 2 * normalizedQuat.z * normalizedQuat.z);
			result->Roll = std::asin(2 * normalizedQuat.x * normalizedQuat.y + 2 * normalizedQuat.z * normalizedQuat.w);
			result->Pitch = std::atan2(2 * normalizedQuat.x * normalizedQuat.w - 2 * normalizedQuat.y * normalizedQuat.z, 1 - 2 * normalizedQuat.x * normalizedQuat.x - 2 * normalizedQuat.z * normalizedQuat.z);

			//WriteToFreepie(poseData, trackedFreepieIndicies[i]);

			if (m_sendSensorData)
			{
				PSMPSMoveCalibratedSensorData sensors = moveView.CalibratedSensorData;

				//Send sensor data through pos/rot struct
				freepie_io_6dof_data sensorData1;
				sensorData1.x = sensors.Accelerometer.x;
				sensorData1.y = sensors.Accelerometer.y;
				sensorData1.z = sensors.Accelerometer.z;

				sensorData1.pitch = sensors.Gyroscope.x;
				sensorData1.roll = sensors.Gyroscope.y;
				sensorData1.yaw = sensors.Gyroscope.z;
				//WriteToFreepie(sensorData1, 1);

				freepie_io_6dof_data sensorData2;
				sensorData2.x = sensors.Magnetometer.x;
				sensorData2.y = sensors.Magnetometer.y;
				sensorData2.z = sensors.Magnetometer.z;

				//WriteToFreepie(sensorData2, 2);
			}
			if (moveView.StartButton == PSMButtonState_DOWN && moveView.StartButton == PSMButtonState_DOWN) {
				result->Recenter = true;
			}
		}
	}
	return result;
}

void RiftCatPsMoveClient::shutdown()
{
	std::cout << "FreepieMoveClient is shutting down!" << std::endl;

	free_controller_views();

	// Close all active network connections
	PSM_Shutdown();
}

RiftCatPsMoveClient::~RiftCatPsMoveClient()
{
	shutdown();
}

void RiftCatPsMoveClient::init_controller_views() {
	// Once created, updates will automatically get pushed into this view
	for (int i = 0; i < trackedControllerCount; i++)
	{
		PSM_AllocateControllerListener(trackedControllerIDs[i]);
		controller_views[i] = PSM_GetController(trackedControllerIDs[i]);

		// Kick off request to start streaming data from the first controller
		PSM_StartControllerDataStreamAsync(
			controller_views[i]->ControllerID,
			m_sendSensorData ? PSMStreamFlags_includePositionData | PSMStreamFlags_includeCalibratedSensorData : PSMStreamFlags_includePositionData,
			&start_stream_request_ids[i]);

		//Set bulb color if specified
		if ((trackedBulbColors[i] >= 0) && (trackedBulbColors[i] < PSMTrackingColorType_MaxColorTypes)) {
			PSMRequestID request_id;
			PSM_SetControllerLEDColorAsync(controller_views[i]->ControllerID, trackedBulbColors[i], &request_id);
			PSM_EatResponse(request_id);
		}
	}
}

void RiftCatPsMoveClient::free_controller_views() {
	// Free any allocated controller views
	for (int i = 0; i < trackedControllerCount; i++)
	{
		if (controller_views[i] != nullptr)
		{
			// Stop the controller stream
			PSMRequestID request_id;
			PSM_StopControllerDataStreamAsync(controller_views[i]->ControllerID, &request_id);
			PSM_EatResponse(request_id);

			// Free out controller listener
			PSM_FreeControllerListener(controller_views[i]->ControllerID);
			controller_views[i] = nullptr;
		}
	}
}
