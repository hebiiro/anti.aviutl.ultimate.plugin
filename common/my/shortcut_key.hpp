#pragma once

namespace my
{
	//
	// このクラスはショートカットキーです。
	//
	struct ShortcutKey
	{
		inline static constexpr struct Key {
			inline static constexpr Label labels[] = {
				{ VK_LBUTTON, L"LBUTTON" },
				{ VK_RBUTTON, L"RBUTTON" },
				{ VK_CANCEL, L"CANCEL" },
				{ VK_MBUTTON, L"MBUTTON" },
				{ VK_XBUTTON1, L"XBUTTON1" },
				{ VK_XBUTTON2, L"XBUTTON2" },
				{ VK_BACK, L"BACK" },
				{ VK_TAB, L"TAB" },
				{ VK_CLEAR, L"CLEAR" },
				{ VK_RETURN, L"RETURN" },

				{ VK_SHIFT, L"SHIFT" },
				{ VK_CONTROL, L"CONTROL" },
				{ VK_MENU, L"MENU" },
				{ VK_PAUSE, L"PAUSE" },
				{ VK_CAPITAL, L"CAPITAL" },

				{ VK_KANA, L"KANA" },
				{ VK_HANGEUL, L"HANGEUL" },
				{ VK_HANGUL, L"HANGUL" },
				{ VK_IME_ON, L"IME_ON" },
				{ VK_JUNJA, L"JUNJA" },
				{ VK_FINAL, L"FINAL" },
				{ VK_HANJA, L"HANJA" },
				{ VK_KANJI, L"KANJI" },
				{ VK_IME_OFF, L"IME_OFF" },

				{ VK_ESCAPE, L"ESCAPE" },

				{ VK_CONVERT, L"CONVERT" },
				{ VK_NONCONVERT, L"NONCONVERT" },
				{ VK_ACCEPT, L"ACCEPT" },
				{ VK_MODECHANGE, L"MODECHANGE" },

				{ VK_SPACE, L"SPACE" },
				{ VK_PRIOR, L"PRIOR" },
				{ VK_NEXT, L"NEXT" },
				{ VK_END, L"END" },
				{ VK_HOME, L"HOME" },
				{ VK_LEFT, L"LEFT" },
				{ VK_UP, L"UP" },
				{ VK_RIGHT, L"RIGHT" },
				{ VK_DOWN, L"DOWN" },
				{ VK_SELECT, L"SELECT" },
				{ VK_PRINT, L"PRINT" },
				{ VK_EXECUTE, L"EXECUTE" },
				{ VK_SNAPSHOT, L"SNAPSHOT" },
				{ VK_INSERT, L"INSERT" },
				{ VK_DELETE, L"DELETE" },
				{ VK_HELP, L"HELP" },

				{ L'0', L"0" },
				{ L'1', L"1" },
				{ L'2', L"2" },
				{ L'3', L"3" },
				{ L'4', L"4" },
				{ L'5', L"5" },
				{ L'6', L"6" },
				{ L'7', L"7" },
				{ L'8', L"8" },
				{ L'9', L"9" },

				{ L'A', L"A" },
				{ L'B', L"B" },
				{ L'C', L"C" },
				{ L'D', L"D" },
				{ L'E', L"E" },
				{ L'F', L"F" },
				{ L'G', L"G" },
				{ L'H', L"H" },
				{ L'I', L"I" },
				{ L'J', L"J" },
				{ L'K', L"K" },
				{ L'L', L"L" },
				{ L'M', L"M" },
				{ L'N', L"N" },
				{ L'O', L"O" },
				{ L'P', L"P" },
				{ L'Q', L"Q" },
				{ L'R', L"R" },
				{ L'S', L"S" },
				{ L'T', L"T" },
				{ L'U', L"U" },
				{ L'V', L"V" },
				{ L'W', L"W" },
				{ L'X', L"X" },
				{ L'Y', L"Y" },
				{ L'Z', L"Z" },

				{ VK_LWIN, L"LWIN" },
				{ VK_RWIN, L"RWIN" },
				{ VK_APPS, L"APPS" },

				{ VK_SLEEP, L"SLEEP" },

				{ VK_NUMPAD0, L"NUMPAD0" },
				{ VK_NUMPAD1, L"NUMPAD1" },
				{ VK_NUMPAD2, L"NUMPAD2" },
				{ VK_NUMPAD3, L"NUMPAD3" },
				{ VK_NUMPAD4, L"NUMPAD4" },
				{ VK_NUMPAD5, L"NUMPAD5" },
				{ VK_NUMPAD6, L"NUMPAD6" },
				{ VK_NUMPAD7, L"NUMPAD7" },
				{ VK_NUMPAD8, L"NUMPAD8" },
				{ VK_NUMPAD9, L"NUMPAD9" },
				{ VK_MULTIPLY, L"MULTIPLY" },
				{ VK_ADD, L"ADD" },
				{ VK_SEPARATOR, L"SEPARATOR" },
				{ VK_SUBTRACT, L"SUBTRACT" },
				{ VK_DECIMAL, L"DECIMAL" },
				{ VK_DIVIDE, L"DIVIDE" },
				{ VK_F1, L"F1" },
				{ VK_F2, L"F2" },
				{ VK_F3, L"F3" },
				{ VK_F4, L"F4" },
				{ VK_F5, L"F5" },
				{ VK_F6, L"F6" },
				{ VK_F7, L"F7" },
				{ VK_F8, L"F8" },
				{ VK_F9, L"F9" },
				{ VK_F10, L"F10" },
				{ VK_F11, L"F11" },
				{ VK_F12, L"F12" },
				{ VK_F13, L"F13" },
				{ VK_F14, L"F14" },
				{ VK_F15, L"F15" },
				{ VK_F16, L"F16" },
				{ VK_F17, L"F17" },
				{ VK_F18, L"F18" },
				{ VK_F19, L"F19" },
				{ VK_F20, L"F20" },
				{ VK_F21, L"F21" },
				{ VK_F22, L"F22" },
				{ VK_F23, L"F23" },
				{ VK_F24, L"F24" },

				{ VK_NAVIGATION_VIEW, L"NAVIGATION_VIEW" },
				{ VK_NAVIGATION_MENU, L"NAVIGATION_MENU" },
				{ VK_NAVIGATION_UP, L"NAVIGATION_UP" },
				{ VK_NAVIGATION_DOWN, L"NAVIGATION_DOWN" },
				{ VK_NAVIGATION_LEFT, L"NAVIGATION_LEFT" },
				{ VK_NAVIGATION_RIGHT, L"NAVIGATION_RIGHT" },
				{ VK_NAVIGATION_ACCEPT, L"NAVIGATION_ACCEPT" },
				{ VK_NAVIGATION_CANCEL, L"NAVIGATION_CANCEL" },

				{ VK_NUMLOCK, L"NUMLOCK" },
				{ VK_SCROLL, L"SCROLL" },

				{ VK_OEM_NEC_EQUAL, L"OEM_NEC_EQUAL" },

				{ VK_OEM_FJ_JISHO, L"OEM_FJ_JISHO" },
				{ VK_OEM_FJ_MASSHOU, L"OEM_FJ_MASSHOU" },
				{ VK_OEM_FJ_TOUROKU, L"OEM_FJ_TOUROKU" },
				{ VK_OEM_FJ_LOYA, L"OEM_FJ_LOYA" },
				{ VK_OEM_FJ_ROYA, L"OEM_FJ_ROYA" },

				{ VK_LSHIFT, L"LSHIFT" },
				{ VK_RSHIFT, L"RSHIFT" },
				{ VK_LCONTROL, L"LCONTROL" },
				{ VK_RCONTROL, L"RCONTROL" },
				{ VK_LMENU, L"LMENU" },
				{ VK_RMENU, L"RMENU" },

				{ VK_BROWSER_BACK, L"BROWSER_BACK" },
				{ VK_BROWSER_FORWARD, L"BROWSER_FORWARD" },
				{ VK_BROWSER_REFRESH, L"BROWSER_REFRESH" },
				{ VK_BROWSER_STOP, L"BROWSER_STOP" },
				{ VK_BROWSER_SEARCH, L"BROWSER_SEARCH" },
				{ VK_BROWSER_FAVORITES, L"BROWSER_FAVORITES" },
				{ VK_BROWSER_HOME, L"BROWSER_HOME" },

				{ VK_VOLUME_MUTE, L"VOLUME_MUTE" },
				{ VK_VOLUME_DOWN, L"VOLUME_DOWN" },
				{ VK_VOLUME_UP, L"VOLUME_UP" },
				{ VK_MEDIA_NEXT_TRACK, L"MEDIA_NEXT_TRACK" },
				{ VK_MEDIA_PREV_TRACK, L"MEDIA_PREV_TRACK" },
				{ VK_MEDIA_STOP, L"MEDIA_STOP" },
				{ VK_MEDIA_PLAY_PAUSE, L"MEDIA_PLAY_PAUSE" },
				{ VK_LAUNCH_MAIL, L"LAUNCH_MAIL" },
				{ VK_LAUNCH_MEDIA_SELECT, L"LAUNCH_MEDIA_SELECT" },
				{ VK_LAUNCH_APP1, L"LAUNCH_APP1" },
				{ VK_LAUNCH_APP2, L"LAUNCH_APP2" },

				{ VK_OEM_1, L"OEM_1" },
				{ VK_OEM_PLUS, L"OEM_PLUS" },
				{ VK_OEM_COMMA, L"OEM_COMMA" },
				{ VK_OEM_MINUS, L"OEM_MINUS" },
				{ VK_OEM_PERIOD, L"OEM_PERIOD" },
				{ VK_OEM_2, L"OEM_2" },
				{ VK_OEM_3, L"OEM_3" },

				{ VK_GAMEPAD_A, L"GAMEPAD_A" },
				{ VK_GAMEPAD_B, L"GAMEPAD_B" },
				{ VK_GAMEPAD_X, L"GAMEPAD_X" },
				{ VK_GAMEPAD_Y, L"GAMEPAD_Y" },
				{ VK_GAMEPAD_RIGHT_SHOULDER, L"GAMEPAD_RIGHT_SHOULDER" },
				{ VK_GAMEPAD_LEFT_SHOULDER, L"GAMEPAD_LEFT_SHOULDER" },
				{ VK_GAMEPAD_LEFT_TRIGGER, L"GAMEPAD_LEFT_TRIGGER" },
				{ VK_GAMEPAD_RIGHT_TRIGGER, L"GAMEPAD_RIGHT_TRIGGER" },
				{ VK_GAMEPAD_DPAD_UP, L"GAMEPAD_DPAD_UP" },
				{ VK_GAMEPAD_DPAD_DOWN, L"GAMEPAD_DPAD_DOWN" },
				{ VK_GAMEPAD_DPAD_LEFT, L"GAMEPAD_DPAD_LEFT" },
				{ VK_GAMEPAD_DPAD_RIGHT, L"GAMEPAD_DPAD_RIGHT" },
				{ VK_GAMEPAD_MENU, L"GAMEPAD_MENU" },
				{ VK_GAMEPAD_VIEW, L"GAMEPAD_VIEW" },
				{ VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON, L"GAMEPAD_LEFT_THUMBSTICK_BUTTON" },
				{ VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON, L"GAMEPAD_RIGHT_THUMBSTICK_BUTTON" },
				{ VK_GAMEPAD_LEFT_THUMBSTICK_UP, L"GAMEPAD_LEFT_THUMBSTICK_UP" },
				{ VK_GAMEPAD_LEFT_THUMBSTICK_DOWN, L"GAMEPAD_LEFT_THUMBSTICK_DOWN" },
				{ VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT, L"GAMEPAD_LEFT_THUMBSTICK_RIGHT" },
				{ VK_GAMEPAD_LEFT_THUMBSTICK_LEFT, L"GAMEPAD_LEFT_THUMBSTICK_LEFT" },
				{ VK_GAMEPAD_RIGHT_THUMBSTICK_UP, L"GAMEPAD_RIGHT_THUMBSTICK_UP" },
				{ VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN, L"GAMEPAD_RIGHT_THUMBSTICK_DOWN" },
				{ VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT, L"GAMEPAD_RIGHT_THUMBSTICK_RIGHT" },
				{ VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT, L"GAMEPAD_RIGHT_THUMBSTICK_LEFT" },

				{ VK_OEM_4, L"OEM_4" },
				{ VK_OEM_5, L"OEM_5" },
				{ VK_OEM_6, L"OEM_6" },
				{ VK_OEM_7, L"OEM_7" },
				{ VK_OEM_8, L"OEM_8" },

				{ VK_OEM_AX, L"OEM_AX" },
				{ VK_OEM_102, L"OEM_102" },
				{ VK_ICO_HELP, L"ICO_HELP" },
				{ VK_ICO_00, L"ICO_00" },

				{ VK_PROCESSKEY, L"PROCESSKEY" },

				{ VK_ICO_CLEAR, L"ICO_CLEAR" },

				{ VK_PACKET, L"PACKET" },

				{ VK_OEM_RESET, L"OEM_RESET" },
				{ VK_OEM_JUMP, L"OEM_JUMP" },
				{ VK_OEM_PA1, L"OEM_PA1" },
				{ VK_OEM_PA2, L"OEM_PA2" },
				{ VK_OEM_PA3, L"OEM_PA3" },
				{ VK_OEM_WSCTRL, L"OEM_WSCTRL" },
				{ VK_OEM_CUSEL, L"OEM_CUSEL" },
				{ VK_OEM_ATTN, L"OEM_ATTN" },
				{ VK_OEM_FINISH, L"OEM_FINISH" },
				{ VK_OEM_COPY, L"OEM_COPY" },
				{ VK_OEM_AUTO, L"OEM_AUTO" },
				{ VK_OEM_ENLW, L"OEM_ENLW" },
				{ VK_OEM_BACKTAB, L"OEM_BACKTAB" },

				{ VK_ATTN, L"ATTN" },
				{ VK_CRSEL, L"CRSEL" },
				{ VK_EXSEL, L"EXSEL" },
				{ VK_EREOF, L"EREOF" },
				{ VK_PLAY, L"PLAY" },
				{ VK_ZOOM, L"ZOOM" },
				{ VK_NONAME, L"NONAME" },
				{ VK_PA1, L"PA1" },
				{ VK_OEM_CLEAR, L"OEM_CLEAR" },
			};
		} c_key;

