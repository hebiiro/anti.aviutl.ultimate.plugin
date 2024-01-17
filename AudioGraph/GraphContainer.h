#pragma once
#include "Hive.h"
#include "Presenter.h"

namespace fgo::audio_graph
{
	//
	// このクラスはグラフウィンドウを格納するウィンドウです。
	// グラフウィンドウはクライアントプロセスで作成されます。
	//
	struct GraphContainer : Tools::AviUtl::PluginWindow
	{
		//
		// このクラスはリスナーです。
		//
		inline static struct Listener {
			virtual void onContextMenu(GraphContainer* graph_container) = 0;
		} *listener = 0;

		//
		// コンフィグファイルのセクション名に使用されます。
		//
		const _bstr_t internal_name;

		//
		// グラフタイプです。Share::AudioGraph::GraphWindowTypeの値を指定します。
		//
		int graph_type;

		//
		// コンストラクタです。
		//
		GraphContainer(LPCWSTR window_name, LPCWSTR internal_name, int graph_type)
			: internal_name(internal_name)
			, graph_type(graph_type)
		{
			// 親ウィンドウを取得します。
			HWND parent = ::GetWindow(magi.fp->hwnd, GW_OWNER);

			// ウィンドウを作成する度にX座標をずらします。
			static int w = 100;
			static int x = 0; x += w;

			// AviUtlプラグインウィンドウと同じように振る舞うウィンドウを作成します。
			createAsPlugin(
				hive.instance, parent, window_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				x, 100, w, 400);
		}

		//
		// 内部で使用される名前を返します。
		//
		LPCWSTR getInternalName() const
		{
			return internal_name;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_GETMINMAXINFO:
				{
//					MY_TRACE_FUNC("WM_GETMINMAXINFO, 0x%08X, 0x%08X", wParam, lParam);

					// ウィンドウの最小サイズを0に変更します。
					auto mmi = (MINMAXINFO*)lParam;
					mmi->ptMinTrackSize.x = 0;
					mmi->ptMinTrackSize.y = 0;

					break;
				}
			case WM_CONTEXTMENU:
				{
					MY_TRACE_FUNC("WM_CONTEXTMENU, 0x%08X, 0x%08X", wParam, lParam);

					// コンテキストメニューを表示します。
					listener->onContextMenu(this);

					break;
				}
			}

			// このメッセージはグラフウィンドウの初期化が完了したときに通知されます。
			if (message == Share::AudioGraph::Message::Init)
			{
				MY_TRACE_FUNC("Share::AudioGraph::Message::Init, 0x%08X, 0x%08X", wParam, lParam);

				auto clientWindow = (HWND)wParam;

				// clientWindowが最初のクライアントウィンドウの場合は
				if (!hive.clientWindow)
				{
					// クライアントウィンドウをハイブに格納します。
					hive.clientWindow = clientWindow;

					// プレゼンテーション(サブスレッド)を開始します。
					presenter.start();
				}

				// グラフウィンドウをターゲットに設定し、リサイズします。
				setTarget(clientWindow);
				resize();
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
