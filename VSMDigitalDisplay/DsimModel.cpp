#include "StdAfx.h"
#include "DsimModel.h"
#include <stdio.h>
#include <string>
#include "ActiveModel.h"

//#define GENERATE_DEBUG

INT DsimModel::isdigital (CHAR *pinname)
{
	return TRUE;
}

VOID DsimModel::setup(IINSTANCE *instance, IDSIMCKT *dsimckt)
{
	mActiveModel = instance->getactivemodel();
	mInstance = instance;
	mDigitalComponent = dsimckt;

#ifdef GENERATE_DEBUG
	std::string filename = "VSMDD debug ";
	filename.append(instance->id());
	filename.append(".txt");
	mDebugOutFP = fopen(filename.c_str(), "w");
#endif

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

	CHAR *t = mInstance->getstrval((CHAR*)"FRAMES");
	if (t)
	{
		std::string filename = t;

		t = mInstance->getstrval((CHAR*)"FRAMESDEBUG");
		bool writeFrames = atoi(t) ? true : false;

		if (mActiveModel && writeFrames && !filename.empty())
		{
			((ActiveModel*)mActiveModel)->getDisplay().setDebugFramesFilename(filename);
		}
	}

	DOUBLE filterTime;
#if VSM_API_VERSION >= 200
	filterTime = mInstance->getnumval((CHAR*)"FILTERSIGNALS");
#else
	mInstance->getnumval(&filterTime, (CHAR*)"FILTERSIGNALS");
#endif
	if (mActiveModel)
	{
		((ActiveModel*)mActiveModel)->getDisplay().setFilterSignals(dsimtime(filterTime));
	}

	BOOL bvalue = mInstance->getboolval((CHAR*)"SYNCDATAWHOLE");
	if (mActiveModel)
	{
		((ActiveModel*)mActiveModel)->getDisplay().setSyncDataWhole(bvalue);
	}

	bvalue = mInstance->getboolval((CHAR*)"SYNCDATADISPLAY");
	if (mActiveModel)
	{
		((ActiveModel*)mActiveModel)->getDisplay().setSyncDataDisplay(bvalue);
	}

	bvalue = mInstance->getboolval((CHAR*)"SYNCDATAWARNING");
	if (mActiveModel)
	{
		((ActiveModel*)mActiveModel)->getDisplay().setSyncDataWarning(bvalue);
	}

	t = mInstance->getstrval((CHAR*)"EXTERNALDISPLAYADDRESS");
	if (t)
	{
		if (mActiveModel && strlen(t) > 0)
		{
			((ActiveModel*)mActiveModel)->getDisplay().setAddress(t);
		}
	}

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

#ifdef GENERATE_DEBUG
	fprintf(mDebugOutFP, DsimModel_DEBUG_FORMAT, time , r, g, b, ishigh(mPinHSYNC->istate()), ishigh(mPinVSYNC->istate()));
	fflush(mDebugOutFP);
#endif

	if (mActiveModel)
	{
		((ActiveModel*)mActiveModel)->getDisplay().simulate(time, r, g, b, ishigh(mPinHSYNC->istate()), ishigh(mPinVSYNC->istate()));

		bool gotDisplayWarning = ((ActiveModel*)mActiveModel)->getDisplay().getSyncDisplayWarningHSYNC();
		if (gotDisplayWarning)
		{
			mInstance->warning((CHAR*)"Display detected data during _HSYNC");
		}
		gotDisplayWarning = ((ActiveModel*)mActiveModel)->getDisplay().getSyncDisplayWarningVSYNC();
		if (gotDisplayWarning)
		{
			mInstance->warning((CHAR*)"Display detected data during _VSYNC");
		}
	}
}

VOID DsimModel::callback (ABSTIME time, EVENTID eventid)
{
}
