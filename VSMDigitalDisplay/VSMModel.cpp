// VSMModel.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "vsm.hpp"
#include "ActiveModel.h"
#include "DsimModel.h"
#include <stdio.h>

static int key = 0x80808081;

extern "C" {
	IDSIMMODEL __declspec (dllexport) *createdsimmodel(CHAR *device, ILICENCESERVER *ils)
	{
		// Scan for and obtain any suitable key
		while (!ils->authorize(key))
		{
			key++;
			char temp[256];
			sprintf(temp, "VSMDigitalDisplay Trying1 %x...\n", key);
			OutputDebugStringA(temp);
		}
		return new DsimModel;
	}
}

extern "C" VOID __declspec (dllexport) deletedsimmodel(IDSIMMODEL *model)
{
	delete (DsimModel *)model;
}

extern "C" __declspec(dllexport) IACTIVEMODEL *createactivemodel(CHAR *device, ILICENCESERVER *ils)
{
	// Scan for and obtain any suitable key
	while (!ils->authorize(key))
	{
		key++;
		char temp[256];
		sprintf(temp, "VSMDigitalDisplay Trying2 %x...\n", key);
		OutputDebugStringA(temp);
	}
	return new ActiveModel;
}

extern "C" VOID __declspec(dllexport) deleteactivemodel(IACTIVEMODEL *model)
{
	delete (ActiveModel *)model;
}
