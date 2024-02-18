#pragma once
#include "Resource.h"
#include "Hive.h"
#include "Action.h"
#include "Common/AviUtl/SliderPanel.h"

namespace fgo::single_out
{
	//
	// このクラスはメインダイアログです。
	//
	struct MainDialog : Tools::Dialog
	{
		BOOL lock = FALSE;
		Tools::AviUtl::SliderPanel<int, Tools::AviUtl::IntExchanger<int>> quality;

		//
		// コンストラクタです。
		//
		MainDialog()
			: quality(0, 100)
		{
		}

		//
		// text内のsrcをdstに置換します。
		// https://learningprog.com/cpp-replace/
		//
		static void replace(std::wstring& text, const std::wstring& src, const std::wstring& dst)
		{
			size_t pos = text.find(src);  // 置換したい文字列が登場する位置を検索する
			size_t len = src.length();    // 置換したい文字列の長さ
			if (pos != text.npos)
				text.replace(pos, len, dst);  // 置換
		}

		//
		// 自動モード用のファイル名を返します。
		//
		_bstr_t getFileNameAuto()
		{
			MY_TRACE_FUNC("");

			// 基本となるファイル名を取得します。
			std::wstring fileName = (BSTR)hive.fileName;
			MY_TRACE_WSTR(fileName.c_str());

			// AviUtlの現在のプロジェクト名を取得します。
			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(magi.auin.GetEditp(), &si);
			WCHAR project[MAX_PATH] = {};
			::StringCchCopyW(project, std::size(project), (_bstr_t)si.project_name);
			::PathStripPathW(project);
			::PathRemoveExtensionW(project);
			MY_TRACE_WSTR(project);

			// AviUtlの現在のフレーム番号を文字列に変換します。
			int frame = magi.fp->exfunc->get_frame(magi.auin.GetEditp());
			std::wstringstream frame_ss;
			frame_ss << std::setw(hive.zero) << std::setfill(L'0') << frame;
			MY_TRACE_WSTR(frame_ss.str());

			// インデックスを文字列に変換します。
			int index = hive.index;
			std::wstringstream index_ss;
			index_ss << std::setw(hive.zero) << std::setfill(L'0') << index;
			MY_TRACE_WSTR(index_ss.str());

			// ファイル名を置換します。
			replace(fileName, L"<project>", (BSTR)project);
			replace(fileName, L"<frame>", frame_ss.str());
			replace(fileName, L"<index>", index_ss.str());

			// インデックスをインクリメントします。
			hive.index = index + 1;

			// インデックスを変更したのでコントロールに反映させます。
			updateControls();

			// 置換後のファイル名を返します。
			return fileName.c_str();
		}

		//
		// ファイル名を返します。
		//
		_bstr_t getFileName()
		{
			MY_TRACE_FUNC("");

			switch (hive.mode)
			{
			case Hive::Mode::Manual:
				{
					// ユーザーが選択したファイル名を返します。
					char fileName[MAX_PATH] = {};
					if (hive.browse(fileName)) return fileName;

					break;
				}
			case Hive::Mode::Auto:
			case Hive::Mode::Overwrite:
				{
					// 自動保存用のファイル名を取得します。
					_bstr_t fileName = getFileNameAuto();
					if ((BSTR)fileName)
					{
						// 上書きモードか確認します。
						if (hive.mode == Hive::Mode::Overwrite)
							return fileName;

						// ファイルが存在するか確認します。
						if (::GetFileAttributesW(fileName) == -1)
							return fileName;

						// ファイルを上書きするかどうかをユーザーに確認します。
						WCHAR text[MAX_PATH] = {};
						::StringCchPrintfW(text, std::size(text), L"%ws\nはすでに存在します。上書きしますか?", (BSTR)fileName);
						if (IDYES == ::MessageBoxW(*this, text, hive.DisplayName, MB_YESNO))
							return fileName;
					}

					break;
				}
			}

			return (BSTR)0;
		}

		//
		// アクションに画像をエクスポートさせます。
		//
		BOOL exportImage(BOOL hasAlpha, BOOL itemOnly)
		{
			_bstr_t fileName = getFileName();
			if (!fileName) return FALSE;
			::MakeSureDirectoryPathExists(fileName);
			return action.onExportImage(fileName, hasAlpha, itemOnly);
		}

		//
		// ウィンドウを作成します。
		//
		BOOL create(HWND parent)
		{
			return __super::create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent);
		}

		//
		// コントロールの値を更新します。
		//
		BOOL updateControls()
		{
			if (lock) return FALSE;

			lock = TRUE;

			quality.setValue(hive.quality);
			setComboBox(IDC_MODE, hive.mode);
			setText(IDC_FILE_NAME, hive.fileName);
			setInt(IDC_INDEX, hive.index);
			setInt(IDC_ZERO, hive.zero);

			lock = FALSE;

			return TRUE;
		}

