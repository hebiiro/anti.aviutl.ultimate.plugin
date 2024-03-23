#pragma once

namespace my
{
	//
	// メモリアドレスの型です。
	//
	using addr_t = uintptr_t;

	template <typename T>
	struct case_insensitive_char_traits
	{
		using base_traits = std::char_traits<T>;

		using char_type = T;
		using int_type = int;
		using off_type = std::streamoff;
		using pos_type = std::streampos;
		using state_type = std::mbstate_t;
		using comparison_category = std::weak_ordering;

		static constexpr void assign(char_type& c1, const char_type& c2) noexcept
		{
			base_traits::assign(c1, c2);
		}

		static constexpr bool eq(char_type c1, char_type c2) noexcept
		{
			return base_traits::eq(std::tolower(c1), std::tolower(c2));
		}

		static constexpr bool lt(char_type c1, char_type c2) noexcept
		{
			return base_traits::lt(std::tolower(c1), std::tolower(c2));
		}

		static constexpr int compare(const char_type* s1, const char_type* s2, size_t n)
		{
			auto order = std::lexicographical_compare_three_way(s1, s1 + n, s2, s2 + n,
				[](char_type c1, char_type c2) -> std::weak_ordering { return std::tolower(c1) <=> std::tolower(c2); });
			return order == std::weak_ordering::equivalent ? 0 : order == std::weak_ordering::greater ? 1 : -1;
		}

		static constexpr size_t length(const char_type* s)
		{
			return base_traits::length(s);
		}

		static constexpr const char_type* find(const char_type* s, size_t n, const char_type& a)
		{
			return std::find_if(s, s + n, [a](char_type c){ return std::tolower(c) == a; });
		}

		static constexpr char_type* move(char_type* s1, const char_type* s2, size_t n)
		{
			return base_traits::move(s1, s2, n);
		}

		static constexpr char_type* copy(char_type* s1, const char_type* s2, size_t n)
		{
			return base_traits::copy(s1, s2, n);
		}

		static constexpr char_type* assign(char_type* s, size_t n, char_type a)
		{
			return base_traits::assign(s, n, a);
		}
	};

	using case_insensitive_string = std::basic_string<char, case_insensitive_char_traits<char>>;
	using case_insensitive_wstring = std::basic_string<wchar_t, case_insensitive_char_traits<wchar_t>>;

	//
	// std::stringをstd::wstringに変換して返します。
	// https://nprogram.hatenablog.com/entry/2018/01/03/113206
	//
	inline std::wstring cp_to_wide(const std::string &s, UINT codepage)
	{
		int in_length = (int)s.length();
		int out_length = MultiByteToWideChar(codepage, 0, s.c_str(), in_length, 0, 0); 
		std::wstring result(out_length, L'\0');
		if (out_length) MultiByteToWideChar(codepage, 0, s.c_str(), in_length, &result[0], out_length);
		return result;
	}

	//
	// std::wstringをstd::stringに変換して返します。
	// https://nprogram.hatenablog.com/entry/2018/01/03/113206
	//
	inline std::string wide_to_cp(const std::wstring &s, UINT codepage)
	{
		int in_length = (int)s.length();
		int out_length = WideCharToMultiByte(codepage, 0, s.c_str(), in_length, 0, 0, 0, 0); 
		std::string result(out_length, '\0');
		if (out_length) WideCharToMultiByte(codepage, 0, s.c_str(), in_length, &result[0], out_length, 0, 0); 
		return result;
	}

	//
	// printf()の%hs、%ws、%tsの代わりの処理です。
	//
	inline std::string hs(const std::string &s) { return s; }
	inline std::string hs(const std::wstring &s) { return wide_to_cp(s, CP_ACP); }
	inline std::wstring ws(const std::string &s) { return cp_to_wide(s, CP_ACP); }
	inline std::wstring ws(const std::wstring &s) { return s; }
#ifdef _UNICODE
	inline std::basic_string<TCHAR> ts(const std::string &s) { return ws(s); }
	inline std::basic_string<TCHAR> ts(const std::wstring &s) { return ws(s); }
#else
	inline std::basic_string<TCHAR> ts(const std::string &s) { return hs(s); }
	inline std::basic_string<TCHAR> ts(const std::wstring &s) { return hs(s); }
#endif

