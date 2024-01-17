#pragma once

namespace fgo::audio_graph
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		struct Label { int value; LPCWSTR label; };

		inline static const LPCWSTR Name = L"AudioGraph";
		inline static const LPCWSTR DisplayName = L"音声グラフ";

		struct CommandID {
			struct State {
				inline static const UINT Off = 1000;
				inline static const UINT On = 1001;
				inline static const UINT OnWithoutPlaying = 1002;
			};
			inline static const UINT Config = 2000;
			struct Window {
				inline static const UINT Begin = 3000;
				inline static const UINT End = 3100;
			};
		};

		struct State {
			inline static const int Off = 0;
			inline static const int On = 1;
			inline static const int OnWithoutPlaying = 2;
			inline static const Label label[] = {
				{ Off, L"Off" },
				{ On, L"On" },
				{ OnWithoutPlaying, L"OnWithoutPlaying" },
			};
		};

		struct Window {
			_bstr_t name;
		};

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		HWND hostWindow = 0; // ホストプロセスのウィンドウです。
		HWND clientWindow = 0; // クライアントプロセスのウィンドウです。

		int state = State::On;
		struct {
			Window left = { L"左レベ" };
			Window right = { L"右レベ" };
		} window;

		template <class... Args>
		int messageBox(Args... args) {
			WCHAR text[MAX_PATH] = {};
			::StringCchPrintfW(text, std::size(text), args...);
			return ::MessageBoxW(mainWindow, text, DisplayName, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
