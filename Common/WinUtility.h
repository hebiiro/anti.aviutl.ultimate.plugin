#pragma once

//--------------------------------------------------------------------

inline int getWidth(const RECT& rc)
{
	return rc.right - rc.left;
}

inline int getHeight(const RECT& rc)
{
	return rc.bottom - rc.top;
}

inline int getCenterX(const RECT& rc)
{
	return (rc.left + rc.right) / 2;
}

inline int getCenterY(const RECT& rc)
{
	return (rc.top + rc.bottom) / 2;
}

inline POINT LP2PT(LPARAM lParam)
{
	POINT pt =
	{
		(short)LOWORD(lParam),
		(short)HIWORD(lParam),
	};
	return pt;
}

inline LPARAM PT2LP(POINT pt)
{
	return MAKELPARAM(pt.x, pt.y);
}

inline POINT operator+(const POINT& a, const POINT& b)
{
	return POINT { a.x + b.x, a.y + b.y };
}

inline POINT operator-(const POINT& a, const POINT& b)
{
	return POINT { a.x - b.x, a.y - b.y };
}

inline void GetMonitorRect(POINT point, RECT& rect)
{
	// ウィンドウが表示されているディスプレイ情報を取得する
	HMONITOR monitor = ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monitor_info = {};
	monitor_info.cbSize = sizeof(monitor_info);
	::GetMonitorInfo(monitor, &monitor_info);
	rect = monitor_info.rcWork;
}

inline void GetMonitorRect(HWND hWnd, RECT& rect)
{
	// ウィンドウが表示されているディスプレイ情報を取得する
	HMONITOR monitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monitor_info = {};
	monitor_info.cbSize = sizeof(monitor_info);
	::GetMonitorInfo(monitor, &monitor_info);
	rect = monitor_info.rcWork;
}

inline void clientToWindow(HWND hwnd, LPRECT rc)
{
	RECT rcWindow; ::GetWindowRect(hwnd, &rcWindow);
	RECT rcClient; ::GetClientRect(hwnd, &rcClient);
	::MapWindowPoints(hwnd, 0, (LPPOINT)&rcClient, 2);

	rc->left += rcWindow.left - rcClient.left;
	rc->top += rcWindow.top - rcClient.top;
	rc->right += rcWindow.right - rcClient.right;
	rc->bottom += rcWindow.bottom - rcClient.bottom;
}

inline void windowToClient(HWND hwnd, LPRECT rc)
{
	RECT rcWindow; ::GetWindowRect(hwnd, &rcWindow);
	RECT rcClient; ::GetClientRect(hwnd, &rcClient);
	::MapWindowPoints(hwnd, 0, (LPPOINT)&rcClient, 2);

	rc->left -= rcWindow.left - rcClient.left;
	rc->top -= rcWindow.top - rcClient.top;
	rc->right -= rcWindow.right - rcClient.right;
	rc->bottom -= rcWindow.bottom - rcClient.bottom;
}

inline BOOL setWindowRect(HWND hwnd, LPCRECT rc, UINT flags = 0)
{
	int x = rc->left;
	int y = rc->top;
	int w = getWidth(*rc);
	int h = getHeight(*rc);

	return ::SetWindowPos(hwnd, 0, x, y, w, h, SWP_NOZORDER | flags);
}

//--------------------------------------------------------------------

inline DWORD getStyle(HWND hwnd)
{
	return ::GetWindowLong(hwnd, GWL_STYLE);
}

inline DWORD setStyle(HWND hwnd, DWORD style)
{
	return ::SetWindowLong(hwnd, GWL_STYLE, style);
}

inline DWORD modifyStyle(HWND hwnd, DWORD remove, DWORD add)
{
	DWORD style = getStyle(hwnd);
	style &= ~remove;
	style |= add;
	return setStyle(hwnd, style);
}

inline DWORD getExStyle(HWND hwnd)
{
	return ::GetWindowLong(hwnd, GWL_EXSTYLE);
}

