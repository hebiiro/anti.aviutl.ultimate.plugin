#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはアクションのインターフェイスです。
	//
	inline struct ActionInterface {
		//
		// コンストラクタです。
		//
		ActionInterface() { action = this; }

		virtual BOOL refresh(const std::wstring& file_name = L"") = 0;
		virtual BOOL install(const std::shared_ptr<Agit::List::Node>& custom_object) = 0;
		virtual BOOL uninstall(const std::shared_ptr<Agit::List::Node>& custom_object) = 0;
		virtual BOOL open_script_file(const std::shared_ptr<Agit::List::Node>& custom_object) = 0;
		virtual BOOL open_script_folder(const std::shared_ptr<Agit::List::Node>& custom_object) = 0;
		virtual BOOL open_guide(const std::shared_ptr<Agit::List::Node>& custom_object) = 0;
		virtual BOOL show_option_dialog(const std::shared_ptr<Agit::List::Node>& custom_object) = 0;
		virtual BOOL begin_drag(const std::shared_ptr<Agit::List::Node>& custom_object) = 0;
		virtual BOOL filter_list() = 0;
	} *action = nullptr;
}