	template <typename T> auto format_arg(T value) { return value; }
	template <typename T> auto format_arg(T* value) { return (addr_t)value; }
	template <typename T> auto format_arg(const T* value) { return (addr_t)value; }
	inline std::basic_string<TCHAR> format_arg(char* s) { return ts(s); }
	inline std::basic_string<TCHAR> format_arg(wchar_t* s) { return ts(s); }
	inline std::basic_string<TCHAR> format_arg(const char* s) { return ts(s); }
	inline std::basic_string<TCHAR> format_arg(const wchar_t* s) { return ts(s); }
	inline std::basic_string<TCHAR> format_arg(const std::string &s) { return ts(s); }
	inline std::basic_string<TCHAR> format_arg(const std::wstring &s) { return ts(s); }

	//
	// std::format()にはハンドルや異なる文字コードセットの文字列を渡せない問題があるので、自分でカスタマイズしています。
	//
	template <typename... Args>
	std::basic_string<TCHAR> format(std::basic_string_view<TCHAR> fmt, const Args&... args)
	{
		return vformat(fmt, {std::make_wformat_args(format_arg(args)...)});
	}

	inline std::filesystem::path get_module_file_name(HINSTANCE instance, size_t buffer_length = MAX_PATH)
	{
		auto buffer = std::make_unique<TCHAR[]>(buffer_length);
		::GetModuleFileName(instance, buffer.get(), buffer_length);
		return buffer.get();
	}

	inline auto get_class_name(HWND hwnd, size_t buffer_length = MAX_PATH)
	{
		std::basic_string<TCHAR, case_insensitive_char_traits<TCHAR>> buffer;
		buffer.resize(buffer_length);
		const auto length = ::GetClassName(hwnd, &buffer[0], buffer.length());
		buffer.resize(length);
		return buffer;
	}

	inline auto get_window_text(HWND hwnd)
	{
		const auto buffer_length = ::GetWindowTextLength(hwnd) + 1;
		std::basic_string<TCHAR> buffer(buffer_length, _T('\0'));
		::GetWindowText(hwnd, &buffer[0], buffer.length());
		buffer.resize(buffer_length - 1);
		return buffer;
	}

	inline int get_width(const RECT& rc)
	{
		return rc.right - rc.left;
	}

	inline int get_height(const RECT& rc)
	{
		return rc.bottom - rc.top;
	}

	inline int get_center_x(const RECT& rc)
	{
		return (rc.left + rc.right) / 2;
	}

	inline int get_center_y(const RECT& rc)
	{
		return (rc.top + rc.bottom) / 2;
	}

	inline POINT lparam_to_point(LPARAM lParam)
	{
		return { (short)LOWORD(lParam), (short)HIWORD(lParam) };
	}

	inline LPARAM point_to_lparam(POINT pt)
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

	inline auto get_client_rect(HWND hwnd)
	{
		RECT rc; ::GetClientRect(hwnd, &rc); return rc;
	}

	inline auto get_window_rect(HWND hwnd)
	{
		RECT rc; ::GetWindowRect(hwnd, &rc); return rc;
	}

	//
	// ポイントが存在するモニタの矩形を返します。
	//
	inline auto get_monitor_rect(POINT point)
	{
		HMONITOR monitor = ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX monitor_info = {};
		monitor_info.cbSize = sizeof(monitor_info);
		::GetMonitorInfo(monitor, &monitor_info);
		return monitor_info.rcWork;
	}

	//
	// ウィンドウが表示されているモニタの矩形を返します。
	//
	inline auto get_monitor_rect(HWND hwnd)
	{
		HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX monitor_info = {};
		monitor_info.cbSize = sizeof(monitor_info);
		::GetMonitorInfo(monitor, &monitor_info);
		return monitor_info.rcWork;
	}

	inline void client_to_window(HWND hwnd, LPRECT rc)
	{
		RECT window_rc; ::GetWindowRect(hwnd, &window_rc);
		RECT client_rc; ::GetClientRect(hwnd, &client_rc);
		::MapWindowPoints(hwnd, 0, (LPPOINT)&client_rc, 2);

		rc->left += window_rc.left - client_rc.left;
		rc->top += window_rc.top - client_rc.top;
		rc->right += window_rc.right - client_rc.right;
		rc->bottom += window_rc.bottom - client_rc.bottom;
	}

