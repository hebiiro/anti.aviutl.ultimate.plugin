#pragma once

namespace apn::item_wave
{
	inline static std::unordered_set<int32_t> string_to_layers(const std::wstring& s)
	{
		std::unordered_set<int32_t> layers;
		std::wstringstream stream(s);
		std::wstring layer;
		while (std::getline(stream, layer, L','))
			layers.emplace(wcstol(layer.c_str(), nullptr, 0) - 1);
		return layers;
	}

	inline static std::wstring layers_to_string(const std::unordered_set<int32_t>& layers)
	{
		std::wstring s;
		for (const auto& layer : layers) {
			if (!s.empty()) s += L", ";
			s += my::format(L"{/}", layer + 1);
		}
		return s;
	}

	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"item_wave";
		inline static constexpr auto c_display_name = L"アイテム波形";

		inline static constexpr struct WaveType {
			inline static constexpr int32_t c_center = 0;
			inline static constexpr int32_t c_bottom_up = 1;
			inline static constexpr int32_t c_top_down = 2;
			inline static constexpr my::Label labels[] = {
				{ c_center, L"center" },
				{ c_bottom_up, L"bottom_up" },
				{ c_top_down, L"top_down" },
			};
		} c_wave_type;

		inline static constexpr struct UpdateMode {
			inline static constexpr int32_t c_off = 0;
			inline static constexpr int32_t c_on = 1;
			inline static constexpr int32_t c_on_without_playing = 2;
			inline static constexpr my::Label labels[] = {
				{ c_off, L"off" },
				{ c_on, L"on" },
				{ c_on_without_playing, L"on_without_playing" },
			};
		} c_update_mode;

		inline static constexpr struct XORMode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_xor = 1;
			inline static constexpr int32_t c_not_xor = 2;
			inline static constexpr int32_t c_not = 3;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_xor, L"xor" },
				{ c_not_xor, L"not_xor" },
				{ c_not, L"not" },
			};
		} c_xor_mode;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND host_window = nullptr;

		//
		// UIプロセスのウィンドウです。
		//
		HWND ui_window = nullptr;

		//
		// AviUtlのファイル情報です。
		//
		AviUtl::FileInfo fi = {};

		//
		// 波形のサイズです。
		//
		int32_t scale = 100;

		//
		// 波形のタイプです。
		//
		int32_t wave_type = c_wave_type.c_bottom_up;

		//
		// 更新モードです。
		//
		int32_t update_mode = c_update_mode.c_on;

		//
		// 波形の描画モードです。
		//
		int32_t xor_mode = c_xor_mode.c_none;

		//
		// 波形の描画に使用するペンの色です。
		//
		COLORREF pen_color = RGB(0, 200, 0);

		//
		// 波形の描画に使用するブラシの色です。
		//
		COLORREF brush_color = RGB(0, 50, 0);

		//
		// TRUEの場合は波形を描画します。
		//
		BOOL show_wave = TRUE;

		//
		// TRUEの場合はアイテム名を描画します。
		//
		BOOL show_text = TRUE;

		//
		// TRUEの場合はアイテム名を常時表示状態にします。
		//
		BOOL namecage = TRUE;

		//
		// TRUEの場合は波形をアイテム名の後ろに描画します。
		//
		BOOL behind = TRUE;

		//
		// 波形を生成するレイヤーです。
		//
		std::unordered_set<int32_t> include_layers;

		//
		// 波形を生成しないレイヤーです。
		//
		std::unordered_set<int32_t> exclude_layers;

		//
		// 波形を生成するフォルダです。
		//
		std::wstring include_folder;

		//
		// 波形を生成しないフォルダです。
		//
		std::wstring exclude_folder;

		//
		// アイテム名の描画位置(X座標)に加算する値です。
		//
		int32_t namecage_offset = -6;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
