#pragma once
public ref class PsMoveServiceResult
{
public:
	PsMoveServiceResult();
	property float X;
	property float Y;
	property float Z;

	property float Pitch;
	property float Roll;
	property float Yaw;

	property bool Recenter;
};

//public ref class SummaryPsMoveResult {
//public:
//	PsMoveServiceResult^ Controller = gcnew PsMoveServiceResult();
//	PsMoveServiceResult^ Accelerometer = gcnew PsMoveServiceResult();
//	PsMoveServiceResult^ Magnetometer = gcnew PsMoveServiceResult();
//};
//
