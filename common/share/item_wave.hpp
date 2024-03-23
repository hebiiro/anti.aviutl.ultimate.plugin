#pragma once

namespace apn::item_wave::share
{
	struct c_prop_name {
		inline static constexpr LPCTSTR c_window_container = _T("window_container");
		inline static constexpr LPCTSTR c_dialog_container = _T("dialog_container");
	};

	struct Message {
		// client => host
		inline static constexpr uint32_t c_init = WM_APP + 1;
		inline static constexpr uint32_t c_cache_result = WM_APP + 2;
		inline static constexpr uint32_t c_frame_changed = WM_APP + 3;

		// host => client
		inline static constexpr uint32_t c_exit = WM_APP + 101;
		inline static constexpr uint32_t c_create_cache = WM_APP + 102;
		inline static constexpr uint32_t c_clear_caches = WM_APP + 103;
		inline static constexpr uint32_t c_redraw_window = WM_APP + 104;
		inline static constexpr uint32_t c_project_changed = WM_APP + 105;
		inline static constexpr uint32_t c_audio_changed = WM_APP + 106;
		inline static constexpr uint32_t c_mass_changed = WM_APP + 107;

		// reader => client
		inline static constexpr uint32_t c_receive_cache = WM_APP + 201;
	};

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

	struct Project {
		int32_t video_scale;
		int32_t video_rate;
		int32_t frame_number;
		int32_t scene_index;
		int32_t current_frame;
	};

	struct Audio {
		uint32_t id = 0;
		uint32_t flag = 0;
		int32_t frame_begin = 0;
		int32_t frame_end = 0;
		int32_t scene_set = 0;
		char file_name[MAX_PATH] = {};
		float volume = 1.0;
		int32_t play_begin = 0;
		float play_speed = 1.0;
		uint32_t layer_flag = 0;
	};

	struct Mass {
		BOOL show_bpm = FALSE; // BPMを表示するかどうかのフラグです。
		struct Tempo {
			int32_t orig = 0; // テンポの基準となるフレーム番号です。
			int32_t bpm = 120; // 1 分あたりの四分音符の数です。テンポの速さはこの変数だけで決まります。
			int32_t above = 4; // 楽譜に書いてある上の数字です(拍子)。テンポの速さには影響を与えません。
			int32_t below = 4; // 楽譜に書いてある下の数字です(分)。テンポの速さには影響を与えません。
		} tempo;
	};

	struct MainBuffer {
		HWND gui_window_container;
		HWND gui_dialog_container;
		HWND gui_window;
		HWND gui_dialog;
		WaveRequest wave_request;
		WaveResult wave_result;
		Project project;
		Audio audio;
		Mass host_mass;
		Mass client_mass;
	};

	struct ReaderBuffer : WaveResult {
	};

	inline struct Name
	{
		inline static FormatText mutex(DWORD pid)
		{
			return FormatText(_T("item_wave.mutex.%08X"), pid);
		}

		inline static FormatText main_buffer(DWORD pid)
		{
			return FormatText(_T("item_wave.main_buffer.%08X"), pid);
		}

		inline static FormatText reader_event(DWORD tid)
		{
			return FormatText(_T("item_wave.reader.event.{}"), tid);
		}

		inline static FormatText reader_buffer(DWORD tid)
		{
			return FormatText(_T("item_wave.reader.buffer.{}"), tid);
		}
	} name;
#if 0
	struct Manager
	{
		Mutex mutex;
		SimpleFileMappingT<MainBuffer> main_buffer;

		BOOL init(DWORD pid)
		{
			MY_TRACE_FUNC("{:#010x}", pid);

			mutex.init(0, FALSE, name.mutex(pid));
			main_buffer.init(name.main_buffer(pid));

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
#if 0
		std::shared_ptr<WaveRequest> get_wave_request()
		{
			MY_TRACE_FUNC("");

			auto shared = wave_request.getBuffer();
			if (!shared) return 0;
			return std::make_shared<WaveRequest>(*shared);
		}

		std::shared_ptr<Project> get_project()
		{
			MY_TRACE_FUNC("");

			auto shared = project.getBuffer();
			if (!shared) return 0;
			return std::make_shared<Project>(*shared);
		}

		std::shared_ptr<Audio> get_audio()
		{
			MY_TRACE_FUNC("");

			auto shared = audio.getBuffer();
			if (!shared) return 0;
			return std::make_shared<Audio>(*shared);
		}

		std::shared_ptr<Mass> get_host_mass()
		{
			MY_TRACE_FUNC("");

			auto shared = host_mass.getBuffer();
			if (!shared) return 0;
			return std::make_shared<Mass>(*shared);
		}

		BOOL set_host_mass(const Mass* mass)
		{
			MY_TRACE_FUNC("");

			Synchronizer sync(mutex);
			auto shared = host_mass.getBuffer();
			if (!shared) return FALSE;
			*shared = *mass;
			return TRUE;
		}

		std::shared_ptr<Mass> get_client_mass()
		{
			MY_TRACE_FUNC("");

			auto shared = client_mass.getBuffer();
			if (!shared) return 0;
			return std::make_shared<Mass>(*shared);
		}

		BOOL set_client_mass(const Mass* mass)
		{
			MY_TRACE_FUNC("");

			Synchronizer sync(mutex);
			auto shared = client_mass.getBuffer();
			if (!shared) return FALSE;
			*shared = *mass;
			return TRUE;
		}
#endif
	};
#endif
}
