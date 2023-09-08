#pragma once
#include "Resource.h"
#include "Shuttle.h"

namespace fgo::nest
{
	//
	// このクラスはシャトルを管理します。
	//
	inline static struct ShuttleManager
	{
		//
		// シャトルのコレクションです。
		//
		std::map<_bstr_t, std::shared_ptr<Shuttle>> shuttles;

		//
		// 指定された名前のシャトルを返します。
		//
		std::shared_ptr<Shuttle> get(const _bstr_t& name)
		{
			auto it = shuttles.find(name);
			if (it == shuttles.end()) return 0;
			return it->second;
		}

		//
		// シャトルをコレクションに追加します。
		//
		BOOL add(const std::shared_ptr<Shuttle>& shuttle, const _bstr_t& name)
		{
			// 指定された名前のシャトルがすでに存在している場合は
			if (get(name))
			{
				// メッセージボックスを出して処理を中止します。

				TCHAR text[MAX_PATH] = {};
				::StringCbPrintf(text, sizeof(text), _T("ウィンドウ名が重複しています\nウィンドウ名 : %ws"), (BSTR)name);
				::MessageBox(hive.mainWindow, text, hive.AppName, MB_OK | MB_ICONWARNING);

				return FALSE;
			}

			// シャトルをコレクションに追加します。
			shuttles[name] = shuttle;

			return TRUE;
		}

		//
		// 指定されたシャトルをコレクションから取り除きます。
		//
		void erase(Shuttle* shuttle)
		{
			// リスナーが存在する場合は
			if (shuttle->listener)
			{
				// リスナーにシャトルの削除を通知します。
				shuttle->listener->releaseShuttle(shuttle);
			}

			// コレクションから削除します。
			shuttles.erase(shuttle->name);

			// 基本的にこの時点で shuttle は無効なポインタになります。
		}

		// シャトルの名前を変更します。
		void rename(Shuttle* shuttle, const _bstr_t& newName)
		{
			assert(shuttles.find(newName) == shuttles.end());

			// シャトルの以前の名前を取得しておきます。
			_bstr_t oldName = shuttle->name;

			// シャトルの名前を変更します。
			shuttle->name = newName;

			// 新しい名前でコレクションに追加します。
			shuttles[newName] = shuttles[oldName];

			// 以前の名前をマップから削除します。
			shuttles.erase(oldName);

			// ウィンドウ名を新しい名前に更新します。
			::SetWindowText(*shuttle, newName);
		}

		// シャトルをリネームするためのダイアログを表示します。
		void showRenameDialog(HWND owner, Shuttle* shuttle)
		{
			RenameDialog dialog(shuttle);

			dialog.doModal(owner);
		}

		//
		// このクラスはシャトルをリネームするためのダイアログです。
		//
		struct RenameDialog : Tools::Dialog2
		{
			Shuttle* shuttle = 0;

			RenameDialog(Shuttle* shuttle)
				: shuttle(shuttle)
			{
			}

			int doModal(HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_RENAME_SHUTTLE), parent);

				::SetDlgItemText(*this, IDC_NEW_NAME, shuttle->name);

				int retValue = __super::doModal2(parent);

				if (IDOK != retValue)
					return retValue;

				TCHAR newName[MAX_PATH] = {};
				::GetDlgItemText(*this, IDC_NEW_NAME, newName, MAX_PATH);

				// 古い名前と新しい名前が違うなら
				if (::StrCmp(shuttle->name, newName) != 0)
				{
					// シャトルの名前を変更します。
					shuttleManager.rename(shuttle, newName);
				}

				return retValue;
			}

			void onOK() override
			{
				TCHAR newName[MAX_PATH] = {};
				::GetDlgItemText(*this, IDC_NEW_NAME, newName, MAX_PATH);

				// 新しい名前が現在のシャトルの名前と違い、
				// なおかつ新しい名前と同じ名前のシャトルがすでに存在する場合は
				if (::StrCmp(shuttle->name, newName) != 0 && shuttleManager.get(newName))
				{
					// メッセージボックスを表示します。
					::MessageBox(*this, _T("名前が重複しています"), hive.AppName, MB_OK | MB_ICONWARNING);

					return;
				}

				__super::onOK();
			}

			void onCancel() override
			{
				__super::onCancel();
			}
		};
	} shuttleManager;
}
