// PsMoveClientWrapper.h

#pragma once
#include "../PSMoveFreepieBridge/FreepieMoveClient.h"

using namespace System;


namespace PsMoveClientWrapper {

	public ref class PsMoveServiceClient
	{
	public:
		PsMoveServiceClient();
		int Run();
	private:
		FreepieMoveClient* freepieMoveClient;
	};

}
