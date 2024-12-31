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
		}

		//
		// IUnknownの実装です。
		//
		virtual ULONG STDMETHODCALLTYPE AddRef() override
		{
			return ++ref_count;
		}

		//
		// IUnknownの実装です。
		//
		virtual ULONG STDMETHODCALLTYPE Release() override
		{
			if (--ref_count == 0)
			{
				Allocator::release(this);

				return 0;
			}

			return ref_count;
		}

		//
		// IUnknownの実装です。
		//
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv) override
		{
			if (iid == IID_IUnknown || iid == IID_IDropSource)
			{
				AddRef();
				*ppv = static_cast<IDropSource*>(this);
				return S_OK;
			}
			else if (iid == IID_IDropSourceNotify)
			{
				AddRef();
				*ppv = static_cast<IDropSourceNotify*>(this);
				return S_OK;
			}

			*ppv = nullptr;
			return E_NOINTERFACE;
		}

		//
		// IDropSourceの実装です。
		//
		virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape_pressed, DWORD key_state) override
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
		virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect) override
		{
			MY_TRACE_FUNC("{:#010x}", effect);

			return DRAGDROP_S_USEDEFAULTCURSORS;
		}

		//
		// IDropSourceNotifyの実装です。
		//
		virtual HRESULT STDMETHODCALLTYPE DragEnterTarget(HWND target) override
		{
			MY_TRACE_FUNC("{:#010x}", target);
			MY_TRACE_HWND(target);

			return S_OK;
		}
        
		//
		// IDropSourceNotifyの実装です。
		//
		virtual HRESULT STDMETHODCALLTYPE DragLeaveTarget() override
		{
			MY_TRACE_FUNC("");

			return S_OK;
		}
	};
}
