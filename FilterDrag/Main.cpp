#include "pch.h"
#include "Servant.h"

//
// デバッグ用コールバック関数。デバッグメッセージを出力する。
//
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

fgo::Servant* get_servant()
{
	return &fgo::filter_drag::servant;
}
