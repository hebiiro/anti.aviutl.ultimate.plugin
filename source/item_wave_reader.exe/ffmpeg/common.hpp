#pragma once

namespace ffmpeg
{
	//
	// このクラスはスコープ終了処理を実行します。
	//
	template <typename F>
	struct scope_exit {
		F func;
		~scope_exit() { func(); }
	};

	//
	// このクラスはユニークコンテキストです。
	//
	template <typename T, auto exit_func>
	struct unique_ctx
	{
		//
		// 生の変数です。
		//
		T raw = {};

		//
		// 生の変数をリセットします。
		//
		void reset(T new_raw = {}) { if (raw) exit_func(raw); raw = new_raw; }

		//
		// デフォルトコンストラクタです。
		//
		unique_ctx() {}

		//
		// コンストラクタです。
		//
		unique_ctx(T raw) : raw(raw) {}

		//
		// デストラクタです。
		//
		~unique_ctx() { reset(); }

		//
		// ムーブコンストラクタです。
		//
		unique_ctx(unique_ctx&& rhs) : raw(rhs.raw) { rhs.raw = {}; }

		//
		// ムーブ代入演算子です。
		//
		unique_ctx& operator=(unique_ctx&& rhs) { reset(rhs.raw); rhs.raw = {}; }

		//
		// アロー演算子です。
		//
		T operator->() const { return raw; }

		//
		// キャスト演算子です。
		//
		operator T() const { return raw; }

		//
		// アドレス演算子です。
		//
		T* operator&() { return &raw; }
		const T* operator&() const { return &raw; }
	};

	//
	// このクラスはフォーマットのコンテキストです。
	//
	using avformat_context = unique_ctx<AVFormatContext*, [](auto& ctx) { avformat_close_input(&ctx); }>;

	//
	// このクラスはコーデックのコンテキストです。
	//
	using avcodec_context = unique_ctx<AVCodecContext*, [](auto& ctx) { avcodec_free_context(&ctx); }>;

	//
	// このクラスはswsのコンテキストです。
	//
	using sws_context = unique_ctx<SwsContext*, [](auto& ctx) { sws_freeContext(ctx); }>;

	//
	// このクラスはswrのコンテキストです。
	//
	using swr_context = unique_ctx<SwrContext*, [](auto& ctx) { swr_free(&ctx); }>;

	//
	// このクラスはチャンネルレイアウトです。
	//
	using avchannel_layout = unique_ctx<AVChannelLayout, [](auto& ctx) { av_channel_layout_uninit(&ctx); }>;

	//
	// このクラスはパケットです。
	//
	using avpacket = unique_ctx<AVPacket*, [](auto& ctx) { av_packet_free(&ctx); }>;

	//
	// このクラスはフレームです。
	//
	using avframe = unique_ctx<AVFrame*, [](auto& ctx) { av_frame_free(&ctx); }>;
}
