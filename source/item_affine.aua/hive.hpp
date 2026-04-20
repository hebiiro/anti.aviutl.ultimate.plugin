#pragma once

namespace apn::item_affine
{
	//
	// このクラスは分数です。
	//
	struct fraction_t
	{
		//
		// 分子(numerator)です。
		//
		int32_t num = 0;

		//
		// 分母(denominator)です。
		//
		int32_t den = 1;

		//
		// 分数を実数で返します。
		//
		inline double real() const
		{
			return (double)num / den;
		}
	};

	//
	// このクラスはアフィン変換用の値を保持します。
	//
	struct affine_t {
		//
		// 処理対象です。
		//
		inline static constexpr struct target_t {
			inline static constexpr int32_t c_begin = 0;
			inline static constexpr int32_t c_end = 1;
			inline static constexpr int32_t c_both = 2;
			inline static constexpr my::Label labels[] = {
				{ c_begin, L"begin" },
				{ c_end, L"end" },
				{ c_both, L"both" },
			};
		} c_target;

		//
		// 処理対象です。
		//
		int32_t target = c_target.c_both;

		//
		// 乗算値です。
		//
		fraction_t mul = {};

		//
		// 加算値です。
		//
		fraction_t add = {};

		//
		// 基数です。
		//
		fraction_t base = {};

		//
		// 指数です。
		//
		fraction_t exp = {};
	};

	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t
	{
		inline static constexpr auto c_name = L"item_affine";
		inline static constexpr auto c_display_name = L"アイテム位置変更";

		//
		// アイテムの位置を変更する場合の原点です。
		//
		inline static constexpr struct orig_t {
			inline static constexpr int32_t c_scene_begin = 0;
			inline static constexpr int32_t c_scene_end = 1;
			inline static constexpr int32_t c_item_begin = 2;
			inline static constexpr int32_t c_item_end = 3;
			inline static constexpr int32_t c_current_frame = 4;
			inline static constexpr my::Label labels[] = {
				{ c_scene_begin, L"scene_begin" },
				{ c_scene_end, L"scene_end" },
				{ c_item_begin, L"item_begin" },
				{ c_item_end, L"item_end" },
				{ c_current_frame, L"current_frame" },
			};
		} c_orig;

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
		// アイテムの位置を変更する場合の設定値です。
		//
		affine_t pos;

		//
		// アイテムの長さを変更する場合の設定値です。
		//
		affine_t len;

		//
		// アイテムの位置を変更する場合の原点です。
		//
		int32_t orig = c_orig.c_scene_begin;

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
