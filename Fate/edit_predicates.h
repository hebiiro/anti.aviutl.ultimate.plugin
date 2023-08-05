#pragma once

#include <cstdint>
#include <array>
#include <cwchar>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// テキストオブジェクト，スクリプト制御，カメラスクリプト制御のエディットボックスは
// 特定の ID に割り振られる:
//	ID = id_edit_base + idx_filter * id_diff_per_filter + id_ofs_****
// ID が他コントロールと重複することはあるが，それが原因で明確な誤動作をした経験はこのプラグインでは今のところない．
inline bool editbox_check_id(uint32_t id) {
	constexpr uint32_t id_edit_base = 22100, id_diff_per_filter = 0x100,
		id_ofs_text = 1, id_ofs_script = 0;
	return id >= id_edit_base &&
		(id - id_edit_base) % id_diff_per_filter <= std::max(id_ofs_text, id_ofs_script);
}
// ウィンドウクラスの名前一致を確認する関数．毎回 MAX_PATH 文字も確保しなくてもよい．
template<size_t N>
inline bool check_window_classname(HWND hwnd, const wchar_t(&name)[N])
{
	wchar_t buff[N + 1];
	return ::GetClassNameW(hwnd, buff, std::size(buff)) == N - 1 && std::wcscmp(buff, name) == 0;
}
// 対象エディットボックスのスタイル確認項目．
// 複数行表示，Enterキーで改行可能，隠れていないという条件を満たすものとする．
inline bool editbox_check_style(HWND hwnd) {
	constexpr auto
		style = ES_MULTILINE | ES_WANTRETURN | WS_VISIBLE,
		style_mask = style;
	return (::GetWindowLongW(hwnd, GWL_STYLE) & style_mask) == style;
}
