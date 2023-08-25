#include "pch.h"
#include "Servant.h"

fgo::Servant* get_servant()
{
	CMyTracer::logger = 0; // デバッグトレースを有効にする場合はこの行をコメントアウトしてください。

	return &fgo::filter_drag::servant;
}
