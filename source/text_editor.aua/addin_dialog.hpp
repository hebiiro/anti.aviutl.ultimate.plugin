#pragma once

namespace apn::text_editor
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : AddinDialogInterface
	{
		//
		// WM_TIMER用のタイマーIDです。
		//
		inline static constexpr struct TimerID {
			inline static constexpr uint32_t c_to_edits = 2025;
			inline static constexpr uint32_t c_from_edits = 2026;
		} c_timer_id;

		//
		// editsのロックカウントです。
		//
		int32_t edits_lock_count = 0;

		//
		// このクラスはeditsをロックします。
		//
		struct EditsLocker {
			AddinDialog* p;
			EditsLocker(AddinDialog* p) : p(p) { p->edits_lock_count++; }
			~EditsLocker() { p->edits_lock_count--; }
		};

		//
		// editsがロックされている場合はTRUEを返します。
		//
		BOOL is_edits_locked() const
		{
			return edits_lock_count != 0;
		}

		//
		// テキストアイテムのテキストをeditsにも反映します。
		//
		BOOL to_edits()
		{
			MY_TRACE_FUNC("");

			// editsがロックされている場合は何もしません。
			if (is_edits_locked()) return FALSE;

			// タイマーを開始します。
			::SetTimer(*this, c_timer_id.c_to_edits, hive.delay, 0);

			return TRUE;
		}

		//
		// editsのテキストをテキストアイテムにも反映します。
		//
		BOOL from_edits()
		{
			MY_TRACE_FUNC("");

			// editsがロックされている場合は何もしません。
			if (is_edits_locked()) return FALSE;

			// タイマーを開始します。
			::SetTimer(*this, c_timer_id.c_from_edits, hive.delay, 0);

			return TRUE;
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			if (hive.zoomable)
			{
				// editsのズームを有効にします。
				::SendMessage(ctrl(IDC_EDITS), EM_SETEXTENDEDSTYLE, ES_EX_ZOOMABLE, ES_EX_ZOOMABLE);
			}
			else
			{
				// editsのズームを無効にします。
				::SendMessage(ctrl(IDC_EDITS), EM_SETEXTENDEDSTYLE, ES_EX_ZOOMABLE, 0);
			}

			::SendMessage(ctrl(IDC_EDITS), WM_SETFONT, (WPARAM)hive.font.handle.get(), TRUE);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto size_s = std::make_shared<RelativePos>(base_size * 3);
			auto size_m = std::make_shared<RelativePos>(base_size * 4);
			auto size_l = std::make_shared<RelativePos>(base_size * 6);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			root->add_pane(c_axis.c_horz, c_align.c_left, full, {}, ctrl(IDC_EDITS));
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			// editsに変更があった場合は
			if (id == IDC_EDITS && code == EN_UPDATE)
			{
				MY_TRACE("editsのテキストが変更されました\n");

				// テキストアイテムにも反映します。
				from_edits();
			}
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_TIMER:
				{
					switch (wParam)
					{
					case c_timer_id.c_to_edits:
						{
							// テキストアイテムのテキストをeditsにセットします。

							// タイマーを停止します。
							::KillTimer(hwnd, wParam);

							// editsがロックされている場合は何もしません。
							if (is_edits_locked()) break;

							// editsをロックします。
							EditsLocker locker(this);

							// テキストアイテムのテキストを取得します。
							auto text_editbox = magi.exin.get_text_editbox();
							auto text = my::get_window_text(text_editbox);

							// editsにセットします。
							auto edits = ::GetDlgItem(hwnd, IDC_EDITS);
							::SetWindowText(edits, text.c_str());

							break;
						}
					case c_timer_id.c_from_edits:
						{
							// editsのテキストをテキストアイテムにセットします。

							// タイマーを停止します。
							::KillTimer(hwnd, wParam);

							// editsがロックされている場合は何もしません。
							if (is_edits_locked()) break;

							// editsをロックします。
							EditsLocker locker(this);

							// editsのテキストを取得します。
							auto edits = ::GetDlgItem(hwnd, IDC_EDITS);
							auto text = my::get_window_text(edits);

							// テキストアイテムにセットします。
							auto text_editbox = magi.exin.get_text_editbox();
							::SetWindowText(text_editbox, text.c_str());

							{
								// ::SetWindowText()しただけでは通知メッセージが発生しないので
								// 手動でEN_CHANGEを送信します。(EN_UPDATEは必要ないようです)

								auto control_id = ::GetDlgCtrlID(text_editbox); // 22101 == 0x5655
								auto setting_dialog = ::GetParent(text_editbox);
								::SendMessage(setting_dialog, WM_COMMAND, MAKEWPARAM(control_id, EN_CHANGE), (LPARAM)text_editbox);
//								::SendMessage(setting_dialog, WM_COMMAND, MAKEWPARAM(control_id, EN_UPDATE), (LPARAM)text_editbox);
							}

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
