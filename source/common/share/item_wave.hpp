#pragma once

namespace apn::item_wave::share
{
	namespace host_window
	{
		namespace prop_name
		{
			inline static constexpr auto c_ui_window = _T("apn::item_wave::share::host_window::ui_window");
			inline static constexpr auto c_dark = _T("apn::item_wave::share::host_window::dark");
		}

		inline HWND get_ui_window(HWND host_window)
		{
			return (HWND)::GetProp(host_window, prop_name::c_ui_window);
		}

		inline HWND set_ui_window(HWND host_window, HWND ui_window)
		{
			return (HWND)::SetProp(host_window, prop_name::c_ui_window, ui_window);
		}

		inline BOOL get_dark(HWND host_window)
		{
			return (BOOL)::GetProp(host_window, prop_name::c_dark);
		}

		inline BOOL set_dark(HWND host_window, BOOL dark)
		{
			return (BOOL)::SetProp(host_window, prop_name::c_dark, (HANDLE)dark);
		}
	}

	inline static constexpr struct Message {
		inline static constexpr uint32_t c_init_process = WM_APP + 100;
		inline static constexpr uint32_t c_exit_process = WM_APP + 101;
		inline static constexpr uint32_t c_cache_result = WM_APP + 102;
		inline static constexpr uint32_t c_create_cache = WM_APP + 200;
		inline static constexpr uint32_t c_clear_caches = WM_APP + 201;
		inline static constexpr uint32_t c_receive_cache = WM_APP + 300;
	} c_message;

	struct Volume {
		inline static constexpr int32_t c_resolution = 30; // 時間軸方向の分解能をFPSで指定します。
		inline static constexpr int32_t c_max_count = c_resolution * 60 * 60 * 4; // 最大4時間
		float level;
	};

	struct WaveRequest {
		char file_name[MAX_PATH];
	};

	struct WaveResult {
		char file_name[MAX_PATH];
		int32_t volume_count;
		Volume volumes[Volume::c_max_count];
	};

	struct Audio {
		uint32_t id = 0;
		uint32_t flag = 0;
		int32_t frame_begin = 0;
		int32_t frame_end = 0;
		int32_t scene_set = 0;
		char file_name[MAX_PATH] = {};
		float volume = 1.0f;
		float play_begin = 0.0f;
		float play_speed = 1.0f;
		uint32_t layer_flag = 0;
	};

	struct MainBuffer {
		WaveRequest wave_request;
		WaveResult wave_result;
	};

	struct ReaderBuffer : WaveResult {
	};

	template <typename TYPE, std::size_t SIZE>
	inline LPSTR copy(TYPE (&array)[SIZE], const std::string& s)
	{
		return ::lstrcpynA(array, s.c_str(), std::size(array));
	}

	inline my::tstring get_completion_event_name(HWND host_window)
	{
		return my::format(_T("item_wave.completion_event.{:#010x}"), host_window);
	}

	inline my::tstring get_mutex_name(HWND host_window)
	{
		return my::format(_T("item_wave.mutex.{:#010x}"), host_window);
	}

	inline my::tstring get_main_buffer_name(HWND host_window)
	{
		return my::format(_T("item_wave.main_buffer.{:#010x}"), host_window);
	}

	inline my::tstring get_reader_event_name(DWORD tid)
	{
		return my::format(_T("item_wave.reader.event.{:#010x}"), tid);
	}

	inline my::tstring get_reader_buffer_name(DWORD tid)
	{
		return my::format(_T("item_wave.reader.buffer.{:#010x}"), tid);
	}
}
