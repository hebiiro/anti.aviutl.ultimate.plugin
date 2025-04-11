#pragma once

namespace apn::dark::skin
{
	//
	// このクラスはアイコンの編集を管理します。
	//
	inline struct IconEditor
	{
		//
		// このクラスはノードです。
		//
		struct Node
		{
			//
			// このクラスはアイコンのカラーです。
			//
			struct ChangeColor { COLORREF src, dst; };

			//
			// アイコンのカラーのコレクションです。
			//
			std::vector<ChangeColor> collection;
		};

		//
		// ノードのコレクションです。
		//
		std::map<std::wstring, std::shared_ptr<Node>> collection;

		//
		// 作成済みアイコンが存在すればそれを返します。
		//
		std::shared_ptr<Node> get(LPCWSTR icon_name) const
		{
			{
				// 名前付きアイコン編集用データを取得します。
				auto it = collection.find(icon_name);
				if (it != collection.end()) return it->second;
			}

			{
				// 名前なしアイコン編集用データを取得します。
				auto it = collection.find(L"");
				if (it != collection.end()) return it->second;
			}

			return nullptr;
		}

		//
		// 指定されたアイコンからスキン用アイコンを作成して返します。
		//
		HICON create(HICON orig_icon, LPCWSTR icon_name)
		{
			MY_TRACE_FUNC("{/hex}, {/}", orig_icon, icon_name);

			ICONINFO ii = {};
			::GetIconInfo(orig_icon, &ii);

			std::shared_ptr<Node> node;

			// 名前なしアイコン編集用データを取得します。
			auto it = collection.find(L"");
			if (it != collection.end()) node = it->second;

			// 名前付きアイコン編集用データを取得します。
			it = collection.find(icon_name);
			if (it != collection.end()) node = it->second;

			if (node) // アイコン編集用データが見つかったのでアイコンを編集します。
			{
				BITMAP bm = {};
				::GetObject(ii.hbmColor, sizeof(bm), &bm);

				auto w = bm.bmWidth;
				auto h = bm.bmHeight;

				my::dc::unique_ptr<> color_dc(::CreateCompatibleDC(nullptr));
				my::gdi::selector color_bitmap_selector(color_dc.get(), ii.hbmColor);
				my::dc::unique_ptr<> mask_dc(::CreateCompatibleDC(nullptr));
				my::gdi::selector mask_bitmap_selector(mask_dc.get(), ii.hbmMask);

				for (decltype(h) y = 0; y < h; y++)
				{
					for (decltype(w) x = 0; x < w; x++)
					{
						if (::GetPixel(mask_dc.get(), x, y))
							continue;

						auto color = ::GetPixel(color_dc.get(), x, y);

						for (const auto& change_color : node->collection)
						{
							if (color == change_color.src)
								::SetPixel(color_dc.get(), x, y, change_color.dst);
						}
					}
				}
			}

			auto icon = ::CreateIconIndirect(&ii);
			::DeleteObject(ii.hbmColor);
			::DeleteObject(ii.hbmMask);

			return icon;
		}

		//
		// 指定されたアイコンの配色を変更します。
		//
		void change_color(LPCWSTR icon_name, COLORREF src, COLORREF dst)
		{
			// ノードを作成し、コレクションに追加します。
			auto& node = collection[icon_name];
			if (!node) node = std::make_shared<Node>();

			if (src != CLR_NONE && dst != CLR_NONE)
				node->collection.push_back({ src, dst });
		}
	} icon_editor;

	//
	// このクラスはスキン用のアイコンを管理します。
	//
	inline struct IconManager
	{
		//
		// このクラスはスキン用アイコンを保持します。
		//
		struct Node
		{
			std::wstring name;
			my::icon::unique_ptr<> icon;

			Node(LPCWSTR name) : name(name) {}
		};

		//
		// ノードのコレクションです。
		//
		std::map<HICON, std::shared_ptr<Node>> collection;

		//
		// 指定されたアイコンに対応するノードを追加します。
		//
		void add(HICON icon, LPCWSTR icon_name)
		{
			MY_TRACE_FUNC("{/hex}, {/}", icon, icon_name);

			collection[icon] = std::make_shared<Node>(icon_name);
		}

		//
		// 指定されたアイコンに対応するスキン用アイコンを返します。
		//
		HICON get(HICON icon)
		{
			MY_TRACE_FUNC("{/hex}", icon);

			// アイコンに関連付けられているノードを取得します。
			auto it = collection.find(icon);
			if (it == collection.end()) return icon;
			auto node = it->second;
			if (!node) return icon;

			// すでにスキン用アイコンを持っている場合はそれを返します。
			if (node->icon) return node->icon.get();

			// スキン用アイコンを作成します。
			node->icon.reset(icon_editor.create(icon, node->name.c_str()));

			// スキン用アイコンを返します。
			return node->icon.get();
		}

		//
		// 編集データを消去します。
		//
		void clear()
		{
			MY_TRACE_FUNC("");

			for (auto& pair : collection)
				pair.second->icon.reset();

			icon_editor.collection.clear();
		}
	} icon_manager;
}