inline DWORD setExStyle(HWND hwnd, DWORD exStyle)
{
	return ::SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
}

inline DWORD modifyExStyle(HWND hwnd, DWORD remove, DWORD add)
{
	DWORD exStyle = getExStyle(hwnd);
	exStyle &= ~remove;
	exStyle |= add;
	return setExStyle(hwnd, exStyle);
}

//--------------------------------------------------------------------

inline HANDLE createFileForRead(LPCTSTR filePath)
{
	return ::CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
}

inline HANDLE createFileForWrite(LPCTSTR filePath)
{
	return ::CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
}

inline HANDLE createFileForEdit(LPCTSTR filePath)
{
	return ::CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
}

inline HANDLE createFileForReadWrite(LPCTSTR filePath)
{
	return ::CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
}

inline DWORD readFile(HANDLE file, LPVOID buffer, DWORD numberOfBytesToRead, LPOVERLAPPED overlapped = 0)
{
	if (!::ReadFile(file, buffer, numberOfBytesToRead, &numberOfBytesToRead, overlapped))
		return 0;
	return numberOfBytesToRead;
}

inline DWORD writeFile(HANDLE file, LPCVOID buffer, DWORD numberOfBytesToWrite, LPOVERLAPPED overlapped = 0)
{
	if (!::WriteFile(file, buffer, numberOfBytesToWrite, &numberOfBytesToWrite, overlapped))
		return 0;
	return numberOfBytesToWrite;
}

inline DWORD writeFile(HANDLE file, LPCSTR text, LPOVERLAPPED overlapped = 0)
{
	DWORD numberOfBytesToWrite = 0;
	if (!::WriteFile(file, text, ::lstrlenA(text) * sizeof(text[0]), &numberOfBytesToWrite, overlapped))
		return 0;
	return numberOfBytesToWrite;
}

inline DWORD writeFile(HANDLE file, LPCWSTR text, LPOVERLAPPED overlapped = 0)
{
	DWORD numberOfBytesToWrite = 0;
	if (!::WriteFile(file, text, ::lstrlenW(text) * sizeof(text[0]), &numberOfBytesToWrite, overlapped))
		return 0;
	return numberOfBytesToWrite;
}

//--------------------------------------------------------------------

class Handle
{
private:

	HANDLE m_handle;

public:

	Handle()
	{
		m_handle = 0;
	}

	Handle(HANDLE handle)
	{
		m_handle = handle;
	}

	~Handle()
	{
		::CloseHandle(m_handle);
	}

private:

	Handle(const Handle& x)
	{
	}

public:

	operator HANDLE() const
	{
		return m_handle;
	}

	Handle& operator=(HANDLE handle)
	{
		if (m_handle)
			::CloseHandle(m_handle);

		m_handle = handle;

		return *this;
	}

	HANDLE* operator&()
	{
		return &m_handle;
	}
};

//--------------------------------------------------------------------

class IconHolder
{
private:

	HICON m_icon;

public:

	IconHolder()
	{
		m_icon = 0;
	}

	IconHolder(HICON icon)
	{
		m_icon = icon;
	}

	~IconHolder()
	{
		if (m_icon) ::DestroyIcon(m_icon);
	}

private:

	IconHolder(const IconHolder& x)
	{
	}

public:

	operator HICON() const
	{
		return m_icon;
	}

	IconHolder& operator=(HICON icon)
	{
		if (m_icon) ::DestroyIcon(m_icon);

		m_icon = icon;

		return *this;
	}

	HICON* operator&()
	{
		return &m_icon;
	}
};

//--------------------------------------------------------------------

class FormatText
{
public:

	TCHAR m_buffer[MAX_PATH];

public:

	FormatText(int value)
	{
		::StringCbPrintf(m_buffer, sizeof(m_buffer), _T("%d"), value);
	}

	FormatText(double value)
	{
		::StringCbPrintf(m_buffer, sizeof(m_buffer), _T("%.10g"), value);
	}

