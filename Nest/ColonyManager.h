#pragma once

//---------------------------------------------------------------------

class ColonyManager
{
public:

	std::vector<HWND> m_array;

public:

	PanePtr getRootPane(HWND colony);
	BOOL canDocking(HWND colony, HWND hwnd);
	void calcLayout(HWND colony);
	void calcAllLayout();
	void updateColonyMenu();
	void executeColonyMenu(UINT id);

	void insert(HWND colony);
	void erase(HWND colony);
	void eraseInternal(HWND colony);
	void clear();
};

//---------------------------------------------------------------------
