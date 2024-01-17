#pragma once

namespace Share::AudioGraph {
	namespace Message {
		// このメッセージはホストプロセスウィンドウの初期化が完了したとき、クライアントプロセスに送られます。
		// その後、クライアントプロセスウィンドウの初期化が完了したとき、ホストプロセスウィンドウに送り返されます。
		// WPARAM == HWND, LPARAM == GraphWindowTypeです。
		//
		inline const UINT Init = ::RegisterWindowMessage(_T("Share::AudioGraph::Init"));

		//
		// このメッセージはクライアントプロセスウィンドウの再描画が必要なとき、
		// ホストプロセスからクライアントプロセスウィンドウに送られます。
		//
		inline const UINT Redraw = ::RegisterWindowMessage(_T("Share::AudioGraph::Redraw"));

		//
		// このメッセージはクライアントプロセスのコンフィグダイアログを表示する必要があるとき、
		// ホストプロセスからクライアントプロセスウィンドウに送られます。
		//
		inline const UINT Config = ::RegisterWindowMessage(_T("Share::AudioGraph::Config"));
	}

	//
	// この構造体は音量データです。
	// クライアントプロセスがMessage::Redrawをハンドルしたとき、共有メモリから読み込まれます。
	//
	struct Volume
	{
		DWORD time;
		int frame;
		double level[2];
		double peak[2];
	};

	//
	// グラフウィンドウのタイプです。
	// Message::Initの引数として使用されます。
	//
	struct GraphWindowType
	{
		static const int Both = 0;
		static const int Left = 1;
		static const int Right = 2;
	};

	//
	// ミューテックスと共有メモリの名前です。
	// %08Xはホストプロセスのウィンドウハンドルです。
	//
	inline constexpr LPCTSTR MutexFormat = _T("AudioGraph.Mutex.%08X");
	inline constexpr LPCTSTR FileMappingFormat = _T("AudioGraph.FileMapping.%08X");
}