	FormatText(LPCTSTR format, ...)
	{
		va_list va;
		va_start(va, format);
		::StringCbVPrintf(m_buffer, sizeof(m_buffer), format, va);
		va_end(va);
	}

	operator LPCTSTR() const
	{
		return m_buffer;
	}
};

//--------------------------------------------------------------------

struct SimpleFileMapping
{
	HANDLE m_handle;
	BYTE* m_buffer;

	SimpleFileMapping()
		: m_handle(0)
		, m_buffer(0)
	{
	}

	SimpleFileMapping(DWORD size, LPCTSTR name)
		: m_handle(0)
		, m_buffer(0)
	{
		init(size, name);
	}

	SimpleFileMapping(DWORD desiredAccess, BOOL inheritHandle, LPCTSTR name)
	{
		open(desiredAccess, inheritHandle, name);
	}

	~SimpleFileMapping()
	{
		::UnmapViewOfFile(m_buffer), m_buffer = 0;
		::CloseHandle(m_handle), m_handle = 0;
	}

	BOOL init(DWORD size, LPCTSTR name)
	{
		MY_TRACE(_T("SimpleFileMapping::init(%d, %s)\n"), size, name);

		m_handle = ::CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, name);
		if (!m_handle) {
			MY_TRACE(_T("::CreateFileMapping() failed.\n"));
			return FALSE;
		}

		m_buffer = (BYTE*)::MapViewOfFile(m_handle, FILE_MAP_WRITE, 0, 0, 0);
		if (!m_buffer) {
			MY_TRACE(_T("::MapViewOfFile() failed.\n"));
			return FALSE;
		}

		return TRUE;
	}

	BOOL open(DWORD desiredAccess, BOOL inheritHandle, LPCTSTR name)
	{
		MY_TRACE(_T("SimpleFileMapping::open(%s)\n"), name);

		m_handle = ::OpenFileMapping(desiredAccess, inheritHandle, name);
		if (!m_handle) {
			MY_TRACE(_T("::OpenFileMapping() failed.\n"));
			return FALSE;
		}

		m_buffer = (BYTE*)::MapViewOfFile(m_handle, FILE_MAP_WRITE, 0, 0, 0);
		if (!m_buffer) {
			MY_TRACE(_T("::MapViewOfFile() failed.\n"));
			return FALSE;
		}

		return TRUE;
	}

	BYTE* getBuffer()
	{
		return m_buffer;
	}
};

template<typename T>
struct SimpleFileMappingT : public SimpleFileMapping
{
	SimpleFileMappingT(){}
	SimpleFileMappingT(LPCTSTR name) : SimpleFileMapping(sizeof(T), name){}
	SimpleFileMappingT(DWORD desiredAccess, BOOL inheritHandle, LPCTSTR name) : SimpleFileMapping(desiredAccess, inheritHandle, name){}
	~SimpleFileMappingT(){}
	BOOL init(LPCTSTR name){ return SimpleFileMapping::init(sizeof(T), name); }
	T* getBuffer(){ return (T*)SimpleFileMapping::getBuffer(); }
};

//--------------------------------------------------------------------

class FileMapping
{
public:

	HANDLE m_handle;

public:

	FileMapping(DWORD size, LPCTSTR name)
		: m_handle(0)
	{
		MY_TRACE(_T("FileMapping::FileMapping(%d, %s)\n"), size, name);

		m_handle = ::CreateFileMapping(
			INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, name);

		if (!m_handle)
		{
			MY_TRACE(_T("::CreateFileMapping() failed.\n"));
			return;
		}
	}

	~FileMapping()
	{
		::CloseHandle(m_handle), m_handle = 0;
	}

	BYTE* allocBuffer()
	{
		return (BYTE*)::MapViewOfFile(
			m_handle, FILE_MAP_WRITE, 0, 0, 0);
	}

	void freeBuffer(BYTE* buffer)
	{
		::UnmapViewOfFile(buffer);
	}

public:

