#pragma once

namespace apn::editbox_tweaker
{
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
			using namespace my::json;

			get_bool(root.get_child("unicode_input"), hive.unicode_input);
			get_bool(root.get_child("ctrl_a"), hive.ctrl_a);
			get_bool(root.get_child("zoomable"), hive.zoomable);
			get_int(root.get_child("delta"), hive.delta);
			get_string(root.get_child("font.name"), hive.font.name);
			get_int(root.get_child("font.height"), hive.font.height);
			get_int(root.get_child("font.pitch"), hive.font.pitch);

			return TRUE;
		}

		//
		// コンフィグをファイルに保存します。
		//
		virtual BOOL on_save(std::ofstream& ofs) override
		{
			using namespace my::json;

			size_t indent = 0;

			ofs << with_eol(indent++, R"({)"s);
			{
				ofs << with_eol(indent, std::format(R"("unicode_input": {},)", from_bool(hive.unicode_input)));
				ofs << with_eol(indent, std::format(R"("ctrl_a": {},)", from_bool(hive.ctrl_a)));
				ofs << with_eol(indent, std::format(R"("zoomable": {},)", from_bool(hive.zoomable)));
				ofs << with_eol(indent, std::format(R"("delta": {},)", hive.delta));

				ofs << with_eol(indent++, R"("font": {)"s);
				{
					ofs << with_eol(indent, std::format(R"("name": {},)", from_string(hive.font.name)));
					ofs << with_eol(indent, std::format(R"("height": {},)", hive.font.height));
					ofs << with_eol(indent, std::format(R"("pitch": {})", hive.font.pitch));
				}
				ofs << with_eol(--indent, R"(})"s);
			}
			ofs << with_eol(--indent, R"(})"s);

			return TRUE;
		}

		virtual int message_box(const std::wstring& text) override
		{
			return hive.message_box(text);
		}
	} config_io;
}
