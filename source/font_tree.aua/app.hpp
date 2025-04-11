#pragma once

namespace apn::font_tree
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// コンストラクタです。
		//
		App() { app = this; }

		//
		// コンフィグダイアログを表示します。
		//
		virtual BOOL show_config_dialog() override
		{
			// コンフィグダイアログです。
			ConfigDialog dialog;

			// コンフィグダイアログを作成します。
			if (!dialog.init(hive.instance, hive.main_window))
				return FALSE;

			// ダイアログに変数をバインドします。
			dialog.bind_text(IDC_DISPLAY_NAME_FORMAT, hive.display_name_format);
			dialog.bind_text(IDC_SEPARATOR_FORMAT, hive.separator_format);
			dialog.bind_check(IDC_PREVIEW_ENABLED, preview_manager.enabled);
			dialog.bind_check(IDC_PREVIEW_LEFT_SIDE, preview_manager.left_side);
			dialog.bind_int(IDC_PREVIEW_ITEM_SIZE_W, preview_manager.item_size.cx);
			dialog.bind_int(IDC_PREVIEW_ITEM_SIZE_H, preview_manager.item_size.cy);
			dialog.bind_text(IDC_PREVIEW_SAMPLE_FORMAT, preview_manager.sample_format);
			dialog.bind_uint(IDC_PREVIEW_FILL_COLOR, preview_manager.fill_color);
			dialog.bind_uint(IDC_PREVIEW_TEXT_COLOR, preview_manager.text_color);

			// コンフィグダイアログを表示します。
			if (IDOK != dialog.do_modal2(hive.main_window))
				return FALSE;

			return TRUE;
		}

		//
		// xmlファイル(旧形式のデータ)を読み込みます。
		//
		virtual BOOL read_xml() override
		{
			MY_TRACE_FUNC("");

			// ファイル選択ダイアログ用のバッファを確保します。
			std::wstring file_name(MAX_PATH, L'\0');

			// ファイル選択ダイアログ用の構造体を設定します。
			OPENFILENAMEW ofn = { sizeof(ofn) };
			ofn.hwndOwner = hive.main_window;
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrTitle = L"旧形式のデータをインポート";
			ofn.lpstrFile = file_name.data();
			ofn.nMaxFile = file_name.size();
			ofn.lpstrFilter = L"xmlファイル (*.xml)\0*.xml\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"xml";

			// 初期ディレクトリを設定します。
			auto dir = magi.get_assets_file_name(hive.c_name);
			ofn.lpstrInitialDir = dir.c_str();
			MY_TRACE_STR(ofn.lpstrInitialDir);

			// ファイル選択ダイアログを表示します。
			if (!::GetOpenFileNameW(&ofn)) return FALSE;

			// xmlファイル(旧形式のデータ)を読み込みます。
			return read_xml(file_name) == S_OK;
		}

		//
		// xmlノードリストからxmlエレメントを取得して返します。
		//
		inline static MSXML2::IXMLDOMElementPtr get_element(auto& node_list, auto index)
		{
			return node_list->item[index];
		}

		//
		// xmlエレメントから文字列を読み込みます。
		//
		inline static HRESULT read_string(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, auto& out_value)
		{
			try
			{
				auto var = element->getAttribute(name);
				if (var.vt == VT_NULL) return S_FALSE;

				auto bstr = (_bstr_t)var;
				if (!(BSTR)bstr) return S_FALSE;

				out_value = (BSTR)bstr;
			}
			catch (_com_error& e)
			{
				return e.Error();
			}

			return S_OK;
		}

		//
		// xmlエレメントから数値を読み込みます。
		//
		template <typename T>
		inline static HRESULT read_int(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& out_value, int radix = 0)
		{
			try
			{
				auto var = element->getAttribute(name);
				if (var.vt == VT_NULL) return S_FALSE;

				auto bstr = (_bstr_t)var;
				if (!(BSTR)bstr) return S_FALSE;

				out_value = (T)wcstol(bstr, nullptr, radix);
			}
			catch (_com_error& e)
			{
				return e.Error();
			}

			return S_OK;
		}

		//
		// xmlエレメントからブール値を読み込みます。
		//
		inline static HRESULT read_bool(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, auto& out_value)
		{
			try
			{
				auto var = element->getAttribute(name);
				if (var.vt == VT_NULL) return S_FALSE;

				auto bstr = (_bstr_t)var;
				if (!(BSTR)bstr || ::lstrlenW(bstr) == 0) return S_FALSE;

				if (::StrCmpIW(bstr, L"ON") == 0)
				{
					out_value = true;
				}
				else if (::StrCmpIW(bstr, L"OFF") == 0)
				{
					out_value = false;
				}
				else if (::StrCmpIW(bstr, L"YES") == 0)
				{
					out_value = true;
				}
				else if (::StrCmpIW(bstr, L"NO") == 0)
				{
					out_value = false;
				}
				else
				{
					out_value = !!wcstol(bstr, nullptr, 0);
				}
			}
			catch (_com_error& e)
			{
				return e.Error();
			}

			return S_OK;
		}


		//
		// xmlエレメントからカラーを読み込みます。
		//
		inline static HRESULT read_color(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, auto& out_value)
		{
			try
			{
				auto var = element->getAttribute(name);
				if (var.vt == VT_NULL) return S_FALSE;

				auto bstr = (_bstr_t)var;
				if (!(BSTR)bstr) return S_FALSE;

				auto c = ::lstrlenW(bstr);
				if (c == 0) return S_FALSE;

				auto str = (BSTR)bstr;

				if (*str == L'#')
				{
					auto hex = wcstoul(str + 1, nullptr, 16);

					if (c == 4)
					{
						// #fc8

						auto r = (hex & 0x0F00) >> 8;
						auto g = (hex & 0x00F0) >> 4;
						auto b = (hex & 0x000F) >> 0;

						r |= r << 4;
						g |= g << 4;
						b |= b << 4;

						out_value = RGB(r, g, b);
					}
					else
					{
						// #ffcc88

						auto r = (hex & 0x00FF0000) >> 16;
						auto g = (hex & 0x0000FF00) >> 8;
						auto b = (hex & 0x000000FF) >> 0;

						out_value = RGB(r, g, b);
					}
				}
				else
				{
					auto sep1 = ::StrChrW(str, L',');
					if (!sep1) return S_FALSE;

					auto sep2 = ::StrChrW(sep1 + 1, L',');
					if (!sep2) return S_FALSE;

					*sep1 = L'\0';
					*sep2 = L'\0';

					auto r = wcstoul(str, nullptr, 0);
					auto g = wcstoul(sep1 + 1, nullptr, 0);
					auto b = wcstoul(sep2 + 1, nullptr, 0);

					out_value = RGB(r, g, b);
				}
			}
			catch (_com_error& e)
			{
				return e.Error();
			}

			return S_OK;
		}

		//
		// 指定されたxmlファイル(旧形式のデータ)を読み込みます。
		//
		HRESULT read_xml(std::wstring file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			try
			{
				// xmlドキュメントを作成します。
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				// xmlファイルを読み込みます。
				if (document->load(file_name.c_str()) == VARIANT_FALSE)
				{
					MY_TRACE("{/}を読み込めませんでした\n", file_name);

					return S_FALSE;
				}

				// ルートエレメントを取得します。
				auto element = document->documentElement;

				// 既存の設定を消去します。
				recent_manager.clear();
				favorite_manager.clear();

				// <setting>を読み込みます。
				read_setting(element);

				// <preview>を読み込みます。
				read_preview(element);

				// <recent>を読み込みます。
				read_recent(element);

				// <favorite>を読み込みます。
				read_favorite(element);

				MY_TRACE("xmlファイルの読み込みに成功しました\n");

				favorite_manager.expand(TVI_ROOT);
			}
			catch (_com_error& e)
			{
				MY_TRACE(
					"xmlファイルの読み込みに失敗しました\n"
					"{/}\n", e.ErrorMessage());

				return e.Error();
			}

			return S_OK;
		}

		//
		// <setting>を読み込みます。
		//
		HRESULT read_setting(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <setting>を読み込みます。
			auto node_list = element->selectNodes(L"setting");
			auto c = node_list->length;
			for (auto i = 0; i < c; i++)
			{
				auto setting_element = get_element(node_list, i);

				read_string(setting_element, L"separatorFormat", hive.separator_format);
			}

			return S_OK;
		}

		//
		// <preview>を読み込みます。
		//
		HRESULT read_preview(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <preview>を読み込みます。
			auto node_list = element->selectNodes(L"preview");
			auto c = node_list->length;
			for (auto i = 0; i < c; i++)
			{
				auto preview_element = get_element(node_list, i);

				read_bool(preview_element, L"enable", preview_manager.enabled);
				read_bool(preview_element, L"left", preview_manager.left_side);
				read_int(preview_element, L"itemWidth", preview_manager.item_size.cx);
				read_int(preview_element, L"itemHeight", preview_manager.item_size.cy);
//				read_string(preview_element, L"textFormat", preview_manager.sample_format);
				read_color(preview_element, L"fillColor", preview_manager.fill_color);
				read_color(preview_element, L"textColor", preview_manager.text_color);
			}

			return S_OK;
		}

		//
		// <recent>を読み込みます。
		//
		HRESULT read_recent(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <recent>を読み込みます。
			auto node_list = element->selectNodes(L"recent");
			auto c = node_list->length;
			for (auto i = 0; i < c; i++)
			{
				auto recent_element = get_element(node_list, i);

				// <recent>内の<font>を読み込みます。
				auto node_list = recent_element->selectNodes(L"font");
				auto c = node_list->length;
				for (auto i = 0; i < c; i++)
				{
					auto font_element = get_element(node_list, i);

					// <font>のnameを読み込みます。
					std::wstring name;
					read_string(font_element, L"name", name);
					MY_TRACE_STR(name);

					// nameが無効の場合は何もしません。
					if (name.empty()) continue;

					ComboBox_AddString(recent_manager, name.c_str());
				}
			}

			return S_OK;
		}

		//
		// <favorite>を読み込みます。
		//
		HRESULT read_favorite(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <favorite>を読み込みます。
			auto node_list = element->selectNodes(L"favorite");
			auto c = node_list->length;
			for (auto i = 0; i < c; i++)
			{
				auto favorite_element = get_element(node_list, i);

				// 再帰的に<font>を読み込みます。
				read_fonts(favorite_element, TVI_ROOT);
			}

			return S_OK;
		}

		//
		// 再帰的に<font>を読み込みます。
		//
		HRESULT read_fonts(const MSXML2::IXMLDOMElementPtr& element, HTREEITEM parent_handle)
		{
			MY_TRACE_FUNC("");

			// <font>を読み込みます。
			auto node_list = element->selectNodes(L"font");
			auto c = node_list->length;
			for (auto i = 0; i < c; i++)
			{
				auto font_element = get_element(node_list, i);

				// ノードを新規作成します。
				auto node = favorite_manager.create_node();

				// <font>のnameを読み込みます。
				read_string(font_element, L"name", node->name);
				MY_TRACE_STR(node->name);

				// <font>のaliasを読み込みます。
				read_string(font_element, L"alias", node->alias);
				MY_TRACE_STR(node->alias);

				// <font>のexpandを読み込みます。
				read_bool(font_element, L"expand", node->expanded);
				MY_TRACE_INT(node->expanded);

				// お気に入りを追加します。
				auto item_handle = favorite_manager.insert(parent_handle, TVI_LAST, node);

				// 再帰的に子要素を読み込みます。
				read_fonts(font_element, item_handle);
			}

			return S_OK;
		}
	} app_impl;
}
