#pragma once

namespace apn::item_list
{
	//
	// このクラスはアイテム一覧のリストです。
	//
	inline struct list_t
	{
		//
		// このクラスはリストの列です。
		//
		inline static constexpr struct column_t {
			inline static constexpr int32_t k_layer = 0;
			inline static constexpr int32_t k_status = 1;
			inline static constexpr int32_t k_name = 2;
			inline static constexpr int32_t k_info = 3;
			inline static constexpr int32_t k_start = 4;
			inline static constexpr int32_t k_range = 5;
			inline static constexpr int32_t k_max_size = 6;
			inline static constexpr my::Label labels[] = {
				{ k_layer, L"layer" },
				{ k_status, L"status" },
				{ k_name, L"name" },
				{ k_info, L"info" },
				{ k_start, L"start" },
				{ k_range, L"range" },
			};
			inline static constexpr struct data_t {
				LPCWSTR text; int32_t width; BOOL flag_editable; BOOL flag_number;
			} k_datas[] = {
				{ L"レイヤー", 100, FALSE, TRUE },
				{ L"グループ", 100, FALSE, TRUE },
				{ L"名前", 100, FALSE, FALSE },
				{ L"フィルタ", 200, FALSE, TRUE },
				{ L"位置", 100, TRUE, TRUE  },
				{ L"長さ", 100, TRUE, TRUE },
			};
		} k_column;

		//
		// このクラスはリストノードです。
		//
		struct node_t
		{
			int32_t orig_index = -1;
			int32_t object_index = -1;
			int32_t layer, frame_begin, frame_end;
			std::wstring values[k_column.k_max_size];

			//
			// 指定された列に対応する文字列を返します。
			//
			inline const std::wstring& get_text(int32_t col_index) const { return values[col_index]; }

			//
			// 指定された列に対応する文字列を変更します。
			//
			inline void set_text(int32_t col_index, const std::wstring& value) { values[col_index] = value; }
		};

		//
		// リストノードのコレクションです。
		//
		std::vector<std::shared_ptr<node_t>> nodes;

		//
		// ノードを新規作成して返します。
		//
		auto& create_node(const auto& object)
		{
			return nodes.emplace_back(std::make_shared<node_t>(
				(int32_t)nodes.size(), magi.exin.get_object_index(object),
				object->layer_set, object->frame_begin, object->frame_end));
		}
	} list;
}
