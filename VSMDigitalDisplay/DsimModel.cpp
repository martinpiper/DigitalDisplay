#include "StdAfx.h"
#include "DsimModel.h"
#include <stdio.h>
#include <string>

INT DsimModel::isdigital (CHAR *pinname)
{
	return TRUE;							// Indicates this is a digital model
}

VOID DsimModel::setup (IINSTANCE *instance, IDSIMCKT *dsimckt)
{
	std::string filename = "VSMDD debug ";
	filename.append(instance->id());
	filename.append(".txt");

	mDebugOutFP = fopen(filename.c_str(), "w");

	mInstance = instance;
	mDigitalComponent = dsimckt;
	int i;
	for (i = 0; i < 8; i++)
	{
		char temp[8];
		sprintf(temp, "D%d", i);
		mPinD[i] = mInstance->getdsimpin(temp, true);
		sprintf(temp, "R%d", i);
		mPinR[i] = mInstance->getdsimpin(temp, true);
		sprintf(temp, "G%d", i);
		mPinG[i] = mInstance->getdsimpin(temp, true);
		sprintf(temp, "B%d", i);
		mPinB[i] = mInstance->getdsimpin(temp, true);
	}

	mPinHSYNC = mInstance->getdsimpin((CHAR*)"_HSYNC", true);
	mPinVSYNC = mInstance->getdsimpin((CHAR*)"_VSYNC", true);
}

VOID DsimModel::runctrl (RUNMODES mode)
{
}

VOID DsimModel::actuate (REALTIME time, ACTIVESTATE newstate)
{
}

BOOL DsimModel::indicate (REALTIME time, ACTIVEDATA *data)
{
	return FALSE;
}

VOID DsimModel::simulate(ABSTIME time, DSIMMODES mode)
{
	int r = 0, g = 0, b = 0;
	for (int i = 7; i >= 0; i--)
	{
		r <<= 1;
		if (ishigh(mPinR[i]->istate()))
		{
			r |= 1;
		}

		g <<= 1;
		if (ishigh(mPinG[i]->istate()))
		{
			g |= 1;
		}

		b <<= 1;
		if (ishigh(mPinB[i]->istate()))
		{
			b |= 1;
		}

		if (ishigh(mPinR[i]->istate()) && ishigh(mPinG[i]->istate()) && ishigh(mPinB[i]->istate()))
		{
			mPinD[i]->setstate(time, 1, SHI);
		}
		else
		{
			mPinD[i]->setstate(time, 1, SLO);
		}
	}

	fprintf(mDebugOutFP, "time %20llu R%02x G%02x B%02x H%02x V%02x\n", time , r, g, b, ishigh(mPinHSYNC->istate()), ishigh(mPinVSYNC->istate()));
	fflush(mDebugOutFP);
}

VOID DsimModel::callback (ABSTIME time, EVENTID eventid)
{
}
