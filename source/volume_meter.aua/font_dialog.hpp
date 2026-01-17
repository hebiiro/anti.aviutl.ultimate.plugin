#pragma once

namespace apn::volume_meter
{
	//
	// このクラスはフォント選択ダイアログです。
	//
	struct font_dialog_t : my::Window
	{
		inline static constexpr struct control_id_t {
			// コンボボックスのコントロールIDです。
			inline static constexpr uint32_t c_font_name = 0x00000470;
			inline static constexpr uint32_t c_style = 0x00000471;
			inline static constexpr uint32_t c_size = 0x00000472;
			inline static constexpr uint32_t c_color = 0x00000473;
			inline static constexpr uint32_t c_charset = 0x00000474;

			// ボタンのコントロールIDです。
			inline static constexpr uint32_t c_apply = 0x00000402;
			inline static constexpr uint32_t c_show_font = 0x00000638;

			// チェックボックスのコントロールIDです。
			inline static constexpr uint32_t c_stroke = 0x00000410;
			inline static constexpr uint32_t c_underline = 0x00000411;
		} c_control_id;

		//
		// コンボボックスで選択されているフォント名です。
		//
		std::wstring font_name;

		//
		// 現在選択されているフォントの情報を取得します。
		//
		void from_ui()
		{
			font_name = my::get_dlg_item_text(hwnd, c_control_id.c_font_name);
		}

		//
		// フォント選択ダイアログを表示します。
		//
		virtual BOOL do_modal(HWND owner, const std::wstring& font_name, DWORD flags = CF_NOVERTFONTS | CF_APPLY)
		{
			// 論理フォントを作成します。
			auto lf = LOGFONTW {};
			wcscpy_s(lf.lfFaceName, font_name.c_str());

			// フォント選択構造体を作成します。
			CHOOSEFONTW cf = { sizeof(cf) };
			cf.Flags = CF_INITTOLOGFONTSTRUCT | flags;
			cf.hwndOwner = owner;
			cf.lpLogFont = &lf;

			// フォント選択ダイアログの
			// ウィンドウプロシージャをフックできるようにします。
			Hooker hooker(*this);

			// フォント選択ダイアログを表示します。
			return ::ChooseFont(&cf);
		}

		//
		// 適用ボタンが押されたときの処理です。
		//
		virtual void on_apply(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					auto code = HIWORD(w_param);
					auto control_id = LOWORD(w_param);
					auto control = (HWND)l_param;

					MY_TRACE_FUNC("WM_COMMAND, {/hex16}, {/hex16}, {/hex}", code, control_id, control);

					switch (LOWORD(w_param))
					{
					case c_control_id.c_font_name:
						{
							// フォントが選択された場合は
							if (code == CBN_SELCHANGE)
							{
								// 現在選択されているフォントの情報を取得します。
								from_ui();
							}

							break;
						}

					// 適用ボタンが押されたときの処理です。
					case c_control_id.c_apply:
						{
							// 現在選択されているフォントの情報を取得します。
							from_ui();

							// サブクラスに処理させます。
							on_apply(hwnd, message, w_param, l_param);

							break;
						}

					// OKボタンが押されたときの処理です。
					case IDOK:
						{
							// 現在選択されているフォントの情報を取得します。
							from_ui();

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	};
	//
	// このクラスはフォント選択ダイアログです。
	// ハンドラを渡すことができます。
	//
	template <typename apply_handler_t>
	struct font_dialog_with_handler_t : font_dialog_t
	{
		//
		// 適用ボタンが押されたときに呼ばれるハンドラです。
		//
		apply_handler_t apply_handler;

		//
		// コンストラクタです。
		//
		font_dialog_with_handler_t(apply_handler_t apply_handler)
			: apply_handler(apply_handler)
		{
		}

		//
		// 適用ボタンが押されたときの処理です。
		//
		virtual void on_apply(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			apply_handler(*this);
		}
	};
}
