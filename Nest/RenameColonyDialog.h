#pragma once

#include "Resource.h"
#include "Nest.h"

//---------------------------------------------------------------------

class RenameColonyDialog : public Tools::Dialog
{
public:
	ShuttleManager* m_shuttleManager = 0;
	Shuttle* m_shuttle = 0;

public:

	virtual int doModal(HWND parent);
	virtual void onOK();
	virtual void onCancel();
};

//---------------------------------------------------------------------
