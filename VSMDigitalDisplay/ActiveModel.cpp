#include "StdAfx.h"
#include "ActiveModel.h"
#include "DsimModel.h"

VOID ActiveModel::initialize(ICOMPONENT *cpt)
{
	mComponent = cpt;
	if (!mComponent)
	{
		return;
	}

	int width = atoi(mComponent->getprop((CHAR*)"WIDTH"));
	int height = atoi(mComponent->getprop((CHAR*)"HEIGHT"));
	mDisplay.Resize(width, height);

	mRotation = atoi(mComponent->getprop((CHAR*)"ROTATION"));

	BOX textbox;
	mComponent->getsymbolarea(-1, &textbox);

	mFPSPos.x = textbox.left + 700;
	mFPSPos.y = textbox.top + 150;
	mLinesPos.x = textbox.left + 1200;
	mLinesPos.y = textbox.top + 150;

	strcpy(mReadoutFPS, "0.0 fps");
	strcpy(mReadoutLines, "0 lines");
}

ISPICEMODEL *ActiveModel::getspicemodel (CHAR *primitive)
{
	return NULL;
}

IDSIMMODEL *ActiveModel::getdsimmodel (CHAR *primitive)
{
	return new DsimModel;
}

VOID ActiveModel::plot (ACTIVESTATE state)
{
	drawElements();
	drawScreen();
}

VOID ActiveModel::animate(INT element, ACTIVEDATA *newstate)
{
	drawText();
	drawScreen();
}

void ActiveModel::drawElements(void)
{
	if (!mComponent)
	{
		return;
	}

	mComponent->drawsymbol(-1);
	mComponent->drawsymbol(0);
}

void ActiveModel::drawText(void)
{
	if (!mComponent)
	{
		return;
	}

	sprintf(mReadoutFPS, "%.1f fps" , 1.0f/realtime(mDisplay.getFrameDeltaTime()));
	sprintf(mReadoutLines, "%d lines" , mDisplay.getMaxLinesCount());


	mComponent->settextcolour(0);
	mComponent->drawtext(mFPSPos.x, mFPSPos.y, 0, TXJ_LEFT | TXJ_MIDDLE, mReadoutFPS);
	mComponent->drawtext(mLinesPos.x, mLinesPos.y, 0, TXJ_LEFT | TXJ_MIDDLE, mReadoutLines);
}

void ActiveModel::drawScreen(void)
{
	if (!mComponent)
	{
		return;
	}

	BOX textbox;
	mComponent->getsymbolarea(-1, &textbox);

	textbox.left += 470;
	textbox.top += 270;
	textbox.right -= 280;
	textbox.bottom -= 170;
	HDC hdc = mComponent->begincache(textbox);

	if (!hdc)
	{
		return;
	}

	int sourceWidth = mDisplay.getWidth();
	int sourceHeight = mDisplay.getHeight();

	int displayWidth = 0, displayHeight = 0;
	switch (mRotation)
	{
	case 0:
	case 2:
	default:
		displayWidth = mDisplay.getWidth();
		displayHeight = mDisplay.getHeight();
		break;
	case 1:
	case 3:
		displayWidth = mDisplay.getHeight();
		displayHeight = mDisplay.getWidth();
		break;
	}

	int *pixels = new int [sizeof(int) * displayWidth * displayHeight];
	const RGBTRIPLE *optSourcePixels = &mDisplay.getPixels()[0];
	int *optDestinationPixels = &pixels[0];
	int i = 0;
	switch (mRotation)
	{
	case 0:
	default:
		for (int y = 0; y < sourceHeight; y++)
		{
			for (int x = 0; x < sourceWidth; x++)
			{
				optDestinationPixels[x + (y * displayWidth)] = (optSourcePixels[i].rgbtRed << 16) | (optSourcePixels[i].rgbtGreen << 8) | (optSourcePixels[i].rgbtBlue << 0);
				i++;
			}
		}
		break;
	case 1:
		for (int y = sourceHeight-1 ; y >= 0; y--)
		{
			for (int x = 0; x < sourceWidth; x++)
			{
				optDestinationPixels[y + (x * displayWidth)] = (optSourcePixels[i].rgbtRed << 16) | (optSourcePixels[i].rgbtGreen << 8) | (optSourcePixels[i].rgbtBlue << 0);
				i++;
			}
		}
		break;
	case 2:
		for (int y = sourceHeight - 1; y >= 0; y--)
		{
			for (int x = sourceWidth-1; x >= 0; x--)
			{
				optDestinationPixels[x + (y * displayWidth)] = (optSourcePixels[i].rgbtRed << 16) | (optSourcePixels[i].rgbtGreen << 8) | (optSourcePixels[i].rgbtBlue << 0);
				i++;
			}
		}
		break;
	case 3:
		for (int y = 0; y < sourceHeight; y++)
		{
			for (int x = sourceWidth - 1; x >= 0; x--)
			{
				optDestinationPixels[y + (x * displayWidth)] = (optSourcePixels[i].rgbtRed << 16) | (optSourcePixels[i].rgbtGreen << 8) | (optSourcePixels[i].rgbtBlue << 0);
				i++;
			}
		}
		break;
	}

	BITMAP bm;
	HBITMAP bitmap = CreateBitmap(displayWidth, displayHeight, 1 , 32 , &pixels[0]);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HGDIOBJ hbmOld = SelectObject(hdcMem, bitmap);


//	GetObject(bitmap, sizeof(bm), &bm);
//	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	HGDIOBJ hBitmap = GetCurrentObject(hdc, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(bm), &bm);
	StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, displayWidth, displayHeight, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);

	DeleteObject(bitmap);

	delete pixels;

	mComponent->endcache();
}

BOOL ActiveModel::actuate (WORD key, INT x, INT y, DWORD flags)
{
	return FALSE;
}
