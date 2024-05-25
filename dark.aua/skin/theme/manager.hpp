#pragma once

namespace apn::dark::skin::theme
{
	//
	// このクラスはビジュアルスタイルのクラス・パーツ・ステートを管理します。
	//
	inline struct Manager
	{
		struct Info
		{
			std::wstring vsclass_name;
			BOOL for_dpi;

			bool operator<(const Info& other) const
			{
				if (for_dpi == other.for_dpi)
					return vsclass_name < other.vsclass_name;
				else
					return for_dpi < other.for_dpi;
			}

			bool operator==(const Info& other) const
			{
				return vsclass_name == other.vsclass_name && for_dpi == other.for_dpi;
			}

			struct Hash
			{
				std::size_t operator()(const Info& info) const
				{
					std::size_t h1 = std::hash<std::wstring>()(info.vsclass_name);
					std::size_t h2 = std::hash<BOOL>()(info.for_dpi);
					return h1 ^ h2;
				}
			};
		};

		//
		// このクラスはビジュアルスタイルクラスのコレクションです。
		//
		struct {
			std::unordered_map<HTHEME, std::shared_ptr<VSClass>> from_handle;
			std::unordered_map<Info, std::shared_ptr<VSClass>, Info::Hash> from_info;
		} vsclasses;

		inline static HWND get_theme_window()
		{
			return hive.theme_window;
		}

		inline static auto get_dpi()
		{
			return ::GetDpiForWindow(hive.theme_window);
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			clear();

			return TRUE;
		}

		//
		// すべてのビジュアルスタイルを削除します。
		//
		void clear()
		{
			MY_TRACE_FUNC("");

			vsclasses.from_handle.clear();
			vsclasses.from_info.clear();
		}

		//
		// ビジュアルスタイルクラスを作成して返します。
		//
		auto create_vsclass(LPCWSTR vsclass_name, BOOL for_dpi = FALSE)
		{
			auto info = Info { vsclass_name, for_dpi };

			// 既存のビジュアルスタイルクラスが存在しない場合は
			auto vsclass = vsclasses.from_info[info];
			if (!vsclass)
			{
				// テーマハンドルを開きます。
				my::theme::unique_ptr<> theme(for_dpi ?
					open_theme_data_for_dpi(vsclass_name) : open_theme_data(vsclass_name));

				// ビジュアルスタイルクラスのインスタンスを作成します。
				// ここで渡されたテーマハンドルはビジュアルスタイルクラスが持つ
				// スマートポインタによって閉じられます。
				vsclass = std::make_shared<VSClass>(theme.release(), vsclass_name);

				// テーマハンドルからビジュアルスタイルクラスを取得できるようにしておきます。
				vsclasses.from_handle[vsclass->theme.get()] = vsclass;

				// ビジュアルスタイルクラス情報からビジュアルスタイルクラスを取得できるようにしておきます。
				vsclasses.from_info[info] = vsclass;
			}

			return vsclass;
		}

		//
		// 指定された名前のテーマハンドルを返します。
		//
		HTHEME get_theme(LPCWSTR vsclass_name)
		{
			auto vsclass = get_vsclass(vsclass_name);
			if (!vsclass) return nullptr;
			return vsclass->theme.get();
		}

		//
		// 指定された名前のビジュアルスタイルクラスを返します。
		//
		std::shared_ptr<VSClass>& get_vsclass(LPCWSTR vsclass_name)
		{
			return vsclasses.from_info[{ vsclass_name, FALSE }];
		}

		//
		// 指定されたテーマハンドルのビジュアルスタイルクラスを返します。
		//
		std::shared_ptr<VSClass>& get_vsclass(HTHEME theme)
		{
			return vsclasses.from_handle[theme];
		}

		//
		// 指定された条件のステートを返します。
		//
		State* get_state(HTHEME theme, int part_id, int state_id)
		{
			auto vsclass = get_vsclass(theme);
			if (!vsclass) return nullptr;
			return &vsclass->parts[part_id].states[state_id];
		}

		//
		// 指定された条件のステートを返します。
		//
		State* get_state(LPCWSTR vsclass_name, int part_id, int state_id)
		{
			auto vsclass = get_vsclass(vsclass_name);
			if (!vsclass) return nullptr;
			return &vsclass->parts[part_id].states[state_id];
		}

		//
		// 指定されたビジュアルスタイルクラス名のテーマハンドルを開きます。
		//
		HTHEME open_theme_data(LPCWSTR vsclass_name)
		{
			auto hwnd = get_theme_window();

			return ::OpenThemeData(hwnd, vsclass_name);
		}

		//
		// 指定されたビジュアルスタイルクラス名のテーマハンドルを開きます。
		//
		HTHEME open_theme_data_for_dpi(LPCWSTR vsclass_name)
		{
			auto hwnd = get_theme_window();

			return ::OpenThemeDataForDpi(hwnd, vsclass_name, get_dpi());
		}

		//
		// 指定されたテーマハンドルを閉じます。
		//
		HRESULT close_theme_data(HTHEME theme)
		{
			return ::CloseThemeData(theme);
		}

		//
		// 指定されたビジュアルスタイルのテーマハンドルを返します。
		//
		HTHEME get_theme_data(LPCWSTR vsclass_name, BOOL for_dpi)
		{
			return create_vsclass(vsclass_name, for_dpi)->theme.get();
		}

		//
		// 塗りつぶし用ブラシを返します。
		//
		HBRUSH get_fill_brush(HDC dc, HBRUSH default_brush, HTHEME theme, int part_id, int state_id)
		{
			if (auto state = skin::theme::manager.get_state(theme, part_id, state_id))
				if (auto brush = state->get_fill_brush()) return brush;
//				if (auto brush = state->get_fill_brush()) return ::SetBkColor(dc, state->stuff.fill.color), brush;

			return default_brush;
		}
	} manager;
}
