#pragma once
#include "Design.h"

namespace fgo::audio_graph_gui
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		struct HitTest {
			inline static const int None = 0;
			inline static const int MinRange = 1;
			inline static const int MaxRange = 2;
		};

		HINSTANCE instance = 0;
		HWND hostWindow = 0;
		Mutex mutex;
		SimpleFileMapping fileMapping;
		WCHAR configFileName[MAX_PATH] = {};

		double level[2] = {};
		double peak[2] = {};

		int minRange = -34;
		int maxRange = 14;

		struct Drag {
			int ht = HitTest::None;
			POINT origPoint = {};
			int origValue = 0;
		} drag;

		BOOL init(HWND hostWindow)
		{
			MY_TRACE_FUNC("0x%08X", hostWindow);

			this->hostWindow = hostWindow;

			mutex.init(0, FALSE, FormatText(Share::AudioGraph::MutexFormat, hostWindow));
			fileMapping.init(sizeof(Share::AudioGraph::Volume), FormatText(Share::AudioGraph::FileMappingFormat, hostWindow));

			WCHAR fileName[MAX_PATH] = {};
			::GetModuleFileNameW(0, fileName, std::size(fileName));
			::PathAppendW(fileName, L"../../UltimateConfig/AudioGraph.ini");
			::PathCanonicalizeW(configFileName, fileName);
			MY_TRACE_WSTR(configFileName);

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		BOOL load()
		{
			MY_TRACE_FUNC("");

			design.load(configFileName);

			getPrivateProfileInt(configFileName, L"Config", L"minRange", minRange);
			getPrivateProfileInt(configFileName, L"Config", L"maxRange", maxRange);

			return TRUE;
		}

		BOOL save()
		{
			MY_TRACE_FUNC("");

			setPrivateProfileInt(configFileName, L"Config", L"minRange", minRange);
			setPrivateProfileInt(configFileName, L"Config", L"maxRange", maxRange);

			design.save(configFileName);

			return TRUE;
		}
	} hive;
}
