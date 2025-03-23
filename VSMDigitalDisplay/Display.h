#pragma once
#include "StdAfx.h"
#include "vsm.hpp"
#include <vector>
#include <string>

#include "RNXPSockets/Inc/XPSockets.h"

#pragma warning(disable : 4251)

class DLLEXPORT Display
{
public:
	Display();
	virtual ~Display();

	void Resize(const int width, const int height);
	void CreateSnow(void);

	void simulate(const ABSTIME time, const BYTE r , const BYTE g , const BYTE b , const bool hSync , const bool vSync);

	const RGBTRIPLE *getPixels()
	{
		return mScreen;
	}

	const int getWidth()
	{
		return mWidth;
	}
	const int getHeight()
	{
		return mHeight;
	}

	const ABSTIME getFrameDeltaTime()
	{
		return mFrameDeltaTime;
	}

	const int getMaxLinesCount()
	{
		return mMaxLinesCount;
	}

	void setDebugFramesFilename(const std::string filename)
	{
		mDebugFramesFilename = filename;
	}

	void setFilterSignals(const ABSTIME time)
	{
		mFilterSignals = time;
	}

	void setSyncDataWhole(const bool value)
	{
		mSyncDataWhole = value;
	}

	void setSyncDataDisplay(const bool value)
	{
		mSyncDataDisplay = value;
	}

	void setSyncDataWarning(const bool value)
	{
		mSyncDataWarning = value;
	}

	bool getSyncDisplayWarningHSYNC(void)
	{
		bool ret = mHaveSyncDisplayWarningHSYNC;
		mHaveSyncDisplayWarningHSYNC = false;
		return ret;
	}
	bool getSyncDisplayWarningVSYNC(void)
	{
		bool ret = mHaveSyncDisplayWarningHSYNC;
		mHaveSyncDisplayWarningHSYNC = false;
		return ret;
	}

	void setAddress(const char *address)
	{
		mAddressSet = mAddress.Import(address);
	}

	bool parseMessage(const char *message);

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
	ABSTIME mLastPixelDrawTime;
	ABSTIME mFilterSignals;

	RGBTRIPLE *mScreen;

	std::string mDebugFramesFilename;
	bool mGotFrame;
	int mFrameNumber;

	bool mSyncDataWhole;
	bool mSyncDataDisplay;
	bool mSyncDataWarning;
	bool mHaveSyncDisplayWarningHSYNC;
	bool mHaveSyncDisplayWarningVSYNC;

	void writeImageFrame(void);

	bool mAddressSet;
	RNReplicaNet::XPAddress mAddress;
	RNReplicaNet::XPSocket mConnectTo;
};
