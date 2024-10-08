﻿#pragma once

namespace my
{
	struct FileUpdateChecker
	{
		std::wstring file_name;
		FILETIME file_time = {};

		FileUpdateChecker()
		{
		}

		FileUpdateChecker(LPCWSTR file_name)
		{
			init(file_name);
		}

		void init(LPCWSTR file_name)
		{
			this->file_name = file_name;

			get_file_time(file_name, &file_time);
		}

		BOOL check_update()
		{
			FILETIME file_time;

			if (!get_file_time(file_name.c_str(), &file_time))
				return FALSE;

			if (!::CompareFileTime(&this->file_time, &file_time))
				return FALSE;

			this->file_time = file_time;

			return TRUE;
		}

		static BOOL get_file_time(LPCWSTR file_name, FILETIME* file_time)
		{
			std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(&::CloseHandle)> file(
				::CreateFileW(file_name, GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0), &::CloseHandle);

			if (file.get() == INVALID_HANDLE_VALUE)
				return FALSE;

			return ::GetFileTime(file.get(), 0, 0, file_time);
		}
	};

	struct FileUpdateCheckers
	{
		std::list<std::shared_ptr<FileUpdateChecker>> collection;

		void clear()
		{
			collection.clear();
		}

		void add(LPCWSTR file_name)
		{
			collection.emplace_back(std::make_shared<FileUpdateChecker>(file_name));
		}

		BOOL check_update()
		{
			for (const auto& item : collection)
			{
				if (item->check_update())
					return TRUE;
			}

			return FALSE;
		}
	};

	struct FileUpdateChecker2 : FileUpdateChecker
	{
		struct Handler {
			virtual void on_file_update(FileUpdateChecker2* checker) = 0;
		};

		HWND hwnd = nullptr;
		Handler* handler = nullptr;

		FileUpdateChecker2()
		{
		}

		FileUpdateChecker2(LPCWSTR file_name, HWND hwnd, Handler* handler)
		{
			init(file_name, hwnd, handler);
		}

		void init(LPCWSTR file_name, HWND hwnd, Handler* handler)
		{
			__super::init(file_name);

			this->hwnd = hwnd;
			this->handler = handler;

			::SetTimer(hwnd, (UINT_PTR)this, 1000,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
				{
					auto p = (FileUpdateChecker2*)timer_id;

					if (p->check_update())
						p->handler->on_file_update(p);
				});
		}

		void exit()
		{
			::KillTimer(hwnd, (UINT_PTR)this);
		}
	};
}
