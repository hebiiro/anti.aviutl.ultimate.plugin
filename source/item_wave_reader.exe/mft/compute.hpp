#pragma once

namespace mft
{
#if 1
	//
	// このクラスは実行時間を計測してデバッグ出力します。
	//
	struct counter_t
	{
		const std::wstring label;
		LARGE_INTEGER freq = {}, start = {}, end = {};

		counter_t(const std::wstring& label) : label(label)
		{
			::QueryPerformanceFrequency(&freq);
			::QueryPerformanceCounter(&start);
		}

		~counter_t()
		{
			::QueryPerformanceCounter(&end);

			MY_TRACE("{/} = {/}秒\n", label, (end.QuadPart - start.QuadPart) / (double)freq.QuadPart);
		}
	};
#else
	//
	// このクラスは何もしません。
	// リリースバージョンで使用します。
	//
	struct counter_t { counter_t(const std::wstring& label) {} };
#endif
	//
	// このクラスはアライン済みのメモリを確保するためのアロケーターです。
	//
	template <typename T, size_t c_alignment>
	struct aligned_allocator
	{
		using value_type = T;

		template<typename U>
		struct rebind
		{
			using other = aligned_allocator<U, c_alignment>;
		};

		aligned_allocator() noexcept {}

		template<typename U>
		aligned_allocator(const aligned_allocator<U, c_alignment>&) noexcept {}

		T* allocate(size_t n)
		{
			auto ptr = _aligned_malloc(n * sizeof(T), c_alignment);
			if (!ptr) throw std::bad_alloc();
			return reinterpret_cast<T*>(ptr);
		}

		void deallocate(T* p, size_t)
		{
			_aligned_free(p);
		}

