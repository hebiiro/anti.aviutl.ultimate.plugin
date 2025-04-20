#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはアクションです。
	//
	inline struct Action : ActionInterface
	{
		//
		// カスタムオブジェクトをexoファイルに書き込みます。
		//
		inline static BOOL write_custom_object(const std::shared_ptr<Agit::List::Node>& custom_object, const std::wstring& file_name)
		{
			// exaファイル用のスクリプト名を取得します。
			auto script_name = my::hs(custom_object->script_name);
			if (custom_object->file_name[0] == L'@')
				script_name += my::hs(custom_object->file_name);

			auto type = 0;
			auto filter = 2;

			// 標準カスタムオブジェクトの場合は
			auto it = agit.embedded.map.find(custom_object->script_name);
			if (it != agit.embedded.map.end())
			{
				// typeにインデックスを指定します。
				type = it->second.index;
				filter = 0;
				script_name.clear();
			}

			// 現在のプロジェクト情報を取得します。
			AviUtl::FileInfo fi = {};
			magi.fp->exfunc->get_file_info(magi.exin.get_editp(), &fi);

			// ファイル出力ストリームを開きます。
			std::ofstream ofs(file_name, std::ios::binary);

			// 拡張編集データを書き込みます。
			ofs << my::format(
				"[exedit]" "\r\n"
				"width={/}" "\r\n"
				"height={/}" "\r\n"
				"rate={/}" "\r\n"
				"scale={/}" "\r\n"
				"length={/}" "\r\n"
				"audio_rate={/}" "\r\n"
				"audio_ch={/}" "\r\n",
				fi.w,
				fi.h,
				fi.video_rate,
				fi.video_scale,
				fi.frame_n,
				fi.audio_rate,
				fi.audio_ch);

			// オブジェクトデータを書き込みます。
			ofs << my::format(
				"[0]" "\r\n"
				"start=1" "\r\n"
				"end={/}" "\r\n"
				"layer=1" "\r\n"
				"overlay=1" "\r\n"
				"camera=0" "\r\n",
				hive.item_length);

			// カスタムオブジェクトフィルタデータを書き込みます。
			ofs << my::format(
				"[0.0]" "\r\n"
				"_name=カスタムオブジェクト" "\r\n"
				"type={/}" "\r\n"
				"filter={/}" "\r\n"
				"name={/}" "\r\n",
				type,
				filter,
				script_name);

			// 標準描画フィルタデータを書き込みます。
			ofs << my::format(
				"[0.1]" "\r\n"
				"_name=標準描画" "\r\n"
				"X=0.0" "\r\n"
				"Y=0.0" "\r\n"
				"Z=0.0" "\r\n"
				"拡大率=100.00" "\r\n"
				"透明度=0.0" "\r\n"
				"回転=0.00" "\r\n"
				"blend=0" "\r\n");

			return TRUE;
		}

		//
		// カスタムオブジェクトのリストをリフレッシュします。
		// スクリプトのファイル名が指定されている場合は該当スクリプトだけをリフレッシュします。
		//
		virtual BOOL refresh(const std::wstring& file_name) override
		{
			MY_TRACE_FUNC("{/}", file_name);

			// ファイル名が同一のリストノードを走査します。
			for (auto& node : agit.list.collection.raw.vec)
			{
				// スクリプトのファイル名が指定されている場合は
				if (file_name.length())
				{
					// ファイル名が異なる場合は何もしません。
					if (node->file_name != file_name) continue;
				}

				// リストノードのファイルタイムを更新します。
				node->file_time = get_file_time_string(get_script_path(node, L".obj"));
			}

			// リストビューを更新します。
			list_dialog.update_list_view();

			// 拡張編集のカスタムオブジェクトを再初期化します。
			reinit_exedit_custom_objects();

			return TRUE;
		}

		//
		// 指定されたカスタムオブジェクトをインストールします。
		//
		virtual BOOL install(const std::shared_ptr<Agit::List::Node>& custom_object) override
		{
			MY_TRACE_FUNC("{/}", custom_object->distributor);

			{
				// インストールコマンドのカレントディレクトリをaviutlフォルダにします。
				auto dir_path = my::get_module_file_name(nullptr).parent_path();

				// インストール用サブプロセスを起動します。
				SubProcess sub_process(dir_path, custom_object->distributor);

				// サブプロセスが終了するまで待機します。
				sub_process.wait();
			}

			// カスタムオブジェクトのリストをリフレッシュします。
			return refresh(custom_object->file_name);
		}

		//
		// 指定されたカスタムオブジェクトをアンインストールします。
		//
		virtual BOOL uninstall(const std::shared_ptr<Agit::List::Node>& custom_object) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 指定されたカスタムオブジェクトのスクリプトファイルを開きます。
		//
		virtual BOOL open_script_file(const std::shared_ptr<Agit::List::Node>& custom_object) override
		{
			MY_TRACE_FUNC("");

			if (custom_object->file_name.empty()) return FALSE;

			return shell_execute(_T("open"),
				get_default_text_editor().c_str(),
				get_script_path(custom_object, L".obj").c_str());
		}

		//
		// 指定されたカスタムオブジェクトのスクリプトフォルダを開きます。
		//
		virtual BOOL open_script_folder(const std::shared_ptr<Agit::List::Node>& custom_object) override
		{
			MY_TRACE_FUNC("");

			if (custom_object->file_name.empty()) return FALSE;

			return shell_execute(_T("open"),
				get_script_folder_path(custom_object).c_str(),
				nullptr);
		}

		//
		// 指定されたカスタムオブジェクトのガイドを開きます。
		//
		virtual BOOL open_guide(const std::shared_ptr<Agit::List::Node>& custom_object) override
		{
			MY_TRACE_FUNC("");

			auto url = my::format(L"https://www.google.com/search?q={/} {/} {/}",
				custom_object->script_name + L" " + custom_object->author,
				L"aviutl カスタムオブジェクト", // 検索用追加ワードです。
				L"-VideoProc -VideoPad -CyberLink -Wondershare"); // 検索用除外ワードです。

			return shell_execute(_T("open"), url.c_str(), nullptr);
		}

		//
		// 指定されたカスタムオブジェクト用のオプションダイアログを表示します。
		//
		virtual BOOL show_option_dialog(const std::shared_ptr<Agit::List::Node>& custom_object) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ドラッグを開始します。
		//
		virtual BOOL begin_drag(const std::shared_ptr<Agit::List::Node>& custom_object) override
		{
			MY_TRACE_FUNC("");

			// テンポラリファイル名を取得します。
			auto file_name = get_config_file_name(L"temp_custom_object.exo");

			// カスタムオブジェクトをテンポラリファイルに書き込みます。
			write_custom_object(custom_object, file_name);

			// シェルアイテムを作成します。
			wil::com_ptr_nothrow<IShellItem> shell_item;
			auto hr = ::SHCreateItemFromParsingName(
				file_name.c_str(), nullptr, IID_PPV_ARGS(&shell_item));
			MY_TRACE_STR(my::get_error_message(hr));
			if (!shell_item) return FALSE;

			// データオブジェクトを作成します。
			wil::com_ptr_nothrow<IDataObject> data_object;
			hr = shell_item->BindToHandler(
				nullptr, BHID_DataObject, IID_PPV_ARGS(&data_object));
			MY_TRACE_STR(my::get_error_message(hr));
			if (!data_object) return FALSE;

			// ドラッグ時に表示するテキストを作成します。
			auto text = (custom_object->script_name != custom_object->file_name) ?
				(custom_object->script_name + custom_object->file_name) : custom_object->script_name;

			// ドロップソースを作成します。
			wil::com_ptr_nothrow<IDropSource> drop_source;
			drop_source.attach(DropSource::Allocator::create(text));

			// ドラッグアンドドロップを開始します。
			auto effect = DWORD {};
			hr = ::SHDoDragDrop(
				hive.main_window,
				data_object.get(),
				drop_source.get(),
				DROPEFFECT_MOVE | DROPEFFECT_COPY,
				&effect);
			MY_TRACE_STR(my::get_error_message(hr));

			return TRUE;
		}

		//
		// カスタムオブジェクトのリストを絞り込みます。
		//
		virtual BOOL filter_list() override
		{
			MY_TRACE_FUNC("");

			// カスタムオブジェクトのリストを絞り込みます。
			agit.filter_list();

			// ダイアログのリストビューを更新します。
			list_dialog.update_list_view();

			return TRUE;
		}
	} action_impl;
}
