// VSMDigitalDisplayTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "DsimModel.h"
#include "Display.h"

int main(int argc , char **argv)
{
	Display *display = new Display();
	display->Resize(256, 256);
	display->setDebugFramesFilename("..\\TestJig\\t");

	FILE *fp = fopen("..\\TestJig\\VSMDD debug VSMDD1.txt", "r");
	while (!feof(fp))
	{
		ABSTIME time;
		int r = 0, g = 0, b = 0;
		BOOL HSync, VSync;
		int num = fscanf(fp, DsimModel_DEBUG_FORMAT, &time , &r , &b , &g , &HSync , &VSync);
		if (num == 6)
		{
			display->simulate(time, r, g, b, HSync, VSync);
		}
	}

	fclose(fp);
}
