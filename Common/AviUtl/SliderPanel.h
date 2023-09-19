#pragma once

namespace Tools::AviUtl
{
	//
	// このクラスはAviUtlのスライダーコントロール群のように振る舞うパネルです。
	//
	template <class T, class Exchanger>
	struct SliderPanel
	{
		static constexpr int SliderMin = 0;
		static constexpr int SliderMax = 1000000;

		struct Label : Tools::Window
		{
		};

		struct Slider : Tools::Window
		{
		};

		struct EditBox : Tools::Window
		{
		};

		Label label;
		Slider slider;
		EditBox editbox;

		T min = 0;
		T max = 0;
		int labelSize = 0;
		int editboxSize = 0;

		//
		// コンストラクタです。
		//
		SliderPanel(T min, T max, int labelSize = 3, int editboxSize = 3)
			: min(min)
			, max(max)
			, labelSize(labelSize)
			, editboxSize(editboxSize)
		{
		}

		//
		// パネルを初期化します。
		// dialogはダイアログです。
		// idはエディットボックスのIDです。
		//
		BOOL init(HWND dialog, UINT id)
		{
			label.subclass(::GetDlgItem(dialog, id - 2));
			slider.subclass(::GetDlgItem(dialog, id - 1));
			editbox.subclass(::GetDlgItem(dialog, id - 0));

			::SendMessage(slider, TBM_SETRANGEMIN, FALSE, SliderMin);
			::SendMessage(slider, TBM_SETRANGEMAX, FALSE, SliderMax);
			::SendMessage(slider, TBM_SETTICFREQ, (SliderMin + SliderMax) / 2, 0);

			return TRUE;
		}

		//
		// コントロールを移動します。
		//
		BOOL move(HDWP dwp, int x, int y, int w, int h)
		{
			::DeferWindowPos(dwp, label, 0, x, y, h * labelSize, h, SWP_NOZORDER | SWP_NOACTIVATE);
			::DeferWindowPos(dwp, slider, 0, x + h * labelSize, y, w - h * (labelSize + editboxSize), h, SWP_NOZORDER | SWP_NOACTIVATE);
			::DeferWindowPos(dwp, editbox, 0, x + w - h * editboxSize, y, h * editboxSize, h, SWP_NOZORDER | SWP_NOACTIVATE);

			return TRUE;
		}

		//
		// ユーザーがスライダーを操作したときに呼ばれます。
		//
		T onChangePos()
		{
			// スライダーの位置から値を取得します。
			T value = getValue(TRUE);

			// 値をテキストに変換します。
			TCHAR text[MAX_PATH] = {};
			value2text(value, text, std::size(text));

			// エディットボックスのテキストを更新しますが、通知はしません。
			setEditBoxTextNoNotify(editbox, text);

			// 値を返します。
			return value;
		}

		//
		// ユーザーがエディットボックスを操作したときに呼ばれます。
		//
		T onChangeText()
		{
			// エディットボックスのテキストから値を取得します。
			T value = getValue(FALSE);

			// 値を位置に変換します。
			int pos = value2pos(value);

			// スライダーの位置を更新しますが、通知はしません。
			::SendMessage(slider, TBM_SETPOS, TRUE, pos);

			// 値を返します。
			return value;
		}

		T pos2value(int pos) const
		{
			T temp = pos - SliderMin;
			temp *= this->max;
			temp /= SliderMax;
			temp += this->min;
			return temp;
		}

		int value2pos(T value) const
		{
			int temp = value - this->min;
			temp *= SliderMax;
			temp /= this->max;
			temp += SliderMin;
			return temp;
		}

		T text2value(LPCTSTR text) const
		{
			return Exchanger::text2value(text);
		}

		void value2text(T value, LPTSTR text, size_t textSize) const
		{
			Exchanger::value2text(value, text, textSize);
		}

		//
		// アプリから値を要求されたときに呼ばれます。
		//
		T getValue(BOOL fromSlider = TRUE) const
		{
			if (fromSlider)
			{
				// スライダーから位置を取得します。
				int pos = ::SendMessage(slider, TBM_GETPOS, 0, 0);

				// 位置を値に変換して返します。
				return pos2value(pos);
			}
			else
			{
				// エディットボックスからテキスト取得します。
				TCHAR text[MAX_PATH] = {};
				::GetWindowText(editbox, text, std::size(text));

				// テキストを値に変換して返します。
				return text2value(text);
			}
		}

		//
		// アプリから値を設定されたときに呼ばれます。
		//
		void setValue(T value) const
		{
			// 値を位置に変換します。
			int pos = value2pos(value);

			// スライダーの位置を更新して、ダイアログに通知します。
			::SendMessage(slider, TBM_SETPOSNOTIFY, 0, pos); // ここでonChangePos()が呼ばれることを想定しています。
		}
	};

	template <class T>
	struct IntExchanger
	{
		inline static T text2value(LPCTSTR text)
		{
			return (T)_ttoi(text);
		}

		inline static void value2text(T value, LPTSTR text, size_t textSize)
		{
			::StringCchPrintf(text, textSize, _T("%d"), value);
		}
	};

	template <class T>
	struct RealExchanger
	{
		inline static T text2value(LPCTSTR text)
		{
			return (T)_ttof(text);
		}

		inline static void value2text(T value, LPTSTR text, size_t textSize)
		{
			::StringCchPrintf(text, textSize, _T("%f"), value);
		}
	};
}
