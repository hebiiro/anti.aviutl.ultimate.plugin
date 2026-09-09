#pragma once

//
// ※このファイルはほぼ全てClaude(無料版)によって生成されています。
//
namespace mft
{
	//
	// このクラスはIMFByteStreamをラップし、指定サイズ(既定100MB)単位で
	// ファイルをまとめて先読みキャッシュすることで、
	// MF内部からの細かい同期読み込みの回数を減らすためのクラスです。
	//
	// ・読み込み専用です(Write系は未対応、E_NOTIMPLを返します)。
	// ・ローカルファイル専用の簡易実装のため、BeginRead/EndReadも
	//   内部的には同期I/Oのまま「非同期の体裁」だけ整えて即時完了させます。
	// ・MFCreateSourceReaderFromByteStream() に渡して使用します。
	//
	class prefetch_byte_stream_t : public IMFByteStream
	{
		//
		// 参照カウントです。
		//
		std::atomic<ULONG> ref_count_ = 1;

		//
		// ファイルハンドルです。
		//
		my::handle::unique_ptr<> file_;

		//
		// ファイルサイズです。
		//
		LONGLONG file_size_ = {};

		//
		// 現在の論理読み込み位置です。
		//
		LONGLONG position_ = {};

		//
		// 先読みバッファのサイズです。(既定100MB)
		//
		size_t buffer_size_ = {};

		//
		// 先読みバッファです。
		//
		std::vector<BYTE> buffer_;

		//
		// バッファが保持しているファイル上の開始位置です。(負値は無効)
		//
		LONGLONG buffer_start_ = -1;

		//
		// バッファ内の有効なバイト数です。
		//
		size_t buffer_valid_size_ = {};

		//
		// スレッド保護用のロックです。
		//
		std::mutex mutex_;

		//
		// BeginRead/EndReadの結果(読み込んだバイト数)を運ぶための
		// 内部専用ラッパーです。IIDは持たないため直接static_castで復元します。
		//
		struct async_result_t : IUnknown
		{
			std::atomic<ULONG> ref_count = 1;
			ULONG cb_read = {};

			HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override
			{
				if (riid == IID_IUnknown) { *ppv = this; AddRef(); return S_OK; }
				*ppv = nullptr;
				return E_NOINTERFACE;
			}
			ULONG __stdcall AddRef() override { return ++ref_count; }
			ULONG __stdcall Release() override
			{
				auto c = --ref_count;
				if (c == 0) delete this;
				return c;
			}
		};