		inline static constexpr struct Mod {
			inline static constexpr DWORD c_shift = 0x01;
			inline static constexpr DWORD c_ctrl = 0x02;
			inline static constexpr DWORD c_alt = 0x04;
			inline static constexpr DWORD c_win = 0x08;
			inline static constexpr LabelT<DWORD> labels[] = {
				{ c_shift, L"SHIFT" },
				{ c_ctrl, L"CTRL" },
				{ c_alt, L"ALT" },
				{ c_win, L"WIN" },
			};
		} c_mod;

		int key = 0;
		DWORD mod = 0;

		//
		// コンストラクタです。
		//
		ShortcutKey(int key, DWORD mod)
			: key(key)
			, mod(mod)
		{
		}

		//
		// コンストラクタです。
		//
		ShortcutKey(const std::wstring& key, const std::wstring& mod)
			: key(str_to_key(key))
			, mod(str_to_mod(mod))
		{
		}

		//
		// このショートカットキーが活性化している場合はTRUEを返します。
		//
		BOOL is_active(int key)
		{
			if (key != this->key) return FALSE;
			if (mod & c_mod.c_shift && !is_key_down(VK_SHIFT)) return FALSE;
			if (mod & c_mod.c_ctrl && !is_key_down(VK_CONTROL)) return FALSE;
			if (mod & c_mod.c_alt && !is_key_down(VK_MENU)) return FALSE;
			if (mod & c_mod.c_win && !is_key_down(VK_LWIN, VK_RWIN)) return FALSE;

			return TRUE;
		}

