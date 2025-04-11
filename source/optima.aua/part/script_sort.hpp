#pragma once

namespace apn::optima
{
	//
	// このクラスはスクリプト並び替えプラグインを改造します。
	//
	inline struct ScriptSort : Part
	{
		//
		// コンストラクタです。
		//
		ScriptSort()
		{
			MY_TRACE_FUNC("");

			part_manager.add(this);
		}

		//
		// 指定されたアドレスにパッチを当てます。
		//
		template <typename T>
		inline static BOOL patch(void* address, const T& from, const T& to)
		{
			constexpr auto size = sizeof(T);
			const auto process = ::GetCurrentProcess();

			// 指定されたアドレスにあるバイナリを取得します。
			T orig = {};
			if (!::ReadProcessMemory(process, address, &orig, size, nullptr)) return FALSE;

			// バイナリがfromではない場合は何もしません。
			if (memcmp(&orig, &from, size) != 0) return FALSE;

			// 指定されたアドレスをtoに書き換えます。
			if (!::WriteProcessMemory(process, address, &to, size, nullptr)) return FALSE;

			return ::FlushInstructionCache(process, address, size);
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			auto script_sort_auf = (uint32_t)::GetModuleHandle(_T("script_sort.auf"));
			if (!script_sort_auf) return FALSE;

			{
				constexpr uint32_t offset = 0x2743;
				constexpr uint8_t from[] = { 0x8B, 0x70, 0x50 };
				constexpr uint8_t to[] = { 0x89, 0xC6, 0x90 };
				const auto address = script_sort_auf + offset;

				if (!patch((LPVOID)address, from, to))
					MY_TRACE("{/hex}を書き換えられませんでした\n", address);
			}

			{
				constexpr uint32_t offset = 0x5B18;
				constexpr uint8_t from[] = { 0x8B, 0x51, 0x50 };
				constexpr uint8_t to[] = { 0x89, 0xCA, 0x90 };
				const auto address = script_sort_auf + offset;

				if (!patch((LPVOID)address, from, to))
					MY_TRACE("{/hex}を書き換えられませんでした\n", address);
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} script_sort;
}
