#pragma once
#include "StdAfx.h"
#include "vsm.hpp"
#include "Display.h"

#if VSM_API_VERSION < 200
#define KEYCODE WORD
#endif

class ActiveModel : public IACTIVEMODEL
{
public:
    VOID initialize (ICOMPONENT *cpt);
    ISPICEMODEL *getspicemodel (CHAR *primitive);
    IDSIMMODEL *getdsimmodel (CHAR *primitive);
    VOID plot (ACTIVESTATE state);
    VOID animate (INT element, ACTIVEDATA *newstate);
	BOOL actuate (KEYCODE key, INT x, INT y, DWORD flags);

	void drawElements(void);
	void drawText(void);
	void drawScreen(void);

	Display& getDisplay()
	{
		return mDisplay;
	}

private:
	ICOMPONENT *mComponent;
	POINT mFPSPos;
	POINT mLinesPos;
	CHAR mReadoutFPS[10];
	CHAR mReadoutLines[10];

	Display mDisplay;

	int mRotation;
};