		using propagate_on_container_move_assignment = std::true_type;
		using is_always_equal = std::true_type;
	};
#if 1
	//
	// 指定された実数を音量範囲にマップします。(最適化版)
	//
	inline uint8_t map(float value)
	{
		auto mapped = (int)(value * 255.0f + 0.5f);
		mapped = (mapped & ~(mapped >> 31));          // max(mapped, 0)
		mapped = (mapped | ((255 - mapped) >> 31));   // min(mapped, 255)
		return (uint8_t)mapped;
	}
#else
	//
	// 指定された実数を音量範囲にマップします。
	//
	inline uint8_t map(float value)
	{
		auto mapped = (int)(value * 255.0f);
		mapped = std::max(mapped, 0);
		mapped = std::min(mapped, 255);

		return (uint8_t)mapped;
	}
#endif

#if 1
	//
	// 指定されたサンプルからRMSを算出して返します。(AVX2版)
	//
	inline uint8_t compute_rms(const float* samples, size_t size)
	{
		counter_t counter(L"AVX2でrms算出");

		// samplesが32byteアラインであることを前提にしています。
		assert(((uintptr_t)samples % 32) == 0);

		auto i = size_t {};
		auto sum = _mm256_setzero_ps();

		// 8要素ずつ処理します。
		for (; i + 8 <= size; i += 8) {
			auto s = _mm256_load_ps(samples + i);
			sum = _mm256_fmadd_ps(s, s, sum); // sum += s * s
		}

		// 水平加算（256 → 128 → スカラー）
		auto low  = _mm256_castps256_ps128(sum);
		auto high = _mm256_extractf128_ps(sum, 1);
		auto sum128 = _mm_add_ps(low, high);

		sum128 = _mm_hadd_ps(sum128, sum128);
		sum128 = _mm_hadd_ps(sum128, sum128);

		auto sum_squares = _mm_cvtss_f32(sum128);

		// 端数を処理します。
		for (; i < size; i++) {
			auto s = samples[i];
			sum_squares += s * s;
		}

		auto rms = std::sqrt(sum_squares / (float)size);
		return map(rms);
	}
#elif 0
	//
	// 指定されたサンプルからRMSを算出して返します。(AVX版)
	//
	inline uint8_t compute_rms(const float* samples, size_t size)
	{
		// samplesが32byteアラインであることを前提にしています。
		assert(((uintptr_t)samples % 32) == 0);

		auto i = size_t {};
		auto sum = _mm256_setzero_ps();

		for (; i + 8 <= size; i += 8) {
			auto s = _mm256_load_ps(samples + i);
			sum = _mm256_add_ps(sum, _mm256_mul_ps(s, s));
		}

		float tmp[8]; _mm256_storeu_ps(tmp, sum);
		auto sum_squares = 0.0f;
		for (auto j = 0; j < 8; j++) sum_squares += tmp[j];

		for (; i < size; i++) {
			auto s = samples[i];
			sum_squares += s * s;
		}

		auto rms = std::sqrt(sum_squares / size);
		return map(rms);
	}
#else
	//
	// 指定されたサンプルからRMSを算出して返します。(SSE版)
	//
	inline uint8_t compute_rms(const float* samples, size_t size)
	{
		// samplesが32byteアラインであることを前提にしています。
		assert(((uintptr_t)samples % 32) == 0);

		auto i = size_t {};
		auto sum = _mm_setzero_ps();

		// 4要素ずつ処理
		for (; i + 4 <= size; i += 4) {
			auto s = _mm_load_ps(samples + i);
			sum = _mm_add_ps(sum, _mm_mul_ps(s, s));
		}

		// 水平加算
		float tmp[4]; _mm_storeu_ps(tmp, sum);
		auto sum_squares = tmp[0] + tmp[1] + tmp[2] + tmp[3];

		// 端数処理
		for (; i < size; i++) {
			auto s = samples[i];
			sum_squares += s * s;
		}

		auto rms = std::sqrt(sum_squares / size);
		return map(rms);
	}
#endif

#if 1
	//
	// 指定されたサンプルからピークを算出して返します。(AVX2版)
	//
	inline uint8_t compute_peak(const float* samples, size_t size)
	{
		counter_t counter(L"AVX2でピーク算出");

		// samplesが32byteアラインであることを前提にしています。
		assert(((uintptr_t)samples % 32) == 0);

		auto i = size_t {};
		auto peak = _mm256_setzero_ps();

		// abs用マスク（符号ビット除去）
		const auto sign_mask = _mm256_set1_ps(-0.0f);

		// 8要素ずつ処理します。
		for (; i + 8 <= size; i += 8) {
			auto s = _mm256_load_ps(samples + i);

			// fabs: 符号ビットを消す
			auto abs_s = _mm256_andnot_ps(sign_mask, s);

			peak = _mm256_max_ps(peak, abs_s);
		}

		// 水平最大（256 → 128 → スカラー）
		auto low  = _mm256_castps256_ps128(peak);
		auto high = _mm256_extractf128_ps(peak, 1);
		auto max128 = _mm_max_ps(low, high);

		float tmp[4]; _mm_storeu_ps(tmp, max128);

		auto max_val = std::max(std::max(tmp[0], tmp[1]), std::max(tmp[2], tmp[3]));

		// 端数を処理します。
		for (; i < size; i++) {
			auto s = samples[i];
			max_val = std::max(max_val, std::abs(s));
		}

		return map(max_val);
	}
#else
	//
	// 指定されたサンプルからピークを算出して返します。(SSE版)
	//
	inline uint8_t compute_peak(const float* samples, size_t size)
	{
		auto i = size_t {};
		auto peak = _mm_setzero_ps();
		auto sign_mask = _mm_set1_ps(-0.0f); // abs用

		for (; i + 4 <= size; i += 4) {
			auto s = _mm_loadu_ps(samples + i);
			auto abs_s = _mm_andnot_ps(sign_mask, s); // fabs
			peak = _mm_max_ps(peak, abs_s);
		}

		float tmp[4]; _mm_storeu_ps(tmp, peak);
		auto max_val = std::max(std::max(tmp[0], tmp[1]), std::max(tmp[2], tmp[3]));

		for (; i < size; i++)
			max_val = std::max(max_val, std::abs(samples[i]));

		return map(max_val);
	}
#endif
}
