#pragma once

namespace apn::item_align
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t
	{
		inline static constexpr auto c_name = L"item_align";
		inline static constexpr auto c_display_name = L"アイテム整列";

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
		HWND main_window = nullptr;

		//
		// TRUEの場合は現在フレームを基準にします。
		//
		BOOL flag_use_current_frame = FALSE;

		//
		// 各要素の時間です。
		//
		struct time_t
		{
			inline static constexpr struct mode_t {
				inline static constexpr int32_t c_none = 0;
				inline static constexpr int32_t c_frame = 1;
				inline static constexpr int32_t c_bpm = 2;
				inline static constexpr my::Label labels[] = {
					{ c_none, L"none" },
					{ c_frame, L"frame" },
					{ c_bpm, L"bpm" },
				};
			} c_mode;

			//
			// 副時間モードです。
			//
			int32_t mode = c_mode.c_none;

			//
			// 時間要素のインデックスです。
			//
			inline static constexpr struct index_t {
				inline static constexpr size_t c_align = 0;
				inline static constexpr size_t c_stretch = 1;
				inline static constexpr size_t c_relative_space = 2;
				inline static constexpr size_t c_absolute_space = 3;
				inline static constexpr size_t c_shift = 4;
				inline static constexpr size_t c_max_size = 5;
				inline static constexpr my::Label labels[] = {
					{ c_align, L"align" },
					{ c_stretch, L"stretch" },
					{ c_relative_space, L"relative_space" },
					{ c_absolute_space, L"absolute_space" },
					{ c_shift, L"shift" },
				};
			} c_index;

			//
			// 時間要素です。
			//
			struct element_t
			{
				//
				// 主時間(秒単位)です。
				//
				std::wstring sec;

				//
				// 副時間(フレームまたはBPM単位)です。
				//
				union {
					int32_t frame;
					int32_t bpm;
				};
			} elements[c_index.c_max_size] =
			{
				{ L"0.5", 0 }, // アイテムを寄せる量(時間)です。
				{ L"0.5", 0 }, // アイテムを伸ばす量(時間)です。
				{ L"0.5", 0 }, // 相対スペース量(時間)です。
				{ L"1", 0 }, // 絶対スペース量(時間)です。
				{ L"0.5", 0 }, // アイテムをずらす量(時間)です。
			};
		} time;

		//
		// アイテムを詰める量(レイヤー数)です。
		//
		int32_t move_vert_layer = 2;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
