#pragma once

namespace apn::text_editor
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// コンフィグとコントロールを更新します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			return addin_dialog.update_controls();
		}

		//
		// テキストアイテムのテキストをeditsにも反映します。
		//
		virtual BOOL to_edits() override
		{
			MY_TRACE_FUNC("");

			return addin_dialog.to_edits();
		}

		//
		// editsのテキストをテキストアイテムにも反映します。
		//
		virtual BOOL from_edits() override
		{
			MY_TRACE_FUNC("");

			return addin_dialog.from_edits();
		}
	} app_impl;
}
