#pragma once

#include "Resource.h"
#include "Nest.h"

//---------------------------------------------------------------------

int showConfigDialog(HWND hwnd);

//---------------------------------------------------------------------

class ConfigDialog : public Tools::Dialog
{
public:
	virtual void onOK();
	virtual void onCancel();
	virtual INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//---------------------------------------------------------------------
