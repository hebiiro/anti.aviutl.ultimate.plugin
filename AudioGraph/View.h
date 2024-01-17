#pragma once
#include "Hive.h"
#include "Presenter.h"
#include "GraphContainer.h"

namespace fgo::audio_graph
{
	//
	// このクラスはグラフコンテナのコレクションを管理します。
	//
	inline struct View : GraphContainer::Listener
	{
		//
		// ウィンドウのコレクションです。
		//
		std::vector<std::shared_ptr<GraphContainer>> collection;

		//
		// コンストラクタです。
		//
		View()
		{
			MY_TRACE_FUNC("");

			GraphContainer::listener = this;
		}

		//
		// デストラクタです。
		//
		~View()
		{
			MY_TRACE_FUNC("");
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// グラフコンテナを作成します。
			auto mainWindow = collection.emplace_back(std::make_shared<GraphContainer>(hive.DisplayName, L"LR-Window", Share::AudioGraph::GraphWindowType::Both));
			auto leftWindow = collection.emplace_back(std::make_shared<GraphContainer>(hive.window.left.name, L"L-Window", Share::AudioGraph::GraphWindowType::Left));
			auto rightWindow = collection.emplace_back(std::make_shared<GraphContainer>(hive.window.right.name, L"R-Window", Share::AudioGraph::GraphWindowType::Right));

			// メインウィンドウをハイブに格納します。
			hive.mainWindow = *mainWindow;

			// メインウィンドウをホストウィンドウに設定します。
			hive.hostWindow = *mainWindow;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// グラフコンテナの作成が完了したことをクライアントプロセスに通知します。
		//
		BOOL notify(DWORD client_tid)
		{
			MY_TRACE_FUNC("%d", client_tid);

			for (const auto& window : collection)
				::PostThreadMessage(client_tid, Share::AudioGraph::Message::Init, (WPARAM)(HWND)*window, window->graph_type);

			return TRUE;
		}

		//
		// 表示されているグラフコンテナが存在する場合はTRUEを返します。
		//
		BOOL isVisible() const
		{
			for (const auto& window : collection)
				if (::IsWindowVisible(*window)) return TRUE;

			return FALSE;
		}

		//
		// 指定されたファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			MY_TRACE_FUNC("%ws", path);

			for (const auto& window : collection)
				getPrivateProfileWindow(path, window->getInternalName(), *window);

			return TRUE;
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			MY_TRACE_FUNC("%ws", path);

			for (const auto& window : collection)
				setPrivateProfileWindow(path, window->getInternalName(), *window);

			return TRUE;
		}

		//
		// コンテキストメニューを表示します。
		//
		void onContextMenu(GraphContainer* graph_container) override
		{
			HMENU menu = ::CreatePopupMenu();

			::AppendMenu(menu, MF_STRING, Hive::CommandID::State::On, _T("有効"));
			::AppendMenu(menu, MF_STRING, Hive::CommandID::State::OnWithoutPlaying, _T("有効(再生時以外)"));
			::AppendMenu(menu, MF_STRING, Hive::CommandID::State::Off, _T("無効"));
			::AppendMenu(menu, MF_SEPARATOR, 0, 0);
			::AppendMenu(menu, MF_STRING, Hive::CommandID::Config, _T("設定"));
			size_t c = collection.size();
			if (c != 0)
			{
				::AppendMenu(menu, MF_SEPARATOR, 0, 0);
				for (size_t i = 0; i < c; i++)
				{
					HWND hwnd = *collection[i];
					UINT id = Hive::CommandID::Window::Begin + i;

					TCHAR text[MAX_PATH] = {};
					::GetWindowText(hwnd, text, std::size(text));
					::AppendMenu(menu, MF_STRING, id, text);
					if (::IsWindowVisible(hwnd)) ::CheckMenuItem(menu, id, MF_CHECKED);
				}
			}

			switch (hive.state)
			{
			case Hive::State::Off: ::CheckMenuItem(menu, Hive::CommandID::State::Off, MF_CHECKED); break;
			case Hive::State::On: ::CheckMenuItem(menu, Hive::CommandID::State::On, MF_CHECKED); break;
			case Hive::State::OnWithoutPlaying: ::CheckMenuItem(menu, Hive::CommandID::State::OnWithoutPlaying, MF_CHECKED); break;
			}

			POINT cursorPos; ::GetCursorPos(&cursorPos);
			UINT id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, cursorPos.x, cursorPos.y, 0, hive.mainWindow, 0);

			switch (id)
			{
			case Hive::CommandID::State::Off:
				{
					hive.state = Hive::State::Off;

					break;
				}
			case Hive::CommandID::State::On:
				{
					hive.state = Hive::State::On;

					break;
				}
			case Hive::CommandID::State::OnWithoutPlaying:
				{
					hive.state = Hive::State::OnWithoutPlaying;

					break;
				}
			case Hive::CommandID::Config:
				{
					::PostMessage(graph_container->getTarget(), Share::AudioGraph::Message::Config, 0, 0);

					break;
				}
			}

			if (id >= Hive::CommandID::Window::Begin && id < Hive::CommandID::Window::End)
			{
				size_t index = id - Hive::CommandID::Window::Begin;
				if (index < collection.size())
					::SendMessage(*collection[index], WM_CLOSE, 0, 0);
			}

			::DestroyMenu(menu);
		}
	} view;
}
