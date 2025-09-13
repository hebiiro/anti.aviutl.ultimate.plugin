#pragma once

namespace apn::ult_combobox
{
	//
	// このクラスは自作コンボボックスのコアです。
	//
	struct Core : Node
	{
		//
		// テキストの描画に使用するパディングです。
		//
		inline static constexpr int c_padding = 4;

		//
		// このクラスは項目です。
		//
		struct Item {
			std::wstring text;
			void* data = {};
		};

		//
		// 項目の配列です。
		//
		std::vector<std::unique_ptr<Item>> items;

		//
		// フォントハンドルです。
		//
		HFONT font = {};

		//
		// カレントアイテムのインデックスです。
		//
		int current_index = CB_ERR;

		//
		// 本体の高さです。
		// WM_CREATE、WM_SETFONTでセットします。
		//
		int body_height = {};

		//
		// アイテムの高さです。
		//
		int item_height = {};

		//
		// ドロップダウンリストボックスの幅です。
		// 初期値の場合は本体の幅と同じになります。
		//
		int dropped_width = {};

		//
		// ドロップダウンリストボックスの高さです。
		// WM_WINDOWPOSCHANGINGでセットします。
		//
		int dropped_height = {};

		//
		// ドロップダウンリストボックスを水平方向にスクロールできる幅です。
		//
		int horizontal_extent = {};

		//
		// TRUEの場合は拡張UIになります。
		//
		BOOL extended_ui = {};

		//
		// デストラクタです。
		//
		virtual ~Core()
		{
		}

		//
		// ウィンドウテキストの文字数を返します。
		//
		int get_window_text_len()
		{
			if ((size_t)current_index >= items.size()) return 0;

			return (int)items[current_index]->text.length();
		}

		//
		// ウィンドウテキストを返します。
		//
		int get_window_text(int buffer_size, LPWSTR buffer)
		{
			if ((size_t)current_index >= items.size()) return 0;

			::lstrcpynW(buffer, items[current_index]->text.c_str(), buffer_size);

			return ::lstrlenW(buffer);
		}

		//
		// アイテムの数を返します。
		//
		int get_count()
		{
			return (int)items.size();
		}

		//
		// すべてのアイテムを削除します。
		//
		int reset_content()
		{
			my::scope_exit scope_exit([&]() {
				redraw();
			});

			items.clear();
			current_index = CB_ERR;

			return CB_OKAY;
		}

		//
		// アイテムを追加します。
		//
		int add_string(LPCWSTR text)
		{
			items.emplace_back(std::make_unique<Item>(text));

			return (int)items.size();
		}

		//
		// 指定された位置にアイテムを追加します。
		//
		int insert_string(int index, LPCWSTR text)
		{
			if (index == -1) return add_string(text);

			if ((size_t)index > items.size()) return CB_ERR;

			items.insert(items.begin() + index, std::make_unique<Item>(text));

			return index;
		}

		//
		// 指定された位置のアイテムを削除します。
		//
		int delete_string(int index)
		{
			if ((size_t)index >= items.size()) return CB_ERR;

			items.erase(items.begin() + index);

			if (current_index == index)
				set_cur_sel(-1);

			return (int)items.size();
		}

		//
		// 指定された位置のアイテムの文字数を返します。
		//
		int get_lb_text_len(int index)
		{
			if ((size_t)index >= items.size()) return CB_ERR;

			return (int)items[index]->text.length();
		}

		//
		// 指定された位置のアイテムのテキストを返します。
		//
		int get_lb_text(int index, LPWSTR buffer)
		{
			if ((size_t)index >= items.size()) return CB_ERR;

			::lstrcpyW(buffer, items[index]->text.c_str());

			return (int)items[index]->text.length();
		}

		//
		// 指定された位置のアイテムの文字列を返します。
		//
		void* get_item_data(int index)
		{
			if ((size_t)index >= items.size()) return {};

			return items[index]->data;
		}

		//
		// 指定された位置のアイテムのデータをセットします。
		//
		int set_item_data(int index, void* data)
		{
			if ((size_t)index >= items.size()) return CB_ERR;

			items[index]->data = data;

			return CB_OKAY;
		}

		//
		// 指定された文字列を持つアイテムのインデックスを返します。
		//
		int find_string(int start_index, LPCWSTR text, const auto& compare)
		{
			if (start_index != -1)
			{
				if ((size_t)start_index >= items.size()) return CB_ERR;
			}

			auto real_start_index = (size_t)(start_index + 1);

			for (auto i = real_start_index; i < items.size(); i++)
			{
				if (compare(items[i]->text.c_str(), text))
					return i;
			}

			for (auto i = size_t {}; i < real_start_index; i++)
			{
				if (compare(items[i]->text.c_str(), text))
					return i;
			}

			return CB_ERR;
		}

		//
		// 指定された文字列を持つアイテムのインデックスを返します。
		//
		int find_string(int start_index, LPCWSTR text)
		{
			return find_string(start_index, text, [](LPCWSTR lhs, LPCWSTR rhs)
			{
				return ::StrStrIW(lhs, rhs) == lhs;
			});
		}

		//
		// 指定された文字列と一致するアイテムのインデックスを返します。
		//
		int find_string_exact(int start_index, LPCWSTR text)
		{
			return find_string(start_index, text, [](LPCWSTR lhs, LPCWSTR rhs)
			{
				return ::StrCmpIW(lhs, rhs) == 0;
			});
		}

		//
		// 指定された文字列を持つアイテムを選択します。
		//
		int select_string(int start_index, LPCWSTR text)
		{
			auto index = find_string(start_index, text);

			if (index != CB_ERR)
				set_cur_sel(index);

			return index;
		}

		//
		// カレントインデックスを返します。
		//
		int get_cur_sel()
		{
			return current_index;
		}

		//
		// カレントインデックスをセットします。
		//
		int set_cur_sel(int index)
		{
			if ((size_t)index >= items.size()) return CB_ERR;

			auto prev_index = current_index;

			my::scope_exit scope_exit([&]() {
				if (current_index != prev_index) redraw();
			});

			return current_index = index;
		}

		//
		// アイテムの高さを返します。
		//
		int get_item_height(int index)
		{
			return item_height;
		}

		//
		// アイテムの高さをセットします。
		//
		int set_item_height(int index, int height)
		{
			item_height = height;

			return item_height;
		}

		//
		// ドロップダウンリストボックスの幅を返します。
		//
		int get_dropped_width()
		{
			return dropped_width;
		}

		//
		// ドロップダウンリストボックスの幅をセットします。
		//
		int set_dropped_width(int width)
		{
			if (width < 0) return CB_ERR;

			return dropped_width = width;
		}

		//
		// 拡張UI状態を返します。
		//
		BOOL get_extended_ui()
		{
			return this->extended_ui;
		}

		//
		// 拡張UI状態をセットします。
		//
		int set_extended_ui(BOOL extended_ui)
		{
			this->extended_ui = extended_ui;

			return CB_OKAY;
		}
	};
}