		//
		// 指定されたキーが押されている場合はTRUEを返します。
		//
		inline static BOOL is_key_down(int key)
		{
			return ::GetKeyState(key) < 0;
		}

		//
		// 指定されたキーのいずれかが押されている場合はTRUEを返します。
		//
		template <typename... Tail>
		inline static BOOL is_key_down(int key, Tail&&... tail)
		{
			if (is_key_down(key))
				return TRUE;
			else
				return is_key_down(std::forward<Tail>(tail)...);
		}

		//
		// 文字列を仮想キーコードに変換して返します。
		//
		static int str_to_key(const std::wstring& str)
		{
			std::wstring s(str);
			std::transform(s.begin(), s.end(), s.begin(), toupper);

			for (const auto& label : c_key.labels)
			{
				if (s == label.text)
					return label.value;
			}

			return 0;
		}

		//
		// 文字列を修飾子に変換して返します。
		//
		static DWORD str_to_mod(const std::wstring& str)
		{
			std::wstring s(str);
			std::transform(s.begin(), s.end(), s.begin(), toupper);

			DWORD mod = 0;

			for (const auto& label : c_mod.labels)
			{
				if (s.find(label.text) != s.npos)
					mod |= label.value;
			}

			return mod;
		}

		//
		// 仮想キーコードを文字列に変換して返します。
		//
		static std::wstring key_to_str(int key)
		{
			for (const auto& label : c_key.labels)
			{
				if (key == label.value)
					return label.text;
			}

			return L"";
		}

		//
		// 修飾子を文字列に変換して返します。
		//
		static std::wstring mod_to_str(DWORD mod)
		{
			std::wstring s(L"");

			for (const auto& label : c_mod.labels)
			{
				if (mod & label.value)
				{
					if (s.length()) s += L" ";
					s += label.text;
				}
			}

			return s;
		}
	};
}
