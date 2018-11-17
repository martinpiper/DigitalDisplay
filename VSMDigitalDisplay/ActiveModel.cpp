#include "StdAfx.h"
#include "ActiveModel.h"
#include "DsimModel.h"

VOID ActiveModel::initialize (ICOMPONENT *cpt)
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

	// Create a new text style in 'Templates' menu or other objects get funky
	// Still makes other meters readouts conform to his style, how to use properly??
	textstyle = cpt->createtextstyle((CHAR*)"UM_METER");

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
	// Draw entire component on the screen
	component->drawsymbol(-1);
	// Draw symbol 0 overtop components
	component->drawsymbol(0);
	// Re-draw the displayed value with the new results
	component->drawtext(volts.x, volts.y, 0, TXJ_CENTRE | TXJ_MIDDLE, readout_v);
	component->drawtext(amps.x, amps.y, 0, TXJ_CENTRE | TXJ_MIDDLE, readout_a);
}

BOOL ActiveModel::actuate (WORD key, INT x, INT y, DWORD flags)
{
	return FALSE;
}
