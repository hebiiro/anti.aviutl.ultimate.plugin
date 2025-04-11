#pragma once

namespace apn::audio_visualizer::share
{
	namespace host_window
	{
		namespace prop_name
		{
			inline static constexpr auto ui_window = _T("apn::audio_visualizer::share::host_window::ui_window");
			inline static constexpr auto dark = _T("apn::audio_visualizer::share::host_window::dark");
		}

		inline HWND get_ui_window(HWND host_window)
		{
			return (HWND)::GetProp(host_window, prop_name::ui_window);
		}

		inline HWND set_ui_window(HWND host_window, HWND ui_window)
		{
			return (HWND)::SetProp(host_window, prop_name::ui_window, ui_window);
		}

		inline BOOL get_dark(HWND host_window)
		{
			return (BOOL)::GetProp(host_window, prop_name::dark);
		}

		inline BOOL set_dark(HWND host_window, BOOL dark)
		{
			return (BOOL)::SetProp(host_window, prop_name::dark, (HANDLE)dark);
		}
	}

	inline static constexpr struct Message {
		inline static constexpr uint32_t c_init_process = WM_APP + 100;
		inline static constexpr uint32_t c_exit_process = WM_APP + 101;
		inline static constexpr uint32_t c_init_co_window = WM_APP + 102;
		inline static constexpr uint32_t c_exit_co_window = WM_APP + 103;
		inline static constexpr uint32_t c_update_option = WM_APP + 104;
		inline static constexpr uint32_t c_update_audio = WM_APP + 105;
		inline static constexpr uint32_t c_request_option = WM_APP + 106;
	} c_message;

	//
	// パワースペクトルの設定です。
	//
	struct SpectreOption
	{
		//
		// パワースペクトルの周波数範囲です。
		//
		double freq_min, freq_max;

		//
		// パワースペクトルの分割数です。
		//
		size_t div;

		//
		// パワースペクトルをRMS基準にスケーリングします。
		//
		BOOL rms_scaling;

		//
		// パワースペクトルの周波数を対数にします。
		//
		BOOL log_freq;
	};

	//
	// この構造体はオプションです。
	//
	struct Option
	{
		//
		// ビジュアル(のスクリプト)が描画する音声の範囲です。
		//
		struct Range {
			int32_t min;
			int32_t max;
		} range = { -60, +10 };

		//
		// ビジュアル(のスクリプト)が描画するマーカー(目安)です。
		//
		struct Marker {
			int32_t base;
			int32_t area;
		} marker = { 0, 5 };

		//
		// パワースペクトルの設定です。
		//
		SpectreOption spectre_option = { 100.0, 24000.0, 20, TRUE, TRUE };
	};

	//
	// この構造体は音声アーティファクトです。
	//
	struct AudioArtifact
	{
		DWORD time;
		int32_t frame;
		SpectreOption spectre_option;
		struct Channel {
			double level;
			double peak;
			struct Spectre {
				double frequency;
				double amplitude;
			} spectre[100];
		} channels[2];
	};

	//
	// 外部プロセスの準備が完了したときにシグナル状態になるイベントの名前を返します。
	// hwndはホストプロセスのウィンドウハンドルです。
	//
	inline my::tstring get_app_ready_event_name(HWND hwnd)
	{
		return my::format(_T("audio_visualizer_ui.app_ready_event.{/hex}"), hwnd);
	}

	inline void to_json(n_json& j, const Option::Range& range)
	{
		j = n_json {
			{ "min", range.min },
			{ "max", range.max },
		};
	}

	inline void from_json(const n_json& j, Option::Range& range)
	{
		j.at("min").get_to(range.min);
		j.at("max").get_to(range.max);
	}

	inline void to_json(n_json& j, const Option::Marker& marker)
	{
		j = n_json {
			{ "base", marker.base },
			{ "area", marker.area },
		};
	}

	inline void from_json(const n_json& j, Option::Marker& marker)
	{
		j.at("base").get_to(marker.base);
		j.at("area").get_to(marker.area);
	}

	inline void to_json(n_json& node, const SpectreOption& spectre_option)
	{
		write_int(node, "freq_min", spectre_option.freq_min);
		write_int(node, "freq_max", spectre_option.freq_max);
		write_int(node, "div", spectre_option.div);
		write_bool(node, "rms_scaling", spectre_option.rms_scaling);
		write_bool(node, "log_freq", spectre_option.log_freq);
	}

	inline void from_json(const n_json& node, SpectreOption& spectre_option)
	{
		read_int(node, "freq_min", spectre_option.freq_min);
		read_int(node, "freq_max", spectre_option.freq_max);
		read_int(node, "div", spectre_option.div);
		read_bool(node, "rms_scaling", spectre_option.rms_scaling);
		read_bool(node, "log_freq", spectre_option.log_freq);
	}

	inline void to_json(n_json& j, const AudioArtifact::Channel::Spectre& spectre)
	{
		j = n_json {
			{ "frequency", spectre.frequency },
			{ "amplitude", spectre.amplitude },
		};
	}

	inline void from_json(const n_json& j, AudioArtifact::Channel::Spectre& spectre)
	{
		j.at("frequency").get_to(spectre.frequency);
		j.at("amplitude").get_to(spectre.amplitude);
	}

	inline void to_json(n_json& j, const AudioArtifact::Channel& channel)
	{
		j = n_json {
			{ "level", channel.level },
			{ "peak", channel.peak },
			{ "spectre", channel.spectre },
		};
	}

	inline void from_json(const n_json& j, AudioArtifact::Channel& channel)
	{
		j.at("level").get_to(channel.level);
		j.at("peak").get_to(channel.peak);
		j.at("spectre").get_to(channel.spectre);
	}
}
