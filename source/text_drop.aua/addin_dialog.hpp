#pragma once

namespace apn::text_drop
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : AddinDialogInterface
	{
		//
		// このクラスはドロップソースです。
		//
		struct DropSource : IDropSource, IDropSourceNotify
		{
			//
			// このクラスはドロップソースの生成を担当します。
			//
			struct Allocator
			{
				inline static auto create(int32_t mode, const std::wstring& exo_path)
				{
					return new DropSource(mode, exo_path);
				}

				inline static auto release(DropSource* p)
				{
					return delete p;
				}
			};

			//
			// このCOMオブジェクトの参照カウントです。
			//
			ULONG ref_count = 1;

			//
			// ドロップモードです。
			//
			int32_t mode;

			//
			// exoファイルのパスです。
			//
			std::wstring exo_path;

			//
			// コンストラクタです。
			//
			DropSource(int32_t mode, const std::wstring& exo_path)
				: mode(mode)
				, exo_path(exo_path)
			{
			}

			//
			// IUnknownの実装です。
			//
			virtual ULONG STDMETHODCALLTYPE AddRef() override
			{
				return ++ref_count;
			}

			//
			// IUnknownの実装です。
			//
			virtual ULONG STDMETHODCALLTYPE Release() override
			{
				if (--ref_count == 0)
				{
					Allocator::release(this);

					return 0;
				}

				return ref_count;
			}

			//
			// IUnknownの実装です。
			//
			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv) override
			{
				if (iid == IID_IUnknown || iid == IID_IDropSource)
				{
					AddRef();
					*ppv = static_cast<IDropSource*>(this);
					return S_OK;
				}
				else if (iid == IID_IDropSourceNotify)
				{
					AddRef();
					*ppv = static_cast<IDropSourceNotify*>(this);
					return S_OK;
				}

				*ppv = nullptr;
				return E_NOINTERFACE;
			}

			//
			// IDropSourceの実装です。
			//
			virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape_pressed, DWORD key_state) override
			{
				MY_TRACE_FUNC("{}, {:#010x}", escape_pressed, key_state);

				// エスケープキーが押されている場合はドラッグアンドドロップを中止します。
				if (escape_pressed) return DRAGDROP_S_CANCEL;

				// 左クリックが解除された場合は
				if (!(key_state & MK_LBUTTON))
				{
					// exoファイルに書き込みます。
					app->write_exo(mode, exo_path);

					// ドラッグアンドドロップを完了します。
					return DRAGDROP_S_DROP;
				}

				// それ以外の場合はドラッグアンドドロップを継続します。
				return S_OK;
			}

			//
			// IDropSourceの実装です。
			//
			virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect) override
			{
				MY_TRACE_FUNC("{:#010x}", effect);

				return DRAGDROP_S_USEDEFAULTCURSORS;
			}

			//
			// IDropSourceNotifyの実装です。
			//
			virtual HRESULT STDMETHODCALLTYPE DragEnterTarget(HWND target) override
			{
				MY_TRACE_FUNC("{:#010x}", target);
				MY_TRACE_HWND(target);

				return S_OK;
			}
        
			//
			// IDropSourceNotifyの実装です。
			//
			virtual HRESULT STDMETHODCALLTYPE DragLeaveTarget() override
			{
				MY_TRACE_FUNC("");

				return S_OK;
			}
		};

		//
		// 指定された座標からモードを取得して返します。
		//
		int32_t get_mode_from_point(const POINT& point)
		{
			auto control = ::ChildWindowFromPointEx(*this, point, CWP_ALL);
			auto control_id = ::GetDlgCtrlID(control);

			switch (control_id)
			{
			case IDC_DO_DRAG_WHOLE: return hive.c_mode.c_whole;
			case IDC_DO_DRAG_LINES: return hive.c_mode.c_lines;
			case IDC_DO_DRAG_CHARS: return hive.c_mode.c_chars;
			case IDC_DO_DRAG_BLOCKS: return hive.c_mode.c_blocks;
			}

			return hive.c_mode.c_none;
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_int(IDC_ITEM_RANGE, hive.item_range);
			get_int(IDC_SPACE_RANGE, hive.space_range);
			get_combobox_index(IDC_ITEM_DIRECTION, hive.item_direction);
			get_text(IDC_EXO_FILE_NAME, hive.exo_file_name);
			get_text(IDC_DROP_TEXT, hive.drop_text);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_int(IDC_ITEM_RANGE, hive.item_range);
			set_int(IDC_SPACE_RANGE, hive.space_range);
			set_combobox_index(IDC_ITEM_DIRECTION, hive.item_direction);
			set_text(IDC_EXO_FILE_NAME, hive.exo_file_name);
			set_text(IDC_DROP_TEXT, hive.drop_text);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_ITEM_DIRECTION, _T("横方向"), _T("縦方向"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto button = std::make_shared<RelativePos>(base_size * 4);
			auto stat = std::make_shared<RelativePos>(base_size * 4);
			auto editbox = std::make_shared<RelativePos>(base_size * 5);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_WHOLE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_LINES));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_CHARS));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_BLOCKS));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_ITEM_RANGE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_ITEM_RANGE));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SPACE_RANGE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_SPACE_RANGE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_ITEM_DIRECTION_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_ITEM_DIRECTION));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_EXO_FILE_NAME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_EXO_FILE_NAME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, full);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_DROP_TEXT));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, id, control);

			switch (id)
			{
			// ボタン
			// エディットボックス
			case IDC_ITEM_RANGE:
			case IDC_SPACE_RANGE:
			case IDC_EXO_FILE_NAME:
			case IDC_DROP_TEXT:
				if (code == EN_UPDATE) update_config(); break;
			// コンボボックス
			case IDC_ITEM_DIRECTION:
				if (code == CBN_SELCHANGE) update_config(); break;
			}
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SETCURSOR:
				{
//					MY_TRACE_FUNC("WM_SETCURSOR, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標からモードを取得します。
					auto point = my::get_cursor_pos();
					my::map_window_points(nullptr, hwnd, &point);
					auto mode = get_mode_from_point(point);
					if (mode == hive.c_mode.c_none) break;

					return ::SetCursor(::LoadCursor(nullptr, IDC_HAND)), TRUE;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標からモードを取得します。
					auto point = my::lp_to_pt(lParam);
					auto mode = get_mode_from_point(point);
					if (mode == hive.c_mode.c_none) break;

					// exoファイルのパスを取得します。
					auto exo_path = hive.exo_file_name.empty() ?
						magi.get_assets_file_name(hive.c_display_name) + L".exo" :
						magi.get_assets_file_name(hive.exo_file_name);

					{
#if 1
						// 空のファイルを作成します。
						std::ofstream ofs(exo_path, std::ios::binary);
#else
						// ドロップソースを使用せずに
						// ここでexoファイルを作成することも可能です。
						app->write_exo(mode, exo_path);
#endif
					}

					// シェルアイテムを作成します。
					ComPtr<IShellItem> shell_item;
					auto hr = ::SHCreateItemFromParsingName(exo_path.c_str(), nullptr, IID_PPV_ARGS(&shell_item));
					MY_TRACE_STR(my::get_error_message(hr));
					if (!shell_item) break;

					// データオブジェクトを作成します。
					ComPtr<IDataObject> data_object;
					hr = shell_item->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&data_object));
					MY_TRACE_STR(my::get_error_message(hr));
					if (!data_object) break;

					// ドラッグアンドドロップを開始します。
					// ::SHDoDragDrop()の場合はドロップソースをnullptrにすることもできますが、
					// 今回はドロップした瞬間に処理したかったので自前のドロップソースを渡しています。
					auto effect = DWORD {};
					hr = ::SHDoDragDrop(
						hwnd,
						data_object.Get(),
						DropSource::Allocator::create(mode, exo_path),
						DROPEFFECT_MOVE | DROPEFFECT_COPY,
						&effect);
					MY_TRACE_STR(my::get_error_message(hr));

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
