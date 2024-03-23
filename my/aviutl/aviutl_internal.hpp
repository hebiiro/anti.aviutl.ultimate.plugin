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
		struct Address
		{
			//
			// このクラスは変数のアドレスを保有します。
			//
			struct Variable
			{
				AviUtl::ExFunc* exfunc = 0;
			} variable;

			//
			// このクラスは関数のアドレスを保有します。
			//
			struct Function
			{
				BOOL (CDECL* get_sys_info)(AviUtl::EditHandle* editp, AviUtl::SysInfo* sip) = 0;
			} function;
		} address;

		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			aviutl = (addr_t)::GetModuleHandle(0);
			if (!aviutl) return FALSE;

			tools::assign(address.variable.exfunc, aviutl + 0x000A8C78);
			tools::assign(address.function.get_sys_info, aviutl + 0x00022120);

			return TRUE;
		}

		//
		// AviUtlのexfuncを取得します。
		//
		// 戻り値
		// AviUtlのexfuncを返します。
		//
		AviUtl::ExFunc* get_exfunc() const { return address.variable.exfunc; }

		//
		// AviUtlシステム情報を取得します。
		//
		// edip [in]
		// AviUtlの編集ハンドルです。
		//
		// sip [out]
		// 取得したシステム情報を格納するポインタです。
		//
		// 戻り値
		// 成功した場合、TRUEを返すと思われます。
		//
		BOOL get_sys_info(AviUtl::EditHandle* editp, AviUtl::SysInfo* sip) const
		{
			return address.function.get_sys_info(editp, sip);
		}

		//
		// フィルタプラグインの数を返します。
		//
		// fp [in]
		// AviUtlのAPIにアクセスするためのフィルタプラグインです。
		//
		// 戻り値
		// フィルタプラグインの数です。
		//
		static int32_t get_filter_plugin_count(AviUtl::FilterPlugin* fp)
		{
			AviUtl::SysInfo si;
			fp->exfunc->get_sys_info(0, &si);
			return si.filter_n;
		}

		//
		// 指定された名前のフィルタプラグインを返します。
		//
		// fp [in]
		// AviUtlのAPIにアクセスするためのフィルタプラグインです。
		//
		// 戻り値
		// フィルタプラグインです。
		//
		static AviUtl::FilterPlugin* get_filter_plugin(AviUtl::FilterPlugin* fp, LPCSTR name)
		{
			for (int32_t i = get_filter_plugin_count(fp) - 1; i >= 0; i--)
			{
				auto filter_plugin = fp->exfunc->get_filterp(i);

				if (!strcmp(filter_plugin->name, name)) return filter_plugin;
			}

			return 0;
		}
	};
}
