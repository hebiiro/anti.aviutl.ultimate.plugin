#pragma once
#include "Dialog.h"

namespace Tools
{
	struct Dialog2 : Dialog
	{
		struct Prop {
			virtual void apply(Dialog* dialog) = 0;
		};

		template<class T>
		struct Int : Prop {
			UINT id; T& value;
			Int(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->setInt(id, value);
			}
			void apply(Dialog* dialog) override {
				value = dialog->getInt(id);
			}
		};

		template<class T>
		struct UInt : Prop {
			UINT id; T& value;
			UInt(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->setUInt(id, value);
			}
			void apply(Dialog* dialog) override {
				value = dialog->getUInt(id);
			}
		};

		template<class T>
		struct Float : Prop {
			UINT id; T& value;
			Float(Dialog* dialog, UINT id, T& value, LPCTSTR format) : id(id), value(value) {
				dialog->setFloat(id, value, format);
			}
			void apply(Dialog* dialog) override {
				value = dialog->getFloat(id);
			}
		};

		template<class T>
		struct Check : Prop {
			UINT id; T& value;
			Check(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->setCheck(id, value);
			}
			void apply(Dialog* dialog) override {
				value = !!dialog->getCheck(id);
			}
		};

		template<class T>
		struct ComboBox : Prop {
			UINT id; T& value;
			ComboBox(Dialog* dialog, UINT id, T& value) : id(id), value(value) {
				dialog->setComboBox(id, value);
			}
			template <class... Tail>
			ComboBox(Dialog* dialog, UINT id, T& value, LPCTSTR text, Tail&&... tail) : id(id), value(value) {
				dialog->setComboBox(id, value, text, std::forward<Tail>(tail)...);
			}
			void apply(Dialog* dialog) override {
				value = dialog->getComboBox(id);
			}
		};

		std::vector<std::shared_ptr<Prop>> props;

		template<class T>
		void setInt(UINT id, T& value) {
			props.emplace_back(std::make_shared<Int<T>>(this, id, value));
		};

		template<class T>
		void setUInt(UINT id, T& value) {
			props.emplace_back(std::make_shared<UInt<T>>(this, id, value));
		};

		template<class T>
		void setFloat(UINT id, T& value, LPCTSTR format) {
			props.emplace_back(std::make_shared<Float<T>>(this, id, value, format));
		};

		template<class T>
		void setCheck(UINT id, T& value) {
			props.emplace_back(std::make_shared<Check<T>>(this, id, value));
		};

		template<class T>
		void setComboBox(UINT id, T& value) {
			props.emplace_back(std::make_shared<ComboBox<T>>(this, id, value));
		};

		template<class T, class... Tail>
		void setComboBox(UINT id, T& value, LPCTSTR text, Tail&&... tail) {
			props.emplace_back(std::make_shared<ComboBox<T>>(this, id, value, text, std::forward<Tail>(tail)...));
		};

		void apply() {
			for (auto& prop : props)
				prop->apply(this);
		}

		int doModal2(HWND parent)
		{
			::EnableWindow(parent, FALSE);
			int retValue = __super::doModal();
			::EnableWindow(parent, TRUE);
			::SetActiveWindow(parent);

			if (IDOK != retValue)
				return retValue;

			apply();

			return retValue;
		}
	};
}