		//
		// コンフィグの値を更新します。
		//
		BOOL updateConfig()
		{
			if (lock) return FALSE;

			hive.quality = quality.getValue();
			hive.mode = getComboBox(IDC_MODE);
			hive.fileName = getText(IDC_FILE_NAME);
			hive.index = getInt(IDC_INDEX);
			hive.zero = getInt(IDC_ZERO);

			return TRUE;
		}

		//
		// ファイル名を取得します。
		//
		BOOL onFileNameDir()
		{
			MY_TRACE_FUNC("");

			char fileName[MAX_PATH] = {};
			if (!hive.browse(fileName))
				return FALSE;

			return setText(IDC_FILE_NAME, (_bstr_t)fileName);
		}

		//
		// コントロールの位置を更新します。
		//
		void onSize()
		{
			MY_TRACE_FUNC("");

			const int base = getBaseSize();
			MY_TRACE_INT(base);
			const int labelSize = base * 3;

			HWND button[] = {
				::GetDlgItem(*this, IDC_EXPORT_FRAME_RGB),
				::GetDlgItem(*this, IDC_EXPORT_FRAME_RGBA),
				::GetDlgItem(*this, IDC_EXPORT_ITEM_RGB),
				::GetDlgItem(*this, IDC_EXPORT_ITEM_RGBA),
			};

			RECT rc; ::GetClientRect(*this, &rc);
			int cx = rc.left;
			int cy = rc.top;
			int cw = getWidth(rc);
			int ch = getHeight(rc);

			HDWP dwp = ::BeginDeferWindowPos(11);

			int y = cy;

			{
				// ボタンの位置を変更します。

				for (size_t i = 0; i < std::size(button); i++)
					::DeferWindowPos(dwp, button[i], 0, cx, y + base * i, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);

				y += base * std::size(button);
			}

			{
				// 「JPEG品質」の位置を変更します。

				quality.move(dwp, cx, y, cw, base);

				y += base;
			}

			{
				// 「モード」の位置を変更します。

				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_MODE_LABEL), 0,
					cx, y, labelSize, base, SWP_NOZORDER | SWP_NOACTIVATE);
				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_MODE), 0,
					cx + labelSize, y, cw - labelSize, base, SWP_NOZORDER | SWP_NOACTIVATE);

				y += base;
			}

			{
				// 「ファイル名」の位置を変更します。

				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_FILE_NAME), 0,
					cx, y, cw - base, base, SWP_NOZORDER | SWP_NOACTIVATE);
				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_FILE_NAME_DIR), 0,
					cx + cw - base, y, base, base, SWP_NOZORDER | SWP_NOACTIVATE);

				y += base;
			}

			{
				// 「インデックス」の位置を変更します。

				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_INDEX_LABEL), 0,
					cx, y, labelSize, base, SWP_NOZORDER | SWP_NOACTIVATE);
				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_INDEX), 0,
					cx + labelSize, y, cw - labelSize, base, SWP_NOZORDER | SWP_NOACTIVATE);

				y += base;
			}

			{
				// 「0の個数」の位置を変更します。

				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_ZERO_LABEL), 0,
					cx, y, labelSize, base, SWP_NOZORDER | SWP_NOACTIVATE);
				::DeferWindowPos(dwp, ::GetDlgItem(*this, IDC_ZERO), 0,
					cx + labelSize, y, cw - labelSize, base, SWP_NOZORDER | SWP_NOACTIVATE);

				y += base;
			}

			::EndDeferWindowPos(dwp);
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// ダイアログプロシージャです。
		//
		INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, 0x%08X, 0x%08X", wParam, lParam);

					quality.init(hwnd, IDC_QUALITY);

					setComboBox(IDC_MODE, hive.mode,
						_T("保存の度に選択"),
						_T("予め指定されたファイルに保存"),
						_T("予め指定されたファイルに上書き保存"));

					break;
				}
			case WM_SIZE:
				{
					onSize();

					break;
				}
			case WM_HSCROLL:
				{
					MY_TRACE_FUNC("WM_HSCROLL, 0x%08X, 0x%08X", wParam, lParam);

					HWND control = (HWND)lParam;

					if (control == quality.slider)
						hive.quality = quality.onChangePos();

					break;
				}
			case WM_COMMAND:
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE_FUNC("WM_COMMAND, 0x%08X, 0x%08X", wParam, lParam);

					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					case IDC_EXPORT_FRAME_RGB: exportImage(FALSE, FALSE); break;
					case IDC_EXPORT_FRAME_RGBA: exportImage(TRUE, FALSE); break;
					case IDC_EXPORT_ITEM_RGB: exportImage(FALSE, TRUE); break;
					case IDC_EXPORT_ITEM_RGBA: exportImage(TRUE, TRUE); break;
					case IDC_QUALITY:
						{
							if (code == EN_CHANGE)
								hive.quality = quality.onChangeText();

							break;
						}
					case IDC_MODE:
						{
							if (code == CBN_SELCHANGE)
								updateConfig();

							break;
						}
					case IDC_FILE_NAME_DIR: onFileNameDir(); break;
					case IDC_FILE_NAME:
					case IDC_INDEX:
					case IDC_ZERO:
						{
							if (code == EN_CHANGE)
								updateConfig();

							break;
						}
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
