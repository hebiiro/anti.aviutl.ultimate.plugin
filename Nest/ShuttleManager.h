#pragma once

//---------------------------------------------------------------------

#include "Shuttle.h"

//---------------------------------------------------------------------

class ShuttleManager
{
public:

	typedef std::map<_bstr_t, ShuttlePtr> ShuttleMap;

	ShuttleMap m_map;

public:

	// 名前からシャトルを取得する。
	ShuttlePtr getShuttle(const _bstr_t& name);

	// 指定された名前でシャトルを登録する。
	void addShuttle(ShuttlePtr shuttle, const _bstr_t& name, HWND hwnd);

	// シャトルを取り除く。
	void removeShuttle(Shuttle* shuttle);

	// シャトルの名前を変更する。
	void renameShuttle(Shuttle* shuttle, const _bstr_t& newName);

	// シャトルの名前を変更する。新しい名前はユーザーが入力する。
	void renameShuttle(HWND owner, Shuttle* shuttle);

};

//---------------------------------------------------------------------