	class Buffer
	{
	public:

		FileMapping* m_fileMapping;
		BYTE* m_buffer;

		Buffer(FileMapping* fileMapping)
		{
			m_fileMapping = fileMapping;
			m_buffer = m_fileMapping->allocBuffer();
		}

		~Buffer()
		{
			m_fileMapping->freeBuffer(m_buffer), m_buffer = 0;
		}

		BYTE* get()
		{
			return m_buffer;
		}
	};
};

//--------------------------------------------------------------------

class Sync
{
public:

	virtual DWORD lock(DWORD timeOut = INFINITE) = 0;
	virtual BOOL unlock() = 0;
};

class Synchronizer
{
public:

	Synchronizer(Sync& sync, DWORD timeOut = INFINITE)
		: m_sync(sync)
	{
		m_sync.lock(timeOut);
	}

	virtual ~Synchronizer()
	{
		m_sync.unlock();
	}

protected:

	Sync& m_sync;
};

class Mutex : public Sync
{
public:

	Mutex()
	{
		m_mutex = 0;
	}

	Mutex(LPSECURITY_ATTRIBUTES sa, BOOL initialOwner, LPCTSTR name)
	{
		init(sa, initialOwner, name);
	}

	virtual ~Mutex()
	{
		::CloseHandle(m_mutex);
	}

protected:

	HANDLE m_mutex;

public:

	void init(LPSECURITY_ATTRIBUTES sa, BOOL initialOwner, LPCTSTR name)
	{
		m_mutex = ::CreateMutex(sa, initialOwner, name);
	}

	virtual DWORD lock(DWORD timeOut = INFINITE)
	{
		return ::WaitForSingleObject(m_mutex, timeOut);
	}

	virtual BOOL unlock()
	{
		::ReleaseMutex(m_mutex);

		return true;
	}
};

class CriticalSection : public Sync
{
public:

	CriticalSection(BOOL doInit)
	{
		if (doInit) init();
	}

	virtual ~CriticalSection()
	{
		::DeleteCriticalSection(&m_cs);
	}

protected:

	CRITICAL_SECTION m_cs;

public:

	void init()
	{
		::InitializeCriticalSection(&m_cs);
	}

	virtual DWORD lock(DWORD timeOut = INFINITE)
	{
		::EnterCriticalSection(&m_cs);
		return WAIT_OBJECT_0;
	}

	virtual BOOL unlock()
	{
		::LeaveCriticalSection(&m_cs);

		return true;
	}
};

class Event : public Sync
{
public:

	Event()
	{
		m_handle = 0;
	}

	Event(LPSECURITY_ATTRIBUTES sa, BOOL manualReset, BOOL initialState, LPCTSTR name)
	{
		init(sa, manualReset, initialState, name);
	}

	Event(DWORD desiredAccess, BOOL inheritHandle, LPCTSTR name)
	{
		open(desiredAccess, inheritHandle, name);
	}

	virtual ~Event()
	{
		::CloseHandle(m_handle);
	}

protected:

	HANDLE m_handle;

public:

	BOOL init(LPSECURITY_ATTRIBUTES sa, BOOL manualReset, BOOL initialState, LPCTSTR name)
	{
		m_handle = ::CreateEvent(sa, manualReset, initialState, name);
		return isValid();
	}

	BOOL open(DWORD desiredAccess, BOOL inheritHandle, LPCTSTR name)
	{
		m_handle = ::OpenEvent(desiredAccess, inheritHandle, name);
		return isValid();
	}

	BOOL isValid() const
	{
		return !!m_handle;
	}

	operator HANDLE() const
	{
		return m_handle;
	}

	virtual DWORD lock(DWORD timeOut = INFINITE)
	{
		return ::WaitForSingleObject(m_handle, timeOut);
	}

	virtual BOOL unlock()
	{
		::SetEvent(m_handle);

		return true;
	}
};

//--------------------------------------------------------------------
