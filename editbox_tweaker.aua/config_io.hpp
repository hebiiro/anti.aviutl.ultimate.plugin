#pragma once

namespace apn::editbox_tweaker
{
	using namespace my::json;

	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			if (!__super::init(hive.instance)) return FALSE;

			if (!hive.font.name.empty())
			{
				// HFONTを作成します。
				// 複数行エディットボックスのWM_SETFONTでこのハンドルが渡されます。
				hive.font.handle = decltype(hive.font.handle)(
					::CreateFontW(hive.font.height, 0,
						0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
						0, 0, 0, hive.font.pitch, hive.font.name.c_str()), &::DeleteObject);
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			return __super::exit(hive.instance);
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_load(ptree& root) override
		{
			get_bool(root, "unicode_input", hive.unicode_input);
			get_bool(root, "ctrl_a", hive.ctrl_a);
			get_bool(root, "zoomable", hive.zoomable);
			get_int(root, "delta", hive.delta);
			get_string(root, "font.name", hive.font.name);
			get_int(root, "font.height", hive.font.height);
			get_int(root, "font.pitch", hive.font.pitch);

			return TRUE;
		}

		//
		// コンフィグをファイルに保存します。
		//
		virtual BOOL on_save(std::ofstream& ofs) override
		{
			ptree root;

			set_bool(root, "unicode_input", hive.unicode_input);
			set_bool(root, "ctrl_a", hive.ctrl_a);
			set_bool(root, "zoomable", hive.zoomable);
			set_int(root, "delta", hive.delta);
			set_string(root, "font.name", hive.font.name);
			set_int(root, "font.height", hive.font.height);
			set_int(root, "font.pitch", hive.font.pitch);

			write_json(ofs, root);

			return TRUE;
		}

		virtual int message_box(const std::wstring& text) override
		{
			return hive.message_box(text);
		}
	} config_io;
}
