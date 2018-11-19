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

	mLastRGB.rgbtRed = 0;
	mLastRGB.rgbtGreen = 0;
	mLastRGB.rgbtBlue = 0;
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
		}
	}

	if (!mPreviousHSync && hSync)
	{
		// HSync start of visible portion of screen
		mTimeHSyncStart = time;
		mCurrentX = 0;
	}
	else if (mPreviousHSync && !hSync)
	{
		// HSync end of visible portion of screen
		if (mTimeHSyncStart >= 0)
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

	if (hSync && vSync && mLineDeltaTime > 0)
	{
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

	mPreviousHSync = hSync;
	mPreviousVSync = vSync;
}
