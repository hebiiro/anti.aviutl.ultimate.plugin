#pragma once

namespace apn::reboot
{
	//
	// このクラスはパーツです。
	//
	struct Part
	{
		//
		// パーツのコレクションです。
		//
		inline static std::vector<Part*> parts;

		//
		// コンストラクタです。
		//
		Part() { parts.emplace_back(this); }

		virtual std::wstring on_get_name() = 0;
		virtual BOOL on_init(){ return TRUE; }
		virtual BOOL on_exit(){ return TRUE; }
		virtual BOOL on_read(n_json& root){ return TRUE; }
		virtual BOOL on_write(n_json& root){ return TRUE; }
		virtual BOOL on_update(AddinDialogInterface* addin_dialog){ return TRUE; }
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control){ return TRUE; }
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point){ return TRUE; }
		virtual BOOL on_update_config(AddinDialogInterface* addin_dialog){ return TRUE; }
		virtual BOOL on_update_controls(AddinDialogInterface* addin_dialog){ return TRUE; }
	};
}
