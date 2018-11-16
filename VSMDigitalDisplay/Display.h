#pragma once
#include "StdAfx.h"
#include "vsm.hpp"
#include <vector>

#pragma warning(disable : 4251)

class DLLEXPORT Display
{
public:
	Display();
	virtual ~Display();

	void Resize(const int width, const int height);

	void simulate(const ABSTIME time, const BYTE r , const BYTE g , const BYTE b , const bool hSync , const bool vSync);

	const std::vector<RGBTRIPLE> getPixels()
	{
		return mScreen;
	}

private:
	int mWidth, mHeight;
	bool mPreviousVSync, mPreviousHSync;
	int mCurrentX, mCurrentY;
	int mMaxLinesCount;
	int mLastIndexPlot;
	RGBTRIPLE mLastRGB;

	// These are start and end times for the visible portion of the screen
	ABSTIME mTimeHSyncStart, mTimeHSyncEnd;
	ABSTIME mLineDeltaTime;
	ABSTIME mTimeVSyncStart, mTimeVSyncEnd;
	ABSTIME mFrameDeltaTime;

	std::vector<RGBTRIPLE> mScreen;
};
