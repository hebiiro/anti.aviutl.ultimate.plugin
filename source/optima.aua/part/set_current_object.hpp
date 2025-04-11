#pragma once

namespace apn::optima
{
	//
	// このクラスは拡張編集のset_current_object()をフックします。
	//
	inline struct SetCurrentObject : Part
	{
		//
		// コンストラクタです。
		//
		SetCurrentObject()
		{
			part_manager.add(this);
		}

		inline static void (CDECL* orig_proc)(int32_t object_index) = nullptr;
		inline static void CDECL hook_proc(int32_t object_index)
		{
			MY_TRACE_FUNC("{/}", object_index);

			Hive::Locker locker;

			if (!hive.reuse_combobox)
				return orig_proc(object_index);

			// カレントオブジェクトを取得します。
			ObjectPtr object;
			if (!object.init(object_index))
				return orig_proc(object_index);

			// コンボボックスのインデックスです。
			int32_t combobox_index = 0;

			// オブジェクトのフィルタを走査します。
			int32_t c = object.get_filter_count();
			for (int32_t i = 0; i < c; i++)
			{
				MY_TRACE_INT(combobox_index);

				// オブジェクトのフィルタを取得します。
				auto filter = object.get_filter(i);

				int32_t rest = 0;
				int32_t check_n = filter.get()->check_n;

				// フィルタがアニメーション効果の場合は
				if (filter.is_animation_effect())
				{
					// コンボボックスを取得します。
					auto hwnd = get_combobox(combobox_index);

					// フィルタのカテゴリを取得します。
					auto filter_category_id = animation_effect_manager.get_category_id(object.get(), filter.index());

					// コンボボックスのカテゴリを取得します。
					auto cached_category_id = get_category_id(hwnd);

					MY_TRACE("アニメーション効果 : {/}, {/hex}, {/}, {/}\n",
						filter.index(), (uint32_t)hwnd, filter_category_id, cached_category_id);

					// カテゴリが異なる場合は
					if (filter_category_id != cached_category_id)
					{
						// フィルタのカテゴリを持つコンボボックスが存在する場合は
						auto cached_combobox_index = find_cached_combobox_index(combobox_index,  filter_category_id);
						if (cached_combobox_index > combobox_index)
						{
							MY_TRACE("cached : {/} <=> {/}\n", combobox_index, cached_combobox_index);

							// コンボボックスを入れ替えます。
							swap_combobox(combobox_index, cached_combobox_index);

							// このコンボボックスは残りから除外します。
							rest++;
						}
					}
					// カテゴリが一致する場合は
					else
					{
						// このコンボボックスは残りから除外します。
						rest++;
					}
				}

				// 残りのコンボボックスを走査します。
				for (int32_t j = rest; j < check_n; j++)
				{
					// 残りのコンボボックスのインデックスを取得します。
					auto rest_combobox_index = combobox_index + j;

					// 残りのコンボボックスを取得します。
					auto hwnd = get_combobox(rest_combobox_index);

					// コンボボックスのカテゴリを取得します。
					auto cached_category_id = get_category_id(hwnd);

					MY_TRACE("残りのコンボボックス : {/}, {/hex}, {/}\n",
						rest_combobox_index, (uint32_t)hwnd, cached_category_id);

					// コンボボックスのカテゴリが有効の場合は
					if (cached_category_id)
					{
						// カテゴリが無効なコンボボックスが存在する場合は
						auto nocached_combobox_index = find_nocached_combobox_index(rest_combobox_index);
						if (nocached_combobox_index > rest_combobox_index)
						{
							MY_TRACE("nocached : {/} <=> {/}\n", rest_combobox_index, nocached_combobox_index);

							// コンボボックスを入れ替えます。
							// これによりカテゴリが有効なコンボボックスがそのまま残ります(キャッシュされます)。
							swap_combobox(rest_combobox_index, nocached_combobox_index);
						}
					}
				}

				// コンボボックスのインデックスを進めます。
				combobox_index += check_n;
			}

			dump_filter_controls();

			return orig_proc(object_index);
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) override
		{
			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			orig_proc = decltype(orig_proc)(magi.exin.exedit + 0x305E0);
			DetourAttach(&(PVOID&)orig_proc, hook_proc);

			DetourTransactionCommit();

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) override
		{
#if 0
			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			DetourDetach(&(PVOID&)orig_proc, hook_proc);

			DetourTransactionCommit();
#endif
			return TRUE;
		}
	} set_current_object;
}