	inline void window_to_client(HWND hwnd, LPRECT rc)
	{
		RECT window_rc; ::GetWindowRect(hwnd, &window_rc);
		RECT client_rc; ::GetClientRect(hwnd, &client_rc);
		::MapWindowPoints(hwnd, 0, (LPPOINT)&client_rc, 2);

		rc->left -= window_rc.left - client_rc.left;
		rc->top -= window_rc.top - client_rc.top;
		rc->right -= window_rc.right - client_rc.right;
		rc->bottom -= window_rc.bottom - client_rc.bottom;
	}

	inline auto set_window_rect(HWND hwnd, LPCRECT rc, UINT flags = 0)
	{
		int x = rc->left;
		int y = rc->top;
		int w = get_width(*rc);
		int h = get_height(*rc);

		return ::SetWindowPos(hwnd, 0, x, y, w, h, SWP_NOZORDER | flags);
	}

	//
	// hwnd2がhwnd1の祖先ウィンドウの場合はTRUEを返します。
	//
	inline BOOL is_ancestor(HWND hwnd1, HWND hwnd2)
	{
		return hwnd1 == hwnd2 || ::IsChild(hwnd2, hwnd1);
	}

	inline DWORD get_style(HWND hwnd)
	{
		return ::GetWindowLong(hwnd, GWL_STYLE);
	}

	inline DWORD set_style(HWND hwnd, DWORD style)
	{
		return ::SetWindowLong(hwnd, GWL_STYLE, style);
	}

	inline DWORD modify_style(HWND hwnd, DWORD remove, DWORD add)
	{
		DWORD style = get_style(hwnd);
		style &= ~remove;
		style |= add;
		return set_style(hwnd, style);
	}

	inline DWORD get_ex_style(HWND hwnd)
	{
		return ::GetWindowLong(hwnd, GWL_EXSTYLE);
	}

	inline DWORD set_ex_style(HWND hwnd, DWORD ex_style)
	{
		return ::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
	}

	inline DWORD modify_ex_style(HWND hwnd, DWORD remove, DWORD add)
	{
		DWORD ex_style = get_ex_style(hwnd);
		ex_style &= ~remove;
		ex_style |= add;
		return set_ex_style(hwnd, ex_style);
	}

	inline void set_editbox_text_no_notify(HWND editbox, LPCTSTR text)
	{
		UINT id = ::SetWindowLongPtr(editbox, GWLP_ID, 0);
		::SetWindowText(editbox, text);
		::SetWindowLongPtr(editbox, GWLP_ID, id);
	}

	inline void set_editbox_text_no_notify(HWND dialog, UINT id, LPCTSTR text)
	{
		set_editbox_text_no_notify(::GetDlgItem(dialog, id), text);
	}

	struct SimpleFileMapping
	{
		HANDLE handle = 0;
		BYTE* buffer = 0;

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
			::UnmapViewOfFile(buffer), buffer = 0;
			::CloseHandle(handle), handle = 0;
		}

		BOOL init(DWORD size, LPCTSTR name)
		{
			handle = ::CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, name);
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

		BYTE* getBuffer()
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
		T* getBuffer() { return (T*)SimpleFileMapping::getBuffer(); }
		T* operator->() { return getBuffer(); }
	};

	struct FileMapping
	{
		HANDLE handle;

		FileMapping(DWORD size, LPCTSTR name) : handle(::CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, name)) {}
		~FileMapping() { ::CloseHandle(handle), handle = 0; }
		BYTE* alloc_buffer() { return (BYTE*)::MapViewOfFile(handle, FILE_MAP_WRITE, 0, 0, 0); }
		void free_buffer(BYTE* buffer) { ::UnmapViewOfFile(buffer); }

		struct Buffer
		{
			FileMapping* file_mapping;
			BYTE* buffer;

			Buffer(FileMapping* file_mapping) : file_mapping(file_mapping), buffer(file_mapping->alloc_buffer()) {}
			~Buffer() { file_mapping->free_buffer(buffer), buffer = 0; }
			BYTE* get() { return buffer; }
		};
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
		HANDLE handle = 0;

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
		HANDLE handle = 0;

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
}
