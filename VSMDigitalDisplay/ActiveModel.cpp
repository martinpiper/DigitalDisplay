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

	BOX textbox;
	mComponent->getsymbolarea(-1, &textbox);

	mFPSPos.x = textbox.left + 700;
	mFPSPos.y = textbox.top + 150;
	mLinesPos.x = textbox.left + 1200;
	mLinesPos.y = textbox.top + 150;

	strcpy(mReadoutFPS, "0.0 fps");
	strcpy(mReadoutLines, "0 lines");

	mDisplay.Resize(256, 270);
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
	mComponent->drawtext(mFPSPos.x, mFPSPos.y, 0, TXJ_CENTRE | TXJ_MIDDLE, mReadoutFPS);
	mComponent->drawtext(mLinesPos.x, mLinesPos.y, 0, TXJ_CENTRE | TXJ_MIDDLE, mReadoutLines);
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

	int *pixels = new int [sizeof(int) * mDisplay.getWidth() * mDisplay.getHeight()];
	const RGBTRIPLE *optSourcePixels = &mDisplay.getPixels()[0];
	int *optDestinationPixels = &pixels[0];
	for (int i = 0; i < mDisplay.getWidth() * mDisplay.getHeight(); i++)
	{
		optDestinationPixels[i] = (optSourcePixels[i].rgbtRed << 16) | (optSourcePixels[i].rgbtGreen << 8) | (optSourcePixels[i].rgbtBlue << 0);
	}

	BITMAP bm;
	HBITMAP bitmap = CreateBitmap(mDisplay.getWidth() , mDisplay.getHeight() , 1 , 32 , &pixels[0]);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HGDIOBJ hbmOld = SelectObject(hdcMem, bitmap);


//	GetObject(bitmap, sizeof(bm), &bm);
//	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	HGDIOBJ hBitmap = GetCurrentObject(hdc, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(bm), &bm);
	StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, mDisplay.getWidth(), mDisplay.getHeight() , SRCCOPY);

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
