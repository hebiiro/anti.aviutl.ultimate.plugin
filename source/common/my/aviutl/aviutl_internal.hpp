#pragma once

namespace my
{
	//
	// このクラスはAviUtlにアクセスします。
	//
	struct AviUtlInternal
	{
		//
		// アドレスの型です。
		//
		using addr_t = uintptr_t;

		//
		// AviUtlのベースアドレスです。
		//
		addr_t aviutl = 0;

		//
		// このクラスはAviUtlのアドレスを保有します。
		//
		struct Address {
			//
			// このクラスは変数のアドレスを保有します。
			//
			struct Variable {
				AviUtl::ExFunc* exfunc = nullptr;
				BOOL* is_playing = nullptr;
				BOOL* is_playing_main = nullptr;
			} variable;

			//
			// このクラスは関数のアドレスを保有します。
			//
			struct Function {
				BOOL (CDECL* get_sys_info)(AviUtl::EditHandle* editp, AviUtl::SysInfo* sip) = nullptr;
			} function;
		} address;

		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			aviutl = (addr_t)::GetModuleHandle(nullptr);
			if (!aviutl) return FALSE;

			tools::assign(address.variable.exfunc, aviutl + 0x000A8C78);
			tools::assign(address.variable.is_playing, aviutl + 0x002C4DBC);
			tools::assign(address.variable.is_playing_main, aviutl + 0x002E0698);
			tools::assign(address.function.get_sys_info, aviutl + 0x00022120);

			return TRUE;
		}

		//
		// AviUtlのexfuncを返します。
		//
		AviUtl::ExFunc* get_exfunc() const { return address.variable.exfunc; }

		//
		// 再生中の場合はTRUEを返します。
		//
		BOOL is_playing() const { return *address.variable.is_playing; }

		//
		// メインウィンドウで再生中の場合はTRUEを返します。
		//
		BOOL is_playing_main() const { return *address.variable.is_playing_main; }

		//
		// AviUtlのシステム情報を返します。
		//
		BOOL get_sys_info(AviUtl::EditHandle* editp, AviUtl::SysInfo* sip) const
		{
			return address.function.get_sys_info(editp, sip);
		}

		//
		// フィルタプラグインの数を返します。
		//
		inline static int32_t get_filter_plugin_count(AviUtl::FilterPlugin* fp)
		{
			AviUtl::SysInfo si = {};
			fp->exfunc->get_sys_info(nullptr, &si);
			return si.filter_n;
		}

		//
		// 指定された名前のフィルタプラグインを返します。
		//
		inline static AviUtl::FilterPlugin* get_filter_plugin(AviUtl::FilterPlugin* fp, LPCSTR name)
		{
			for (int32_t i = get_filter_plugin_count(fp) - 1; i >= 0; i--)
			{
				auto filter_plugin = fp->exfunc->get_filterp(i);

				if (!::lstrcmpA(filter_plugin->name, name)) return filter_plugin;
			}

			return nullptr;
		}
	};
}
