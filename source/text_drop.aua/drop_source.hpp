#pragma once

namespace apn::text_drop
{
	//
	// このクラスはドロップソースです。
	//
	struct DropSource : IDropSource, IDropSourceNotify
	{
		//
		// このクラスはドロップソースの生成を担当します。
		//
		struct Allocator
		{
			inline static auto create(int32_t mode, const std::wstring& exo_path)
			{
				return new DropSource(mode, exo_path);
			}

			inline static auto release(DropSource* p)
			{
				return delete p;
			}
		};

		//
		// このCOMオブジェクトの参照カウントです。
		//
		ULONG ref_count = 1;

		//
		// ドロップモードです。
		//
		int32_t mode;

		//
		// exoファイルのパスです。
		//
		std::wstring exo_path;

		//
		// コンストラクタです。
		//
		DropSource(int32_t mode, const std::wstring& exo_path)
			: mode(mode)
			, exo_path(exo_path)
		{
			MY_TRACE_FUNC("{}, {}", mode, exo_path);
		}

		//
		// デストラクタです。
		//
		~DropSource()
		{
			MY_TRACE_FUNC("");
		}

		//
		// IUnknownの実装です。
		//
		STDMETHODIMP_(ULONG) AddRef() override
		{
			MY_TRACE_FUNC("");

			return ::InterlockedIncrement(&ref_count);
		}

		//
		// IUnknownの実装です。
		//
		STDMETHODIMP_(ULONG) Release() override
		{
			MY_TRACE_FUNC("");

			auto result = ::InterlockedDecrement(&ref_count);
			if (result == 0) Allocator::release(this);
			return result;
		}

		//
		// IUnknownの実装です。
		//
		STDMETHODIMP QueryInterface(REFIID iid, void** ppv) override
		{
			MY_TRACE_FUNC("");

			if (iid == IID_IUnknown || iid == IID_IDropSource)
			{
				*ppv = static_cast<IDropSource*>(this);
			}
			else if (iid == IID_IDropSourceNotify)
			{
				*ppv = static_cast<IDropSourceNotify*>(this);
			}
			else
			{
				*ppv = nullptr;
				return E_NOINTERFACE;
			}

			AddRef();
			return S_OK;
		}

		//
		// IDropSourceの実装です。
		//
		STDMETHODIMP QueryContinueDrag(BOOL escape_pressed, DWORD key_state) override
		{
			MY_TRACE_FUNC("{}, {:#010x}", escape_pressed, key_state);

			// エスケープキーが押されている場合はドラッグアンドドロップを中止します。
			if (escape_pressed) return DRAGDROP_S_CANCEL;

			// 左クリックが解除された場合は
			if (!(key_state & MK_LBUTTON))
			{
				// exoファイルに書き込みます。
				app->write_exo(mode, exo_path);

				// ドラッグアンドドロップを完了します。
				return DRAGDROP_S_DROP;
			}

			// それ以外の場合はドラッグアンドドロップを継続します。
			return S_OK;
		}

		//
		// IDropSourceの実装です。
		//
		STDMETHODIMP GiveFeedback(DWORD effect) override
		{
			MY_TRACE_FUNC("{:#010x}", effect);

			return DRAGDROP_S_USEDEFAULTCURSORS;
		}

		//
		// IDropSourceNotifyの実装です。
		//
		STDMETHODIMP DragEnterTarget(HWND target) override
		{
			MY_TRACE_FUNC("{:#010x}", target);
			MY_TRACE_HWND(target);

			return S_OK;
		}
        
		//
		// IDropSourceNotifyの実装です。
		//
		STDMETHODIMP DragLeaveTarget() override
		{
			MY_TRACE_FUNC("");

			return S_OK;
		}
	};
}
