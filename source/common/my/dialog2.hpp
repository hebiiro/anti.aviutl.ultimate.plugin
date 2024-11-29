#pragma once

namespace my
{
	//
	// このクラスは変数をバインドできるダイアログです。
	//
	struct Dialog2 : Dialog
	{
		struct Prop {
			virtual void apply(Dialog* dialog) = 0;
		};

		template <typename T>
		struct Text : Prop {
			UINT id; T& value;
			Text(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->set_text(id, value);
			}
			void apply(Dialog* dialog) override {
				value = dialog->get_text(id);
			}
		};

		template <typename T>
		struct Int : Prop {
			UINT id; T& value;
			Int(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->set_int(id, value);
			}
			void apply(Dialog* dialog) override {
				value = dialog->get_int(id);
			}
		};

		template <typename T>
		struct UInt : Prop {
			UINT id; T& value;
			UInt(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->set_uint(id, value);
			}
			void apply(Dialog* dialog) override {
				value = dialog->get_uint(id);
			}
		};

		template <typename T>
		struct Float : Prop {
			UINT id; T& value;
			Float(Dialog* dialog, UINT id, T& value, LPCTSTR format) : id(id), value(value) {
				dialog->set_float(id, value, format);
			}
			void apply(Dialog* dialog) override {
				value = dialog->get_float(id);
			}
		};

		template <typename T>
		struct Check : Prop {
			UINT id; T& value;
			Check(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->set_check(id, value);
			}
			void apply(Dialog* dialog) override {
				value = !!dialog->get_check(id);
			}
		};

		template <typename T>
		struct ComboBox : Prop {
			UINT id; T& value;
			ComboBox(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->set_combobox_index(id, value);
			}
			template <typename... Tail>
			ComboBox(Dialog* dialog, UINT id, T& value, Tail&&... tail) : id(id), value(value) {
				dialog->init_combobox(id, std::forward<Tail>(tail)...);
				dialog->set_combobox_index(id, value);
			}
			void apply(Dialog* dialog) override {
				value = dialog->get_combobox_index(id);
			}
		};

		std::vector<std::shared_ptr<Prop>> props;

		template <typename T>
		void bind_text(UINT id, T& value) {
			props.emplace_back(std::make_shared<Text<T>>(this, id, value));
		};

		template <typename T>
		void bind_int(UINT id, T& value) {
			props.emplace_back(std::make_shared<Int<T>>(this, id, value));
		};

		template <typename T>
		void bind_uint(UINT id, T& value) {
			props.emplace_back(std::make_shared<UInt<T>>(this, id, value));
		};

		template <typename T>
		void bind_float(UINT id, T& value, LPCTSTR format) {
			props.emplace_back(std::make_shared<Float<T>>(this, id, value, format));
		};

		template <typename T>
		void bind_check(UINT id, T& value) {
			props.emplace_back(std::make_shared<Check<T>>(this, id, value));
		};

		template <typename T>
		void bind_combobox_index(UINT id, T& value) {
			props.emplace_back(std::make_shared<ComboBox<T>>(this, id, value));
		};

		template <typename T, typename... Tail>
		void bind_combobox_index(UINT id, T& value, LPCTSTR text, Tail&&... tail) {
			props.emplace_back(std::make_shared<ComboBox<T>>(this, id, value, text, std::forward<Tail>(tail)...));
		};

		void apply() {
			for (auto& prop : props)
				prop->apply(this);
		}

		int do_modal2(HWND parent)
		{
			::EnableWindow(parent, FALSE);
			int ret_value = __super::do_modal();
			::EnableWindow(parent, TRUE);
			::SetActiveWindow(parent);

			if (IDOK != ret_value)
				return ret_value;

			apply();

			return ret_value;
		}
	};
}