	public:
		//
		// コンストラクタです。指定されたファイルを開きます。
		//
		prefetch_byte_stream_t(const std::wstring& file_path, size_t buffer_size = 100 * 1024 * 1024)
			: buffer_size_(buffer_size)
		{
			MY_TRACE_FUNC("");
			MY_TRACE_STR(file_path);
			MY_TRACE_INT(buffer_size);

			file_.reset(::CreateFileW(file_path.c_str(),
				GENERIC_READ, FILE_SHARE_READ, nullptr,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
			if (file_.get() == INVALID_HANDLE_VALUE)
				throw L"prefetch_byte_stream_t: CreateFileW()が失敗しました";

			LARGE_INTEGER size = {};
			if (!::GetFileSizeEx(file_.get(), &size))
				throw L"prefetch_byte_stream_t: GetFileSizeEx()が失敗しました";
			file_size_ = size.QuadPart;

			// 先読みバッファをあらかじめ確保しておきます。
			buffer_.resize(buffer_size_);
		}

		//
		// IUnknown
		//

		HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override
		{
			if (!ppv) return E_POINTER;

			if (riid == IID_IUnknown || riid == __uuidof(IMFByteStream))
			{
				*ppv = static_cast<IMFByteStream*>(this);
				AddRef();
				return S_OK;
			}

			*ppv = nullptr;
			return E_NOINTERFACE;
		}

		ULONG __stdcall AddRef() override { return ++ref_count_; }

		ULONG __stdcall Release() override
		{
			auto c = --ref_count_;
			if (c == 0) delete this;
			return c;
		}

		//
		// IMFByteStream
		//

		HRESULT __stdcall GetCapabilities(DWORD* pdwCapabilities) override
		{
			if (!pdwCapabilities) return E_POINTER;

			*pdwCapabilities =
				MFBYTESTREAM_IS_READABLE |
				MFBYTESTREAM_IS_SEEKABLE |
				MFBYTESTREAM_DOES_NOT_USE_NETWORK;

			return S_OK;
		}

		HRESULT __stdcall GetLength(QWORD* pqwLength) override
		{
			if (!pqwLength) return E_POINTER;
			*pqwLength = (QWORD)file_size_;
			return S_OK;
		}

		HRESULT __stdcall SetLength(QWORD) override
		{
			return E_NOTIMPL; // 読み込み専用です。
		}

		HRESULT __stdcall GetCurrentPosition(QWORD* pqwPosition) override
		{
			if (!pqwPosition) return E_POINTER;
			std::lock_guard<std::mutex> lock(mutex_);
			*pqwPosition = (QWORD)position_;
			return S_OK;
		}

		HRESULT __stdcall SetCurrentPosition(QWORD qwPosition) override
		{
			std::lock_guard<std::mutex> lock(mutex_);
			position_ = (LONGLONG)qwPosition;
			return S_OK;
		}

		HRESULT __stdcall IsEndOfStream(BOOL* pfEndOfStream) override
		{
			if (!pfEndOfStream) return E_POINTER;
			std::lock_guard<std::mutex> lock(mutex_);
			*pfEndOfStream = (position_ >= file_size_) ? TRUE : FALSE;
			return S_OK;
		}

		HRESULT __stdcall Read(BYTE* pb, ULONG cb, ULONG* pcbRead) override
		{
			if (!pb || !pcbRead) return E_POINTER;

			std::lock_guard<std::mutex> lock(mutex_);

			auto total = ULONG {};

			while (total < cb)
			{
				// ファイル終端に達した場合はループを終了します。
				if (position_ >= file_size_) break;

				// 現在位置がバッファ範囲外の場合はまとめて先読みします。
				if (buffer_start_ < 0 ||
					position_ <  buffer_start_ ||
					position_ >= buffer_start_ + (LONGLONG)buffer_valid_size_)
				{
					if (!refill(position_)) return E_FAIL;
					if (buffer_valid_size_ == 0) break; // 想定外のEOFです。
				}

				auto offset = (size_t)(position_ - buffer_start_);
				auto avail  = buffer_valid_size_ - offset;
				auto n      = (std::min)(avail, (size_t)(cb - total));

				memcpy(pb + total, buffer_.data() + offset, n);

				position_ += (LONGLONG)n;
				total     += (ULONG)n;
			}

			*pcbRead = total;
			return S_OK;
		}

		HRESULT __stdcall BeginRead(BYTE* pb, ULONG cb, IMFAsyncCallback* pCallback, IUnknown* pState) override
		{
			if (!pb || !pCallback) return E_POINTER;

			// ローカルファイル専用の簡易実装のため、
			// 読み込み自体はここで同期的に完了させ、
			// IMFAsyncResultだけを作って即座に完了通知します。
			auto ar = new (std::nothrow) async_result_t {};
			if (!ar) return E_OUTOFMEMORY;

			auto hr = Read(pb, cb, &ar->cb_read);
			if (FAILED(hr)) { ar->Release(); return hr; }

			ComPtr<IMFAsyncResult> result;
			hr = ::MFCreateAsyncResult(ar, pCallback, pState, result.GetAddressOf());
			ar->Release();
			if (FAILED(hr)) return hr;

			return pCallback->Invoke(result.Get());
		}

		HRESULT __stdcall EndRead(IMFAsyncResult* pResult, ULONG* pcbRead) override
		{
			if (!pResult || !pcbRead) return E_POINTER;

			ComPtr<IUnknown> unk;
			auto hr = pResult->GetObject(unk.GetAddressOf());
			if (FAILED(hr)) return hr;

			// BeginRead()で自分が生成したasync_result_tだと分かっているため
			// 直接static_castで復元します。
			auto ar = static_cast<async_result_t*>(unk.Get());
			*pcbRead = ar->cb_read;
			return S_OK;
		}

		HRESULT __stdcall Write(const BYTE*, ULONG, ULONG*) override { return E_NOTIMPL; }
		HRESULT __stdcall BeginWrite(const BYTE*, ULONG, IMFAsyncCallback*, IUnknown*) override { return E_NOTIMPL; }
		HRESULT __stdcall EndWrite(IMFAsyncResult*, ULONG*) override { return E_NOTIMPL; }

		HRESULT __stdcall Seek(MFBYTESTREAM_SEEK_ORIGIN SeekOrigin, LONGLONG llSeekOffset,
			DWORD, QWORD* pqwCurrentPosition) override
		{
			std::lock_guard<std::mutex> lock(mutex_);

			auto base = (SeekOrigin == msoBegin) ? 0LL : position_;
			auto pos = base + llSeekOffset;
			if (pos < 0 || pos > file_size_) return E_INVALIDARG;

			position_ = pos;
			if (pqwCurrentPosition) *pqwCurrentPosition = (QWORD)position_;
			return S_OK;
		}

		HRESULT __stdcall Flush() override { return S_OK; }

		HRESULT __stdcall Close() override
		{
			std::lock_guard<std::mutex> lock(mutex_);
			file_.reset();
			return S_OK;
		}

	private:
		//
		// 指定位置からバッファサイズ分だけファイルをまとめて読み込みます。
		// (呼び出し側でmutex_をロック済みであることが前提です)
		//
		bool refill(LONGLONG start_pos)
		{
			counter_t counter(L"先読み");

			if (start_pos < 0 || start_pos >= file_size_)
			{
				buffer_start_ = start_pos;
				buffer_valid_size_ = 0;
				return true;
			}

			auto li = LARGE_INTEGER { .QuadPart = start_pos };
			if (!::SetFilePointerEx(file_.get(), li, nullptr, FILE_BEGIN))
				return false;

			auto want = (size_t)(std::min)((LONGLONG)buffer_size_, file_size_ - start_pos);
			auto total_read = size_t {};

			while (total_read < want)
			{
				auto n = DWORD {};
				auto remaining = (DWORD)(std::min)(want - total_read, (size_t)0xFFFFFFFFu);

				if (!::ReadFile(file_.get(), buffer_.data() + total_read, remaining, &n, nullptr))
					return false;

				if (n == 0) break; // 想定外のEOFです。
				total_read += n;
			}

			buffer_start_ = start_pos;
			buffer_valid_size_ = total_read;

//			MY_TRACE_INT(start_pos);
//			MY_TRACE_INT(total_read);

			return true;
		}
	};
}