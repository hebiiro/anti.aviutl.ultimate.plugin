#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスはアイコン関係のフックを担当します。
	//
	inline struct icon_t : entry_t::node_t
	{
		//
		// このクラスはアイコンを編集します。
		//
		struct editor_t
		{
			//
			// このクラスはノードです。
			//
			struct node_t
			{
				//
				// このクラスはアイコンのカラーです。
				//
				struct change_color_t { COLORREF src, dst; };

				//
				// アイコンのカラーのコレクションです。
				//
				std::vector<change_color_t> change_colors;
			};

			//
			// ノードのコレクションです。
			//
			std::map<std::wstring, std::shared_ptr<node_t>> nodes;

			//
			// 作成済みアイコンが存在すればそれを返します。
			//
			std::shared_ptr<node_t> get(LPCWSTR icon_name) const
			{
				{
					// 名前付きアイコン編集用データを取得します。
					auto it = nodes.find(icon_name);
					if (it != nodes.end()) return it->second;
				}

				{
					// 名前なしアイコン編集用データを取得します。
					auto it = nodes.find(L"");
					if (it != nodes.end()) return it->second;
				}

				return nullptr;
			}

			//
			// 指定されたアイコンから編集済みアイコンを作成して返します。
			//
			HICON create(HICON orig_icon, LPCWSTR icon_name)
			{
				MY_TRACE_FUNC("{/hex}, {/}", orig_icon, icon_name);

				// オリジナルのアイコン情報を取得します。
				ICONINFO ii = {};
				::GetIconInfo(orig_icon, &ii);
				my::gdi::unique_ptr<HBITMAP> color_bitmap(ii.hbmColor);
				my::gdi::unique_ptr<HBITMAP> mask_bitmap(ii.hbmMask);

				// ノードを作成します。
				std::shared_ptr<node_t> node;
				{
					// 名前なしアイコン編集用データを取得します。
					auto it = nodes.find(L"");
					if (it != nodes.end()) node = it->second;

					// 名前付きアイコン編集用データを取得します。
					it = nodes.find(icon_name);
					if (it != nodes.end()) node = it->second;
				}

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
							// マスクのピクセルが有効の場合は除外します。
							if (::GetPixel(mask_dc.get(), x, y)) continue;

							auto pixel_color = ::GetPixel(color_dc.get(), x, y);

							for (const auto& change_color : node->change_colors)
							{
								if (pixel_color == change_color.src)
									::SetPixel(color_dc.get(), x, y, change_color.dst);
							}
						}
					}
				}

				// 編集したアイコンを返します。
				return ::CreateIconIndirect(&ii);
			}

			//
			// 指定されたアイコンの配色を変更します。
			//
			void change_color(LPCWSTR icon_name, COLORREF src, COLORREF dst)
			{
				// ノードを作成し、コレクションに追加します。
				auto& node = nodes[icon_name];
				if (!node) node = std::make_shared<node_t>();

				if (src != CLR_NONE && dst != CLR_NONE)
					node->change_colors.emplace_back(src, dst);
			}
		} editor;

		//
		// このクラスはアイコンを管理します。
		//
		struct manager_t
		{
			//
			// このクラスはアイコンを保持します。
			//
			struct node_t
			{
				//
				// アイコンのリソース名です。
				//
				std::wstring name;

				//
				// アイコンのハンドルです。
				//
				my::icon::unique_ptr<> icon;
			};

			//
			// ノードのコレクションです。
			//
			std::map<HICON, std::shared_ptr<node_t>> nodes;

			//
			// 指定されたアイコンに対応するノードを追加します。
			//
			void add(HICON icon, LPCWSTR icon_name)
			{
				MY_TRACE_FUNC("{/hex}, {/}", icon, icon_name);

				nodes[icon] = std::make_shared<node_t>(icon_name);
			}

			//
			// 指定されたアイコンに対応する編集済みアイコンを返します。
			//
			HICON get(HICON icon)
			{
				MY_TRACE_FUNC("{/hex}", icon);

				// アイコンに関連付けられているノードを取得します。
				auto it = nodes.find(icon);
				if (it == nodes.end()) return icon;
				auto node = it->second;
				if (!node) return icon;

				// すでに編集済みアイコンを持っている場合はそれを返します。
				if (node->icon) return node->icon.get();

				// 編集済みアイコンを作成します。
				node->icon.reset(hook::icon.editor.create(icon, node->name.c_str()));

				// 編集済みアイコンを返します。
				return node->icon.get();
			}

			//
			// 編集データを消去します。
			//
			void clear()
			{
				MY_TRACE_FUNC("");

				for (auto& pair : nodes)
					pair.second->icon.reset();

				hook::icon.editor.nodes.clear();
			}
		} manager;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			// APIフックを開始します。
			{
				my::hook::detours detours;
//				my::hook::attach(LoadIconA);
//				my::hook::attach(LoadIconW);
				my::hook::attach(LoadImageA);
//				my::hook::attach(LoadImageW);
				my::hook::attach(DrawIconEx);
//				my::hook::attach(LoadMenuA);
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			// APIフックを終了します。
			{
				my::hook::detours detours;
//				my::hook::detach(LoadIconA);
//				my::hook::detach(LoadIconW);
				my::hook::detach(LoadImageA);
//				my::hook::detach(LoadImageW);
				my::hook::detach(DrawIconEx);
//				my::hook::detach(LoadMenuA);
			}

			return TRUE;
		}

		//
		// このクラスは::LoadIconA()をフックします。
		//
		struct {
			inline static HICON WINAPI hook_proc(HINSTANCE instance, LPCSTR icon_name)
			{
				MY_TRACE_FUNC("{/hex}, {/}", instance, safe_string(icon_name));

				return orig_proc(instance, icon_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadIconA;
		} LoadIconA;

		//
		// このクラスは::LoadIconW()をフックします。
		//
		struct {
			inline static HICON WINAPI hook_proc(HINSTANCE instance, LPCWSTR icon_name)
			{
				MY_TRACE_FUNC("{/hex}, {/}", instance, safe_string(icon_name));

				return orig_proc(instance, icon_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadIconW;
		} LoadIconW;

		//
		// このクラスは::LoadImageA()をフックします。
		//
		struct {
			inline static HANDLE WINAPI hook_proc(HINSTANCE instance, LPCSTR name, UINT type, int cx, int cy, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/}, {/}, {/hex}", instance, safe_string(name), type, cx, cy, flags);

				if ((my::addr_t)name > 0x0000FFFF && !::IsBadReadPtr(name, 1))
				{
					if (instance == ::GetModuleHandle(nullptr) && ::StrStrIA(name, "ICON_"))
					{
						MY_TRACE("AviUtlのアイコンをアイコンマネージャに追加します {/}, {/hex}\n", name, flags);

						auto str = "AVIUTL_"s + name;

						auto result = orig_proc(hive.instance, str.c_str(), type, cx, cy, flags);
						hook::icon.manager.add((HICON)result, my::ws(str).c_str());
						return result;
					}
					else if (instance == ::GetModuleHandle(_T("exedit.auf")) && ::StrStrIA(name, "ICON_"))
					{
						MY_TRACE("拡張編集のアイコンをアイコンマネージャに追加します {/}, {/hex}\n", name, flags);

						auto str = "EXEDIT_"s + name;

						auto result = orig_proc(hive.instance, str.c_str(), type, cx, cy, flags);
						hook::icon.manager.add((HICON)result, my::ws(str).c_str());
						return result;
					}
				}

				return orig_proc(instance, name, type, cx, cy, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadImageA;
		} LoadImageA;

		//
		// このクラスは::LoadImageW()をフックします。
		//
		struct {
			inline static HANDLE WINAPI hook_proc(HINSTANCE instance, LPCWSTR name, UINT type, int cx, int cy, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/}, {/}, {/hex}", instance, safe_string(name), type, cx, cy, flags);

				return orig_proc(instance, name, type, cx, cy, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadImageW;
		} LoadImageW;

		//
		// このクラスは::DrawIconEx()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, HICON icon, int w, int h, UINT step, HBRUSH brush, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}", dc, x, y, icon, w, h, step, brush, flags);

				icon = hook::icon.manager.get(icon);

				return orig_proc(dc, x, y, icon, w, h, step, brush, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawIconEx;
		} DrawIconEx;
	} icon;
}
