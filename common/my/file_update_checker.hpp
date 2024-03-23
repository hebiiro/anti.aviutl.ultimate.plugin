#pragma once

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
}
