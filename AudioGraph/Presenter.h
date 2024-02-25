#pragma once
#include "Present.h"

namespace fgo::audio_graph
{
	inline struct Presenter
	{
		inline static const UINT WM_SEND = WM_APP + 1;

		DWORD tid = 0;
		HANDLE thread = 0;

		BOOL m_isPlaying = FALSE;
		DWORD m_startTime = 0;
		int m_startFrame = 0;

		// 以下はメインスレッド側の処理です。

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			if (!thread) return FALSE;

//			::PostThreadMessage(tid, WM_QUIT, 0, 0);
			::TerminateThread(thread, 0);
			::CloseHandle(thread), thread = 0;

			return TRUE;
		}

		//
		// サブスレッドを開始します。
		//
		BOOL start()
		{
			MY_TRACE_FUNC("");

			if (thread) return FALSE;

			thread = ::CreateThread(0, 0, threadProc, this, 0, &tid);
			return !!thread;
		}

		//
		// サブスレッドにプレゼントを送ります。
		//
		BOOL send(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Servant::ProcState& proc_state)
		{
			MY_TRACE_FUNC("");

			if (fpip->audio_ch != 1 && fpip->audio_ch != 2)
				return FALSE;

			if (fpip->audio_n == 0)
				return FALSE;

			DWORD time = 0;

			if (isSameFrame(fp, fpip))
			{
				MY_TRACE(_T("再生中ではありません\n"));

				m_isPlaying = FALSE;
			}
			else
			{
				if (!proc_state.is_playing)
				{
					// 再生中なのにフレームが噛み合わないときは
					// 他のプラグインから呼ばれている可能性があるので何もしない。
					return FALSE;
				}

				if (!m_isPlaying || m_startFrame >= fpip->frame)
				{
					MY_TRACE(_T("再生が開始されました\n"));

					m_isPlaying = TRUE;
					m_startTime = ::timeGetTime();
					m_startFrame = fpip->frame;

					time = m_startTime;
				}
				else
				{
					MY_TRACE(_T("再生中です\n"));

					MY_TRACE_INT(fpip->frame);
					MY_TRACE_INT(m_startFrame);

					int offset = frame2ms(fp, fpip, fpip->frame - m_startFrame);
					MY_TRACE_INT(offset);

					time = m_startTime + offset;
					MY_TRACE_INT(time);
				}
			}

			// ここで確保したメモリはサブスレッドで開放されます。
			return ::PostThreadMessage(tid, WM_SEND, (WPARAM)new Present(fp, fpip, time), 0);
		}

		//
		// fpip->frameが変更されていない場合はTRUEを返します。
		//
		static BOOL isSameFrame(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			return fp->exfunc->get_frame(fpip->editp) == fpip->frame;
		}

		//
		// フレーム番号をミリ秒に変換して返します。
		//
		static int frame2ms(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, int frame)
		{
			AviUtl::FileInfo fi = {};
			fp->exfunc->get_file_info(fpip->editp, &fi);

			if (fi.video_rate != 0 && fi.video_scale != 0)
				return (int)(frame * 1000.0f * fi.video_scale / fi.video_rate);
			else
				return 0;
		}

		// 以下はサブスレッド側の処理です。

		//
		// スレッド関数から呼ばれます。
		//
		DWORD onThreadProc()
		{
			MY_TRACE_FUNC("");

			SubThread sub;

			return sub.messageLoop();
		}

		//
		// スレッド関数です。
		//
		static DWORD CALLBACK threadProc(LPVOID param)
		{
			MY_TRACE_FUNC("");

			Presenter* presenter = (Presenter*)param;

			return presenter->onThreadProc();
		}

		//
		// このクラスはサブスレッドです。
		// まず、メインスレッドからプレゼントを受け取ります。
		// 次にそのプレゼントの中身(音量)を共有メモリに書き込みます。
		// 最後にクライアントプロセスにメッセージを送信し、再描画を促します。
		//
		struct SubThread
		{
			Mutex mutex;
			SimpleFileMapping fileMapping;

			//
			// コンストラクタです。
			//
			SubThread()
				: mutex(0, FALSE, FormatText(Share::AudioGraph::MutexFormat, hive.hostWindow))
				, fileMapping(sizeof(Share::AudioGraph::Volume), FormatText(Share::AudioGraph::FileMappingFormat, hive.hostWindow))
			{
				MY_TRACE_FUNC("");
			}

			//
			// デストラクタです。
			//
			~SubThread()
			{
				MY_TRACE_FUNC("");
			}

			//
			// メッセージループです。
			//
			DWORD messageLoop()
			{
				MSG msg = {};
				while (::GetMessage(&msg, 0, 0, 0))
				{
					if (!msg.hwnd)
					{
						switch (msg.message)
						{
						case WM_SEND:
							{
								// クライアントプロセスに音量を送信します。
								onSend((Present*)msg.wParam);

								break;
							}
						case WM_QUIT:
							{
								// スレッドを終了します。
								return 0;
							}
						}
					}

					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}

				return 0;
			}

			//
			// クライアントプロセスに音量を送信します。
			//
			void onSend(Present* present)
			{
//				MY_TRACE_FUNC("");

				// 音量を算出します。
				present->calc();

				{
					// 算出された音量を共有メモリに書き込みます。

//					Synchronizer sync(mutex);
					auto shared = (Share::AudioGraph::Volume*)fileMapping.getBuffer();

					*shared = present->volume;
				}

				// 使用済みのメモリを開放します。
				delete present;

				// クライアントプロセスに再描画を促します。
				::SendMessage(hive.clientWindow, Share::AudioGraph::Message::Redraw, 0, 0);
			}
		};
	} presenter;
}
