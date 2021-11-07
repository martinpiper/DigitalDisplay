#include "stdafx.h"
#include "Display.h"

Display::Display() : mScreen(0)
{
	Resize(0, 0);
}

Display::~Display()
{
	delete mScreen;
}

void Display::Resize(const int width, const int height)
{
	mWidth = width;
	mHeight = height;
	delete mScreen;
	mScreen = new RGBTRIPLE[mWidth * mHeight];
	srand(0);
	for (int i = 0; i < mWidth*mHeight; i++)
	{
		mScreen[i].rgbtRed = (BYTE)rand();
		mScreen[i].rgbtGreen = mScreen[i].rgbtRed;
		mScreen[i].rgbtBlue = mScreen[i].rgbtRed;
	}

	mPreviousVSync = true;
	mPreviousHSync = true;
	mCurrentX = 0;
	mCurrentY = 0;
	mMaxLinesCount = 0;
	mLastIndexPlot = 0;

	mTimeHSyncStart = -1;
	mTimeHSyncEnd = -1;
	mLineDeltaTime = 0;

	mTimeVSyncStart = -1;
	mTimeVSyncEnd = -1;
	mFrameDeltaTime = 0;
	mFilterSignals = 0;

	mLastRGB.rgbtRed = 0;
	mLastRGB.rgbtGreen = 0;
	mLastRGB.rgbtBlue = 0;

	mGotFrame = false;
	mFrameNumber = 0;

	mHaveSyncDisplayWarningHSYNC = false;
	mHaveSyncDisplayWarningVSYNC = false;

	mSyncDataDisplay = false;
	mSyncDataWarning = false;
}

void Display::simulate(const ABSTIME time, const BYTE r, const BYTE g, const BYTE b, const bool hSync, const bool vSync)
{
	if (!mPreviousVSync && vSync)
	{
		// VSync start of visible portion of screen
		if (mTimeVSyncStart >= 0)
		{
			mFrameDeltaTime = time - mTimeVSyncStart;
		}
		mTimeVSyncStart = time;

		mCurrentX = 0;
		mCurrentY = 0;
		mLastIndexPlot = 0;
		mLastRGB.rgbtRed = 0;
		mLastRGB.rgbtGreen = 0;
		mLastRGB.rgbtBlue = 0;
		if (mGotFrame && !mDebugFramesFilename.empty())
		{
			writeImageFrame();
		}
	}
	else if (mPreviousVSync && !vSync)
	{
		if (mTimeVSyncStart >= 0)
		{
			// VSync end of visible portion of screen
			mTimeVSyncEnd = time;

			// Fill any pixels from the last update
			while (mLastIndexPlot < mWidth*mHeight)
			{
				mScreen[mLastIndexPlot++] = mLastRGB;
			}
			mGotFrame = true;
		}
	}

	if (!mPreviousHSync && hSync)
	{
		// In this special debug mode the data is visible in the sync period, +HSync end of visible portion of screen
		if (mSyncDataDisplay && mTimeHSyncStart >= 0)
		{
			mTimeHSyncEnd = time;
			mLineDeltaTime = mTimeHSyncEnd - mTimeHSyncStart;
			if (mLineDeltaTime > 0)
			{
				mCurrentY++;
				if (mCurrentY > mMaxLinesCount)
				{
					mMaxLinesCount = mCurrentY;
				}
			}
		}

		// HSync start of visible portion of screen
		mTimeHSyncStart = time;
		mCurrentX = 0;
	}
	else if (mPreviousHSync && !hSync)
	{
		// Not in debug mode, -HSync end of visible portion of screen
		if (!mSyncDataDisplay && mTimeHSyncStart >= 0)
		{
			mTimeHSyncEnd = time;
			mLineDeltaTime = mTimeHSyncEnd - mTimeHSyncStart;
			if (mLineDeltaTime > 0)
			{
				mCurrentY++;
				if (mCurrentY > mMaxLinesCount)
				{
					mMaxLinesCount = mCurrentY;
				}
			}
		}
	}

	// Depending on the debug display mode, choose to output data during the sync or not...
	if ((!mSyncDataDisplay && hSync && vSync && mLineDeltaTime > 0) || (mSyncDataDisplay && mLineDeltaTime > 0 && mTimeVSyncStart > 0))
	{
		ABSTIME pixelDrawtimeDelta = time - mLastPixelDrawTime;
		if (pixelDrawtimeDelta > 0 && pixelDrawtimeDelta < mFilterSignals && mLastIndexPlot > 0)
		{
			// Rewind to resubmit the last very recent update, try to filter out signal deltas due to very short hi to lo or lo to hi variances
			mLastIndexPlot--;
			mLastRGB = mScreen[mLastIndexPlot];
		}
		mLastPixelDrawTime = time;

		mCurrentX = (int) ((mWidth * (time - mTimeHSyncStart)) / mLineDeltaTime);

		int thisIndex = (mCurrentY * mWidth) + mCurrentX;

		// Fill any pixels from the last update
		while (mLastIndexPlot < thisIndex && mLastIndexPlot < mWidth*mHeight)
		{
			mScreen[mLastIndexPlot++] = mLastRGB;
		}

		mLastRGB.rgbtRed = r;
		mLastRGB.rgbtGreen = g;
		mLastRGB.rgbtBlue = b;

		if (mLastIndexPlot < mWidth*mHeight)
		{
			mScreen[mLastIndexPlot] = mLastRGB;
		}
	}

	if (mSyncDataWarning && !hSync && (r > 0 || g > 0 || b > 0))
	{
		mHaveSyncDisplayWarningHSYNC = true;
	}
	if (mSyncDataWarning && !vSync && (r > 0 || g > 0 || b > 0))
	{
		mHaveSyncDisplayWarningVSYNC = true;
	}


	mPreviousHSync = hSync;
	mPreviousVSync = vSync;
}

void Display::writeImageFrame(void)
{
	std::string filename = mDebugFramesFilename;
	char number[10];
	sprintf(number, "%08d", mFrameNumber);
	filename += number;
	filename += ".bmp";

	// 256x256x24bit BMP header
	unsigned char data[54] = {
		0x42, 0x4D, 0x36, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
		0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	data[0x12] = mWidth & 255;
	data[0x13] = (mWidth>>8) & 255;
	data[0x14] = (mWidth >> 16) & 255;
	data[0x15] = (mWidth >> 24) & 255;

	data[0x16] = mHeight & 255;
	data[0x17] = (mHeight >> 8) & 255;
	data[0x18] = (mHeight >> 16) & 255;
	data[0x19] = (mHeight >> 24) & 255;

	FILE *fp = fopen(filename.c_str(), "wb");
	fwrite(data, 1, sizeof(data), fp);
	// Write out bottom to top
	for (int y = mHeight - 1; y >= 0; y--)
	{
		for (int x = 0; x < mWidth; x++)
		{
			fputc(mScreen[x + (y*mWidth)].rgbtBlue, fp);
			fputc(mScreen[x + (y*mWidth)].rgbtGreen, fp);
			fputc(mScreen[x + (y*mWidth)].rgbtRed, fp);
		}
	}

	fclose(fp);

	mFrameNumber++;
}
