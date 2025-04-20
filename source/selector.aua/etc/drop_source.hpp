#pragma once

namespace apn::selector
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
			inline static auto create(const std::wstring& text)
			{
				return new DropSource(text);
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
		// ドラッグ用のオブジェクトです。
		//
		Dragger dragger;

		//
		// コンストラクタです。
		//
		DropSource(const std::wstring& text)
		{
			MY_TRACE_FUNC("");

			dragger.start(text);
		}

		//
		// デストラクタです。
		//
		virtual ~DropSource()
		{
			MY_TRACE_FUNC("");
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
			MY_TRACE_FUNC("{/}, {/hex}", escape_pressed, key_state);

			// エスケープキーが押されている場合は
			if (escape_pressed)
			{
				// ドラッグアンドドロップを中止します。
				return dragger.stop(), DRAGDROP_S_CANCEL;
			}

			// 左クリックが解除された場合は
			if (!(key_state & MK_LBUTTON))
			{
				// ドラッグアンドドロップを完了します。
				return dragger.stop(), DRAGDROP_S_DROP;
			}

			// それ以外の場合はドラッグアンドドロップを継続します。
			return S_OK;
		}

		//
		// IDropSourceの実装です。
		//
		virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect) override
		{
			MY_TRACE_FUNC("{/hex}", effect);

			dragger.move();

			return DRAGDROP_S_USEDEFAULTCURSORS;
		}

		//
		// IDropSourceNotifyの実装です。
		//
		virtual HRESULT STDMETHODCALLTYPE DragEnterTarget(HWND target) override
		{
			MY_TRACE_FUNC("{/hex}", target);
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
