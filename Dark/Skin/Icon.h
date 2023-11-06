#pragma once

namespace fgo::dark::skin
{
	//
	// このクラスはアイコンの編集を管理します。
	//
	inline struct IconEditor
	{
		struct Node
		{
			struct ChangeColor { COLORREF src, dst; };

			std::vector<ChangeColor> collection;

			HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
			{
				MY_TRACE_FUNC("");

				ChangeColor color = { CLR_NONE, CLR_NONE };
				getPrivateProfileColor(element, L"src", color.src);
				getPrivateProfileColor(element, L"dst", color.dst);

				if (color.src != CLR_NONE && color.dst != CLR_NONE)
					collection.push_back(color);

				return S_OK;
			}
		};

		std::map<_bstr_t, std::shared_ptr<Node>> collection;

		//
		// 作成済みアイコンが存在すればそれを返します。
		//
		std::shared_ptr<Node> get(LPCWSTR iconName) const
		{
			{
				// 名前付きアイコン編集用データを取得する。
				auto it = collection.find(iconName);
				if (it != collection.end()) return it->second;
			}

			{
				// 名前なしアイコン編集用データを取得する。
				auto it = collection.find(L"");
				if (it != collection.end()) return it->second;
			}

			return 0;
		}

		//
		// 指定されたアイコンからスキン用アイコンを作成して返します。
		//
		HICON create(HICON originalIcon, LPCWSTR iconName)
		{
			MY_TRACE_FUNC("0x%08X, %ws", originalIcon, iconName);

			ICONINFO ii = {};
			::GetIconInfo(originalIcon, &ii);

			std::shared_ptr<Node> node;

			// 名前なしアイコン編集用データを取得する。
			auto it = collection.find(L"");
			if (it != collection.end()) node = it->second;

			// 名前付きアイコン編集用データを取得する。
			it = collection.find(iconName);
			if (it != collection.end()) node = it->second;

			if (node) // アイコン編集用データが見つかったのでアイコンを編集する。
			{
				BITMAP bm = {};
				::GetObject(ii.hbmColor, sizeof(bm), &bm);
				int w = bm.bmWidth;
				int h = bm.bmHeight;

				HDC colorDC = ::CreateCompatibleDC(0);
				HBITMAP oldColorBitmap = (HBITMAP)::SelectObject(colorDC, ii.hbmColor);
				HDC maskDC = ::CreateCompatibleDC(0);
				HBITMAP oldMaskBitmap = (HBITMAP)::SelectObject(maskDC, ii.hbmMask);
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						if (::GetPixel(maskDC, x, y))
							continue;

						COLORREF color = ::GetPixel(colorDC, x, y);

						for (auto changeColor : node->collection)
						{
							if (color == changeColor.src)
								::SetPixel(colorDC, x, y, changeColor.dst);
						}
					}
				}
				::SelectObject(maskDC, oldMaskBitmap);
				::DeleteDC(maskDC);
				::SelectObject(colorDC, oldColorBitmap);
				::DeleteDC(colorDC);
			}

			HICON icon = ::CreateIconIndirect(&ii);
			::DeleteObject(ii.hbmColor);
			::DeleteObject(ii.hbmMask);

			return icon;
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			collection.clear();

			// <EditIcons>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"EditIcons");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				// <EditIcon>を読み込みます。
				MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"EditIcon");
				int c = nodeList->length;
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

					_bstr_t iconName = L"";
					getPrivateProfileString(element, L"iconName", iconName);

					auto node = std::make_shared<Node>();
					collection[iconName] = node;

					// <ChangeColor>を読み込みます。
					MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"ChangeColor");
					int c = nodeList->length;
					for (int i = 0; i < c; i++)
					{
						MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

						node->load(element);
					}
				}
			}

			return S_OK;
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
			_bstr_t name;
			IconHolder icon;

			Node(LPCWSTR name) : name(name) {}
		};

		//
		// ノードのコレクションです。
		//
		std::map<HICON, std::shared_ptr<Node>> collection;

		//
		// 指定されたアイコンに対応するノードを追加します。
		//
		void add(HICON icon, LPCWSTR iconName)
		{
			MY_TRACE_FUNC("0x%08X, %ws", icon, iconName);

			collection[icon] = std::make_shared<Node>(iconName);
		}

		//
		// 指定されたアイコンに対応するスキン用アイコンを返します。
		//
		HICON get(HICON icon)
		{
			MY_TRACE_FUNC("0x%08X", icon);

			// アイコンに関連付けられているノードを取得します。
			auto it = collection.find(icon);
			if (it == collection.end()) return icon;
			auto node = it->second;
			if (!node) return icon;

			// すでにスキン用アイコンを持っている場合はそれを返します。
			if (node->icon) return node->icon;

			// スキン用アイコンを作成します。
			node->icon = icon_editor.create(icon, node->name);

			// スキン用アイコンを返します。
			return node->icon;
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			for (auto pair : collection)
				pair.second->icon = 0;

			return icon_editor.load(element);
		}
	} icon_manager;
}
