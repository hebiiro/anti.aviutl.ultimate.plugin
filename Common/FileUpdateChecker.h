#pragma once

class FileUpdateChecker
{
private:

	WCHAR m_filePath[MAX_PATH];
	FILETIME m_fileTime;

public:

	FileUpdateChecker()
		: m_filePath()
		, m_fileTime()
	{
//		MY_TRACE(_T("FileUpdateChecker::FileUpdateChecker()\n"));
	}

	FileUpdateChecker(LPCWSTR filePath)
	{
//		MY_TRACE(_T("FileUpdateChecker::FileUpdateChecker(%ws)\n"), filePath);

		init(filePath);
	}

	void init(LPCWSTR filePath)
	{
		::StringCbCopyW(m_filePath, sizeof(m_filePath), filePath);
		getFileTime(m_filePath, &m_fileTime);
	}

	LPCWSTR getFilePath() const
	{
		return m_filePath;
	}

	BOOL isFileUpdated()
	{
//		MY_TRACE(_T("FileUpdateChecker::isFileUpdated()\n"));

		FILETIME fileTime;

		if (!getFileTime(m_filePath, &fileTime))
			return FALSE;

		if (!::CompareFileTime(&m_fileTime, &fileTime))
			return FALSE;

		m_fileTime = fileTime;

		return TRUE;
	}

	static BOOL getFileTime(LPCWSTR filePath, FILETIME* fileTime)
	{
		HANDLE file = ::CreateFileW(filePath, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

		if (file == INVALID_HANDLE_VALUE)
			return FALSE;

		BOOL retValue = ::GetFileTime(file, 0, 0, fileTime);

		::CloseHandle(file);

		return retValue;
	}
};

typedef std::shared_ptr<FileUpdateChecker> FileUpdateCheckerPtr;
typedef std::list<FileUpdateCheckerPtr> FileUpdateCheckerContainer;

class FileUpdateCheckers
{
private:

	FileUpdateCheckerContainer m_items;

public:

	FileUpdateCheckers()
	{
//		MY_TRACE(_T("FileUpdateCheckers::FileUpdateCheckers()\n"));
	}

	void clear()
	{
//		MY_TRACE(_T("FileUpdateCheckers::clear()\n"));

		m_items.clear();
	}

	void add(LPCWSTR filePath)
	{
//		MY_TRACE(_T("FileUpdateCheckers::add(%ws)\n"), filePath);

		FileUpdateCheckerPtr ptr(new FileUpdateChecker(filePath));
		m_items.push_back(ptr);
	}

	BOOL isFileUpdated()
	{
//		MY_TRACE(_T("FileUpdateCheckers::isFileUpdated()\n"));

		for (auto item : m_items)
		{
			if (item->isFileUpdated())
				return TRUE;
		}

		return FALSE;
	}
};
