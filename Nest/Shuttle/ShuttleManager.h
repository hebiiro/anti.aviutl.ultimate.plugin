#pragma once
#include "Resource.h"
#include "Hive.h"
#include "Shuttle.h"

namespace fgo::nest
{
	//
	// このクラスはシャトルを管理します。
	//
	inline static struct ShuttleManager : Shuttle::StaticListener
	{
		//
		// シャトルのコレクションです。
		//
		std::map<_bstr_t, std::shared_ptr<Shuttle>> shuttles;

		//
		// コンストラクタです。
		//
		ShuttleManager()
		{
			// シャトルマネージャーをシャトルのスタティックリスナーに設定します。
			Shuttle::staticListener = this;
		}

		//
		// 指定された名前のシャトルを返します。
		//
		std::shared_ptr<Shuttle> get(const _bstr_t& name)
		{
			auto it = shuttles.find(name);
			if (it == shuttles.end()) return 0;
			return it->second;
		}

		// シャトルの名前を変更します。
		BOOL rename(Shuttle* shuttle, const _bstr_t& newName)
		{
			if (shuttles.find(newName) != shuttles.end())
				return FALSE;

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

			return TRUE;
		}

		// シャトルをリネームするためのダイアログを表示します。
		int showRenameDialog(Shuttle* shuttle, HWND parent)
		{
			RenameDialog dialog(shuttle);

			return dialog.doModal(parent);
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
				::GetDlgItemText(*this, IDC_NEW_NAME, newName, std::size(newName));

				// 古い名前と新しい名前が違うなら
				if (_tcscmp(shuttle->name, newName) != 0)
				{
					// シャトルの名前を変更します。
					shuttleManager.rename(shuttle, newName);
				}

				return retValue;
			}

			void onOK() override
			{
				TCHAR newName[MAX_PATH] = {};
				::GetDlgItemText(*this, IDC_NEW_NAME, newName, std::size(newName));

				// 新しい名前が現在のシャトルの名前と違い、
				// なおかつ新しい名前と同じ名前のシャトルがすでに存在する場合は
				if (_tcscmp(shuttle->name, newName) != 0 && shuttleManager.get(newName))
				{
					// メッセージボックスを表示して拒否します。

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

	protected:

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
				::StringCchPrintf(text, std::size(text), _T("ウィンドウ名が重複しています\nウィンドウ名 : %ws"), (LPCWSTR)name);
				::MessageBox(hive.mainWindow, text, hive.AppName, MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);

				return FALSE;
			}

			// シャトルをコレクションに追加します。
			shuttles[name] = shuttle;

			return TRUE;
		}

		//
		// 指定されたシャトルをコレクションから取り除きます。
		//
		BOOL erase(const std::shared_ptr<Shuttle>& shuttle)
		{
			// リスナーが存在する場合は
			if (shuttle->listener)
			{
				// リスナーにシャトルの削除を通知します。
				shuttle->listener->releaseShuttle(shuttle.get());
			}

			// コレクションから削除します。
			return !!shuttles.erase(shuttle->name);
		}

		//
		// シャトルが作成されたときに呼ばれます。
		//
		void onInitShuttle(const std::shared_ptr<Shuttle>& shuttle) override
		{
			MY_TRACE_FUNC("%ws", (LPCWSTR)shuttle->name);

			add(shuttle, shuttle->name);
		}

		//
		// シャトルが破壊されたときに呼ばれます。
		//
		void onExitShuttle(const std::shared_ptr<Shuttle>& shuttle) override
		{
			MY_TRACE_FUNC("%ws", (LPCWSTR)shuttle->name);

			// このシャトルをコレクションから削除します。
			erase(shuttle);
		}
	} shuttleManager;
}
