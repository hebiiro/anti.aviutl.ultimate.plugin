#pragma once

namespace my
{
	// 以下のクラスは標準スマートポインタでハンドルを管理します。

	namespace handle
	{
		inline const auto deleter = [](HANDLE x){ ::CloseHandle(x); };

		template <typename T = HANDLE>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HANDLE>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HANDLE>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace find_file
	{
		inline const auto deleter = [](HANDLE x){ if (x != INVALID_HANDLE_VALUE) ::FindClose(x); };

		template <typename T = HANDLE>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HANDLE>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HANDLE>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace win_hook
	{
		inline const auto deleter = [](HHOOK x){ ::UnhookWindowsHookEx(x); };

		template <typename T = HHOOK>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HHOOK>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HHOOK>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace win_event_hook
	{
		inline const auto deleter = [](HWINEVENTHOOK x){ ::UnhookWinEvent(x); };

		template <typename T = HWINEVENTHOOK>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HWINEVENTHOOK>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HWINEVENTHOOK>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace icon
	{
		inline const auto deleter = [](HICON x){ ::DestroyIcon(x); };

		template <typename T = HICON>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HICON>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HICON>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace menu
	{
		inline const auto deleter = [](HMENU x){ ::DestroyMenu(x); };

		template <typename T = HMENU>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HMENU>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HMENU>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace theme
	{
		inline const auto deleter = [](HTHEME x){ ::CloseThemeData(x); };

		template <typename T = HTHEME>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HTHEME>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HTHEME>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace gdi
	{
		inline const auto deleter = [](HGDIOBJ x){ ::DeleteObject(x); };

		template <typename T = HGDIOBJ>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HGDIOBJ>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HGDIOBJ>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace dc
	{
		inline const auto deleter = [](HDC x){ ::DeleteDC(x); };

		template <typename T = HDC>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HDC>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HDC>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	namespace dwp
	{
		inline const auto deleter = [](HDWP x){ if (x) ::EndDeferWindowPos(x); };

		template <typename T = HDWP>
		using unique_ptr = std::unique_ptr<typename std::remove_pointer<T>::type, decltype(deleter)>;

		template <typename T = HDWP>
		struct shared_ptr : std::shared_ptr<typename std::remove_pointer<T>::type>
		{
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<typename std::remove_pointer<T>::type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};

		template <typename T = HDWP>
		using weak_ptr = std::weak_ptr<typename std::remove_pointer<T>::type>;
	}

	// 以下のクラスは自作クラスでハンドルを管理します。

	struct Handle
	{
		HANDLE handle;
		Handle(HANDLE handle = nullptr) : handle(handle) {}
		~Handle() { if (handle) ::CloseHandle(handle); }
		operator HANDLE() { return handle; }
	};

	struct Icon
	{
		HICON handle;
		Icon(HICON handle = nullptr) : handle(handle) {}
		~Icon() { if (handle) ::DestroyIcon(handle); }
		operator HICON() { return handle; }
	};

	struct Menu
	{
		HMENU handle;
		Menu(HMENU handle = nullptr) : handle(handle) {}
		~Menu() { if (handle) ::DestroyMenu(handle); }
		operator HMENU() { return handle; }
	};

	struct Theme
	{
		HTHEME handle;
		Theme(HTHEME handle = nullptr) : handle(handle) {}
		~Theme() { if (handle) ::CloseThemeData(handle); }
		operator HTHEME() { return handle; }
	};

	template <typename T>
	struct GdiObj
	{
		T handle = nullptr;
		GdiObj(T handle) : handle(handle) {}
		~GdiObj() { if (handle) ::DeleteObject(handle); }
		operator T() { return handle; }
	};

	struct DC
	{
		HDC handle;
		DC(HDC handle = nullptr) : handle(handle) {}
		~DC() { if (handle) ::DeleteDC(handle); }
		operator HDC() { return handle; }
	};

	struct SimpleFileMapping
	{
		HANDLE handle = nullptr;
		BYTE* buffer = nullptr;

		SimpleFileMapping()
		{
		}

		SimpleFileMapping(DWORD size, LPCTSTR name)
		{
			init(size, name);
		}

		SimpleFileMapping(DWORD desired_access, BOOL inherit_handle, LPCTSTR name)
		{
			open(desired_access, inherit_handle, name);
		}

		~SimpleFileMapping()
		{
			::UnmapViewOfFile(buffer), buffer = nullptr;
			::CloseHandle(handle), handle = nullptr;
		}

		BOOL init(DWORD size, LPCTSTR name)
		{
			handle = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, name);
			if (!handle) return FALSE;

			buffer = (BYTE*)::MapViewOfFile(handle, FILE_MAP_WRITE, 0, 0, 0);
			if (!buffer) return FALSE;

			return TRUE;
		}

		BOOL open(DWORD desired_access, BOOL inherit_handle, LPCTSTR name)
		{
			handle = ::OpenFileMapping(desired_access, inherit_handle, name);
			if (!handle) return FALSE;

			buffer = (BYTE*)::MapViewOfFile(handle, FILE_MAP_WRITE, 0, 0, 0);
			if (!buffer) return FALSE;

			return TRUE;
		}

		BYTE* get_buffer()
		{
			return buffer;
		}
	};

	template <typename T>
	struct SimpleFileMappingT : SimpleFileMapping
	{
		SimpleFileMappingT() {}
		SimpleFileMappingT(LPCTSTR name) : SimpleFileMapping(sizeof(T), name) {}
		SimpleFileMappingT(DWORD desired_access, BOOL inherit_handle, LPCTSTR name) : SimpleFileMapping(desired_access, inherit_handle, name) {}
		~SimpleFileMappingT() {}
		BOOL init(LPCTSTR name) { return SimpleFileMapping::init(sizeof(T), name); }
		T* get_buffer() { return (T*)SimpleFileMapping::get_buffer(); }
		T* operator->() { return get_buffer(); }
		T& operator*() { return *get_buffer(); }
	};

	struct FileMapping
	{
		HANDLE handle;

		FileMapping(DWORD size, LPCTSTR name) : handle(::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, name)) {}
		~FileMapping() { ::CloseHandle(handle), handle = nullptr; }
		BYTE* alloc_buffer() { return (BYTE*)::MapViewOfFile(handle, FILE_MAP_WRITE, 0, 0, 0); }
		void free_buffer(BYTE* buffer) { ::UnmapViewOfFile(buffer); }

		struct Buffer
		{
			FileMapping* file_mapping;
			BYTE* buffer;

			Buffer(FileMapping* file_mapping) : file_mapping(file_mapping), buffer(file_mapping->alloc_buffer()) {}
			~Buffer() { file_mapping->free_buffer(buffer), buffer = nullptr; }
			BYTE* get() { return buffer; }
		};
	};

	template <typename T>
	struct SharedMemory
	{
		T* buffer;
		SharedMemory(HANDLE file_mapping) : buffer((T*)::MapViewOfFile(file_mapping, FILE_MAP_WRITE, 0, 0, 0)) {}
		~SharedMemory() { ::UnmapViewOfFile(buffer); }
		T* get() { return buffer; }
		operator T*() { return buffer; }
		T* operator->() { return buffer; }
	};

	struct Sync
	{
		virtual ~Sync() {}
		virtual DWORD lock(DWORD timeout = INFINITE) = 0;
		virtual BOOL unlock() = 0;
	};

	struct Synchronizer
	{
		Sync& sync;

		Synchronizer(Sync& sync, DWORD timeout = INFINITE) : sync(sync) { sync.lock(timeout); }
		virtual ~Synchronizer() { sync.unlock(); }
	};

	struct Mutex : Sync
	{
		HANDLE handle = nullptr;

		Mutex() {}
		Mutex(LPSECURITY_ATTRIBUTES sa, BOOL initial_owner, LPCTSTR name) { init(sa, initial_owner, name); }
		Mutex(DWORD desired_access, BOOL inherit_handle, LPCTSTR name) { open(desired_access, inherit_handle, name); }
		virtual ~Mutex() override { ::CloseHandle(handle); }
		virtual DWORD lock(DWORD timeout = INFINITE) override { return ::WaitForSingleObject(handle, timeout); }
		virtual BOOL unlock() override { return ::ReleaseMutex(handle); }
		BOOL init(LPSECURITY_ATTRIBUTES sa, BOOL initial_owner, LPCTSTR name) { return !!(handle = ::CreateMutex(sa, initial_owner, name)); }
		BOOL open(DWORD desired_access, BOOL inherit_handle, LPCTSTR name) { return !!(handle = ::OpenMutex(desired_access, inherit_handle, name)); }
		BOOL is_valid() const { return !!handle; }
		operator HANDLE() const { return handle; }
	};

	struct Event : Sync
	{
		HANDLE handle = nullptr;

		Event() {}
		Event(LPSECURITY_ATTRIBUTES sa, BOOL manual_reset, BOOL initial_state, LPCTSTR name) { init(sa, manual_reset, initial_state, name); }
		Event(DWORD desired_access, BOOL inherit_handle, LPCTSTR name) { open(desired_access, inherit_handle, name); }
		virtual ~Event() override { ::CloseHandle(handle); }
		virtual DWORD lock(DWORD timeout = INFINITE) override { return ::WaitForSingleObject(handle, timeout); }
		virtual BOOL unlock() override { return ::SetEvent(handle); }
		BOOL init(LPSECURITY_ATTRIBUTES sa, BOOL manual_reset, BOOL initial_state, LPCTSTR name) { return !!(handle = ::CreateEvent(sa, manual_reset, initial_state, name)); }
		BOOL open(DWORD desired_access, BOOL inherit_handle, LPCTSTR name) { return !!(handle = ::OpenEvent(desired_access, inherit_handle, name)); }
		BOOL is_valid() const { return !!handle; }
		operator HANDLE() const { return handle; }
	};

	struct CriticalSection : Sync
	{
		CRITICAL_SECTION cs;

		CriticalSection(BOOL do_init) { if (do_init) init(); }
		virtual ~CriticalSection() { ::DeleteCriticalSection(&cs); }
		virtual DWORD lock(DWORD timeout = INFINITE) override { ::EnterCriticalSection(&cs); return WAIT_OBJECT_0; }
		virtual BOOL unlock() override { ::LeaveCriticalSection(&cs); return TRUE; }
		void init() { ::InitializeCriticalSection(&cs); }
	};

	struct DeferWindowPos
	{
		HDWP dwp;

		DeferWindowPos(int c = 0) : dwp(BeginDeferWindowPos(c)) {}
		~DeferWindowPos() { if (dwp) ::EndDeferWindowPos(dwp); }
		operator HDWP() { return dwp; }
		BOOL set_window_pos(HWND hwnd, HWND insert_after, int x, int y, int w, int h, UINT flags) {
			return !!(dwp = ::DeferWindowPos(dwp, hwnd, insert_after, x, y, w, h, flags));
		}
		BOOL set_window_pos(HWND hwnd, HWND insert_after, LPCRECT rc, UINT flags) {
			return !!(dwp = my::defer_window_pos(dwp, hwnd, insert_after, rc, flags));
		}
	};
}
