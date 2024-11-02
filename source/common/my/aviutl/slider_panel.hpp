#pragma once

namespace my::aviutl
{
	template <typename T>
	struct IntExchanger
	{
		inline static T value_from_text(const std::wstring& text)
		{
			return (T)_wtoi(text.c_str());
		}

		inline static std::wstring text_from_value(T value)
		{
			return std::to_wstring(value);
		}
	};

	template <typename T>
	struct FloatExchanger
	{
		inline static T value_from_text(const std::wstring& text)
		{
			return (T)_wtof(text.c_str());
		}

		inline static std::wstring text_from_value(T value)
		{
			return std::to_wstring(value);
		}
	};

	//
	// このクラスはAviUtlのスライダーコントロール群のように振る舞うパネルです。
	//
	template <typename T = int, typename Exchanger = IntExchanger<T>>
	struct SliderPanel
	{
		inline static constexpr int c_slider_min = 0;
		inline static constexpr int c_slider_max = 1000000;

		struct Slider : my::Window
		{
		};

		struct EditBox : my::Window
		{
		};

		Slider slider;
		EditBox editbox;

		T min_value = {};
		T max_value = {};

		//
		// パネルを初期化します。
		//
		BOOL init(T min_value, T max_value, HWND dialog, UINT slider_id, UINT editbox_id)
		{
			this->min_value = min_value;
			this->max_value = max_value;

			slider.subclass(::GetDlgItem(dialog, slider_id));
			editbox.subclass(::GetDlgItem(dialog, editbox_id));

			::SendMessage(slider, TBM_SETRANGEMIN, FALSE, c_slider_min);
			::SendMessage(slider, TBM_SETRANGEMAX, FALSE, c_slider_max);
			::SendMessage(slider, TBM_SETTICFREQ, (c_slider_min + c_slider_max) / 2, 0);

			return TRUE;
		}

		//
		// ユーザーがスライダーを操作したときに呼ばれます。
		//
		T on_change_pos() const
		{
			// スライダーの位置から値を取得します。
			auto value = get_value_from_slider();

			// 値をテキストに変換します。
			auto text = text_from_value(value);

			// エディットボックスのテキストを更新しますが、通知はしません。
			set_editbox_text_no_notify(editbox, text.c_str());

			// 値を返します。
			return value;
		}

		//
		// ユーザーがエディットボックスを操作したときに呼ばれます。
		//
		T on_change_text() const
		{
			// エディットボックスのテキストから値を取得します。
			auto value = get_value_from_editbox();

			// 値を位置に変換します。
			auto pos = pos_from_value(value);

			// スライダーの位置を更新しますが、通知はしません。
			::SendMessage(slider, TBM_SETPOS, TRUE, pos);

			// 値を返します。
			return value;
		}

		T value_from_pos(int pos) const
		{
			auto temp = pos - c_slider_min;
			temp *= max_value;
			temp /= c_slider_max;
			temp += min_value;
			return temp;
		}

		int pos_from_value(T value) const
		{
			auto temp = value - min_value;
			temp *= c_slider_max;
			temp /= max_value;
			temp += c_slider_min;
			return temp;
		}

		T value_from_text(const std::wstring& text) const
		{
			return Exchanger::value_from_text(text);
		}

		std::wstring text_from_value(T value) const
		{
			return Exchanger::text_from_value(value);
		}

		//
		// アプリから値を要求されたときに呼ばれます。
		//
		T get_value_from_slider() const
		{
			// スライダーから位置を取得します。
			auto pos = (int)::SendMessage(slider, TBM_GETPOS, 0, 0);

			// 位置を値に変換して返します。
			return value_from_pos(pos);
		}

		//
		// アプリから値を要求されたときに呼ばれます。
		//
		T get_value_from_editbox() const
		{
			// エディットボックスからテキスト取得します。
			auto text = get_window_text(editbox);

			// テキストを値に変換して返します。
			return value_from_text(text);
		}

		//
		// アプリから値を要求されたときに呼ばれます。
		//
		void get_value(T& value) const
		{
			value = get_value_from_slider();
		}

		//
		// アプリから値を設定されたときに呼ばれます。
		//
		void set_value(T value) const
		{
			// 値を位置に変換します。
			auto pos = pos_from_value(value);

			// スライダーの位置を更新します。
			::SendMessage(slider, TBM_SETPOS, 0, pos);

			// エディットボックスのテキストを更新します。
			on_change_pos();
		}
	};
}
