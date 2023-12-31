#pragma once
#include "Hive.h"
#include "Dark/dark.h"

namespace fgo::font_select
{
	//
	// このクラスは描画を担当します。
	//
	inline struct Paint
	{
		Dark::Api dark; // ダークモード用APIです。

		struct Mode {
			inline static const int Auto = 0;
			inline static const int System = 1;
			inline static const int Theme = 2;
			inline static const int Custom = 3;
			inline static const Hive::Label label[] = {
				{ Auto, L"Auto" },
				{ System, L"System" },
				{ Theme, L"Theme" },
				{ Custom, L"Custom" },
			};
		};
		int mode = Mode::Auto; // 描画モードです。

		struct State {
			_bstr_t name;
			struct {
				COLORREF fill, text, font;
			} color;
		};
		struct StateID {
			inline static const int Normal = 0;
			inline static const int Select = 1;
			inline static const int MaxSize = 2;
		};
		State state[StateID::MaxSize] = {
			{ L"c1_normal", { RGB(0x00, 0x00, 0x00), RGB(0xcc, 0xcc, 0xcc), RGB(0xff, 0xff, 0xff) } }, // 通常の描画色です。
			{ L"c1_selected", { RGB(0x66, 0x66, 0x66), RGB(0xcc, 0xcc, 0xcc), RGB(0xff, 0xff, 0xff) } }, // 選択の描画色です。
		};

		//
		// 初期化処理を行います。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ダークモードAPIを初期化します。
			dark.init();

			return TRUE;
		}

		//
		// 後始末処理を行います。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 引数で指定された条件に合う背景の色を返します。
		//
		COLORREF getFillColor(int stateId)
		{
			BOOL useTheme = mode == Mode::Theme;
			if (mode == Mode::Auto) useTheme = dark.isEnabled();

			if (useTheme) {
				Dark::ColorSet* colorSet = dark.getNamedColor(state[stateId].name);
				if (colorSet) return colorSet->fillColor;
			}
			if (mode == Mode::Custom)
				return state[stateId].color.fill;
			return ::GetSysColor(stateId == StateID::Normal ? COLOR_WINDOW : COLOR_HIGHLIGHT);
		}

		//
		// 引数で指定された条件に合うテキストの色を返します。
		//
		COLORREF getTextColor(int stateId)
		{
			BOOL useTheme = mode == Mode::Theme;
			if (mode == Mode::Auto) useTheme = dark.isEnabled();

			if (useTheme) {
				Dark::ColorSet* colorSet = dark.getNamedColor(state[stateId].name);
				if (colorSet) return colorSet->textForeColor;
			}
			if (mode == Mode::Custom)
				return state[stateId].color.text;
			return ::GetSysColor(COLOR_GRAYTEXT);
		}

		//
		// 引数で指定された条件に合うフォントの色を返します。
		//
		COLORREF getFontColor(int stateId)
		{
			BOOL useTheme = mode == Mode::Theme;
			if (mode == Mode::Auto) useTheme = dark.isEnabled();

			if (useTheme) {
				Dark::ColorSet* colorSet = dark.getNamedColor(state[stateId].name);
				if (colorSet) return colorSet->textForeColor;
			}
			if (mode == Mode::Custom)
				return state[stateId].color.font;
			return ::GetSysColor(stateId == StateID::Normal ? COLOR_WINDOWTEXT : COLOR_HIGHLIGHTTEXT);
		}
	} paint;
}
