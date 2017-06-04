#pragma once
#include "RiftCatPsMoveClient.h"
#include "PsMoveServiceResult.h"

public ref class PsMoveServiceClientWrapper
{
public:
	PsMoveServiceClientWrapper();
	~PsMoveServiceClientWrapper();
	bool Startup();
	PsMoveServiceResult^ Update();
	void Shutdown();
private:
	RiftCatPsMoveClient* riftCatPsMoveClient;
};

