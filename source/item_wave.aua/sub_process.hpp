#pragma once

namespace apn::item_wave
{
	//
	// このクラスはサブプロセスです。
	//
	struct sub_process_t
	{
		HANDLE job_object = {};
		PROCESS_INFORMATION pi = {};

		//
		// コンストラクタです。
		//
		sub_process_t(const std::wstring& dir, const std::wstring& arg_command_line, BOOL show = TRUE)
		{
			// ジョブオブジェクトを作成します。
			job_object = ::CreateJobObject({}, {});

			// メインプロセスが終了した時にサブプロセスも終了させたいので
			// ジョブオブジェクトにJOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSEを設定します。
			auto job_object_info = JOBOBJECT_EXTENDED_LIMIT_INFORMATION {};
			job_object_info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
			auto r2 = ::SetInformationJobObject(job_object,
				JobObjectExtendedLimitInformation, &job_object_info, sizeof(job_object_info));

			// 一旦コマンドラインをコピーします。
			WCHAR command_line[4096] = {};
			wcsncpy_s(command_line, arg_command_line.c_str(), _TRUNCATE);

			// サブプロセスを作成します。
			STARTUPINFOW si = { sizeof(si) };
			if (!show)
			{
				// 非表示で起動するようにします。
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_HIDE;
			}
			auto r1 = ::CreateProcessW(
				nullptr,			// application_name
				command_line,		// command_line
				nullptr,			// process_attributes
				nullptr,			// thread_attributes
				TRUE,				// inherit_handles
				CREATE_SUSPENDED,	// create_flags
				nullptr,			// enviroment
				dir.c_str(),		// current_directory
				&si,				// startup_info
				&pi);				// process_information

			// サブプロセスにジョブオブジェクトを関連付けます。
			auto r3 = ::AssignProcessToJobObject(job_object, pi.hProcess);
		}

		//
		// デストラクタです。
		//
		~sub_process_t()
		{
			// サブプロセスのハンドルを閉じます。
			::CloseHandle(job_object);
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}

		//
		// サブプロセスの実行を開始します。
		//
		BOOL start()
		{
			// サブプロセスを実行します。
			return ::ResumeThread(pi.hThread) != -1;
		}

		//
		// サブプロセスが終了するまで待機します。
		//
		DWORD wait(DWORD time = INFINITE)
		{
			return ::WaitForSingleObject(pi.hProcess, time);
		}
	};
}
