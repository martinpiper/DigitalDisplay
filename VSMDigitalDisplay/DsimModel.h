#pragma once
#include "StdAfx.h"
#include <stdio.h>
#include "vsm.hpp"

#define InfoLog(__s__) inst->log(__s__);

#define DsimModel_DEBUG_FORMAT "time %20llu R%02x G%02x B%02x H%02x V%02x\n"

class DsimModel : public IDSIMMODEL
{
public:
	INT isdigital (CHAR *pinname);
	VOID setup (IINSTANCE *inst, IDSIMCKT *dsim);
	VOID runctrl (RUNMODES mode);
	VOID actuate (REALTIME time, ACTIVESTATE newstate);
	BOOL indicate (REALTIME time, ACTIVEDATA *data);
	VOID simulate (ABSTIME time, DSIMMODES mode);
	VOID callback (ABSTIME time, EVENTID eventid);
private:
	IINSTANCE *mInstance;
	IDSIMCKT *mDigitalComponent;
	IDSIMPIN *mPinD[8];
	IDSIMPIN *mPinHSYNC , *mPinVSYNC;
	IDSIMPIN *mPinR[8], *mPinG[8], *mPinB[8];

	FILE *mDebugOutFP;

	IACTIVEMODEL *mActiveModel;
};
