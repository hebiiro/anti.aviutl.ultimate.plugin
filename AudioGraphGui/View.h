#pragma once
#include "Hive.h"
#include "GraphWindow.h"
#include "ConfigDialog.h"

namespace fgo::audio_graph_gui
{
	//
	// このクラスはグラフウィンドウのコレクションを管理します。
	//
	inline struct View : GraphWindow::Listener
	{
		std::vector<std::shared_ptr<GraphWindow>> collection;

		//
		// コンストラクタです。
		//
		View()
		{
			MY_TRACE_FUNC("");

			GraphWindow::listener = this;
		}

		//
		// デストラクタです。
		//
		~View()
		{
			MY_TRACE_FUNC("");
		}

		//
		// グラフウィンドウを作成し、コレクションに追加します。
		//
		std::shared_ptr<GraphWindow> createGraphWindow(HWND graphContainer, int graphWindowType)
		{
			MY_TRACE_FUNC("0x%08X, %d", graphContainer, graphWindowType);

			auto window = std::make_shared<GraphWindow>(graphWindowType);
			collection.emplace_back(window);
			BOOL result = window->create(graphContainer);
			MY_TRACE_INT(result);
			MY_TRACE_HWND(*window);
			return window;
		}

		//
		// 音量データを更新します。
		//
		BOOL updateVolume()
		{
			MY_TRACE_FUNC("");

			Share::AudioGraph::Volume volume = {};

			{
//				Synchronizer sync(hive.mutex);
				auto shared = (Share::AudioGraph::Volume*)hive.fileMapping.getBuffer();

				volume = *shared;
			}

			for (size_t i = 0; i < std::size(hive.level); i++)
			{
				hive.level[i] = volume.level[i];
				hive.peak[i] = volume.peak[i];
			}

			if (volume.time != 0)
			{
				DWORD time = ::timeGetTime();

				if (volume.time > time)
				{
					DWORD sleepTime = volume.time - time;
					MY_TRACE(_T("%d, %.3f, %.3f, %.3f秒間スリープします\n"), volume.frame, time / 1000.0, volume.time / 1000.0, sleepTime / 1000.0);
					::Sleep(sleepTime / 2);
				}
				else if (time - volume.time > 5) // 許容範囲以上に遅れている場合は描画を行わない。
				{
					MY_TRACE(_T("%d, %.3f, %.3f, 遅れているので無視します\n"), volume.frame, time / 1000.0, volume.time / 1000.0);
					return FALSE;
				}
			}
			else
			{
				MY_TRACE_INT(volume.frame);
			}

			return TRUE;
		}

		//
		// コンフィグダイアログを表示します。
		//
		BOOL config(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			ConfigDialog dialog(hwnd);

			int retValue = dialog.doModal2(hwnd);
			if (IDOK != retValue) return FALSE;

			return TRUE;
		}

		//
		// すべてのグラフウィンドウを再描画します。
		//
		void invalidate()
		{
			MY_TRACE_FUNC("");

			for (auto& window : collection)
				::InvalidateRect(*window, 0, FALSE);
		}

		//
		// すべてのグラフウィンドウを描画します。
		//
		void doPaint()
		{
			MY_TRACE_FUNC("");

			for (auto& window : collection)
				window->doPaint();
		}

		//
		// すべてのグラフウィンドウのデザインを更新します。
		//
		void updateDesign()
		{
			MY_TRACE_FUNC("");

			for (auto& window : collection)
				window->updateDesign();
		}

		//
		// 音量データを更新します。
		//
		BOOL onRedraw(GraphWindow* window) override
		{
			MY_TRACE_FUNC("");

			if (updateVolume())
				doPaint();

			return TRUE;
		}

		//
		// コンフィグダイアログを表示します。
		//
		BOOL onConfig(GraphWindow* window) override
		{
			MY_TRACE_FUNC("");

			if (config(*window))
				updateDesign();

			return TRUE;
		}

		//
		// すべてのグラフウィンドウを再描画します。
		//
		BOOL onInvalidate(GraphWindow* window) override
		{
			MY_TRACE_FUNC("");

			invalidate();

			return TRUE;
		}
	} view;
}
