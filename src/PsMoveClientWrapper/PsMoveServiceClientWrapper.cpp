#include "stdafx.h"
#include "PsMoveServiceClientWrapper.h"


PsMoveServiceClientWrapper::PsMoveServiceClientWrapper()
{
	riftCatPsMoveClient = new RiftCatPsMoveClient();
}

PsMoveServiceClientWrapper::~PsMoveServiceClientWrapper()
{
	riftCatPsMoveClient->~RiftCatPsMoveClient();
}

bool PsMoveServiceClientWrapper::Startup()
{
	return riftCatPsMoveClient->startup();
}

PsMoveServiceResult^ PsMoveServiceClientWrapper::Update()
{
	return riftCatPsMoveClient->update();
}

void PsMoveServiceClientWrapper::Shutdown()
{
	riftCatPsMoveClient->shutdown();
}
