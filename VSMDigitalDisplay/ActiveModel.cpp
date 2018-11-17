#include "StdAfx.h"
#include "ActiveModel.h"
#include "DsimModel.h"

VOID ActiveModel::initialize(ICOMPONENT *cpt)
{
	component = cpt;

	// Get entire component(-1) origin
	BOX textbox;
	cpt->getsymbolarea(-1, &textbox);

	// Do some math to position the readouts properly
	volts.x = (textbox.left + textbox.right) / 2;
	volts.y = (textbox.top + textbox.bottom) / 2 - 100;
	amps.x = (textbox.left + textbox.right) / 2;
	amps.y = (textbox.top + textbox.bottom) / 2 + 100;

	// Initial readout:
	strcpy(readout_v, " V00.0");
	strcpy(readout_a, " A0.00");
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
	// Draw entire component on the screen
	component->drawsymbol(-1);
	// Draw symbol 0 overtop components
	component->drawsymbol(0);
	// Place the default readouts on the screen
	component->drawtext(volts.x, volts.y, 0, TXJ_CENTRE | TXJ_MIDDLE, readout_v);
	component->drawtext(amps.x, amps.y, 0, TXJ_CENTRE | TXJ_MIDDLE, readout_a);
}

VOID ActiveModel::animate (INT element, ACTIVEDATA *newstate)
{
	BOX textbox;
	component->getsymbolarea(-1, &textbox);

	// Draw entire component on the screen
//	component->drawsymbol(-1);
	// Draw symbol 0 overtop components
//	component->drawsymbol(0);
	// Re-draw the displayed value with the new results
//	component->drawtext(volts.x, volts.y, 0, TXJ_CENTRE | TXJ_MIDDLE, readout_v);
//	component->drawtext(amps.x, amps.y, 0, TXJ_CENTRE | TXJ_MIDDLE, readout_a);

	textbox.left += 470;
	textbox.top += 270;
	textbox.right -= 250;
//	textbox.bottom += 300;
	HDC hdc = component->begincache(textbox);

	int pixels[32 * 32];
	for (int i = 0; i < 32 * 32; i++)
	{
		pixels[i] = rand();
	}

	BITMAP bm;
	PAINTSTRUCT ps;
	HBITMAP bitmap = CreateBitmap(32 , 32 , 1 , 32 , pixels);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HGDIOBJ hbmOld = SelectObject(hdcMem, bitmap);


//	GetObject(bitmap, sizeof(bm), &bm);
//	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	HGDIOBJ hBitmap = GetCurrentObject(hdc, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(bm), &bm);
	StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, 32 , 32, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);

	DeleteObject(bitmap);

#if 0
	MoveToEx(hDC, volts.x, volts.y, NULL);
	LineTo(hDC, volts.x+50, volts.y+50);
	COLORREF color = RGB(255, 0, 0); // COLORREF to hold the color info`
	for (int i = 0; i < 400; i++)
	{
		for (int j = 0; j < 50; j++)
			SetPixel(hDC, i, j, color);
	}
#endif

	component->endcache();
}

BOOL ActiveModel::actuate (WORD key, INT x, INT y, DWORD flags)
{
	return FALSE;
}
