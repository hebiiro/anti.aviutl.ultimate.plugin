//
// テーブルヘッダーをテーブル要素に追加します。
//
function add_table_headers(table_element, json) {

	//
	// テーブルヘッダーの配列を作成して返します。
	//
	function create_table_headers(json) {

		// テーブルヘッダーの配列を作成します。
		const headers = [];

		// 中間点を走査します。
		for (let i = 0; i < json.midpt.length; i++) {

			// 中間点を取得します。
			const midpt = json.midpt[i];

			// テーブルヘッダーを追加します。
			headers.push({ "number": i, "frame": midpt.frame_begin });
		}

		{
			// 最後尾の中間点を取得します。
			const midpt = json.midpt.at(-1);

			// テーブルヘッダーを追加します。
			headers.push({ "number": json.midpt.length, "frame": midpt.frame_end });
		}

		// テーブルヘッダーの配列を返します。
		return headers;
	}

	// テーブルヘッダーの配列を取得します。
	const headers = create_table_headers(json);

	{
		const tr_element = create_element(table_element, "tr");

		create_element(tr_element, "th", [ [ "class", "fixed_corner" ] ], "中間点");
		for (header of headers) {
			create_element(tr_element, "th", [ [ "class", "midpt fixed_top" ] ], header.number + 1);
		}
	}

	{
		const tr_element = create_element(table_element, "tr");

		create_element(tr_element, "th", [ [ "class", "fixed_corner_2" ] ], "フレーム");
		for (header of headers) {
			create_element(tr_element, "th", [ [ "class", "midpt fixed_top_2" ] ], header.frame);
		}
	}
}

//
// テーブルフィルタの配列の原型を作成して返します。
//
function pre_create_table_filters(json) {

	// テーブルフィルタの配列です。
	const table_filters = [];

	// 最初の中間点を取得します。
	const midpt = json.midpt[0];

	// フィルタを走査します。
	for (let j = 0; j < midpt.filter.length; j++) {

		// フィルタを取得します。
		const filter = midpt.filter[j];

		// テーブル列数を取得します。
		const col_count = json.midpt.length + 2;

		// テーブル行要素を追加します。
		const tr_element = create_element(null, "tr");

		// テーブル行要素にフィルタ名を追加します。
		create_element(tr_element, "th",
			[ [ "class", "filter" ], [ "colspan", col_count ] ], filter.name);

		// テーブルフィルタを作成します。
		const table_filter = { "col_count": col_count,
			"index": j, "tr_element": tr_element,
			"table_tracks": [], "table_checks": [] };

		// テーブルフィルタを追加します。
		table_filters.push(table_filter);

		// テーブルフィルタにテーブルトラックを追加します。
		pre_create_table_track(table_filter, filter);

		// テーブルフィルタにテーブルチェックを追加します。
		pre_create_table_check(table_filter, filter, json.exdata[j]);
	}

	return table_filters;
}

//
// テーブルフィルタの配列を作成して返します。
//
function create_table_filters(json) {

	//
	// tr要素にtd要素を追加します。
	//
	function add_td_element(parent_element, value, id) {
		const td_element = create_element(parent_element, "td");
		create_element(td_element, "input",
			[ [ "type", "text" ], [ "id", id ], [ "name", id ], [ "value", value ] ]);
		return td_element;
	}

	// テーブルフィルタの配列の原型を作成します。
	const table_filters = pre_create_table_filters(json);

	// 中間点を走査します。
	for (let i = 0; i < json.midpt.length; i++) {

		// 中間点を取得します。
		const midpt = json.midpt[i];

		// フィルタを走査します。
		for (let j = 0; j < midpt.filter.length; j++) {

			// フィルタを取得します。
			const filter = midpt.filter[j];

			// トラックが存在しない場合は何もしません。
			if (!filter.track) continue;

			// テーブルフィルタを取得します。
			const table_filter = table_filters[j];

			// トラックを走査します。
			for (let k = 0; k < filter.track.length; k++) {

				// トラックを取得します。
				const track = filter.track[k];

				// テーブルトラックを取得します。
				const table_track = table_filter.table_tracks[k];

				// tr要素にトラックの左値を追加します。
				add_td_element(table_track.tr_element, track.value_left, `track-value_${i}_${j}_${k}`);

				// 最後の中間点の場合は
				if (midpt == json.midpt.at(-1)) {
					// tr要素にトラックの右値を追加します。
					add_td_element(table_track.tr_element, track.value_right, `track-value_${i + 1}_${j}_${k}`);
				}
			}
		}
	}

	// 最後尾のフィルタを取得します。
	const filter = json.midpt[0].filter.at(-1);

	// 最後尾のフィルタがExEdit::Filter::Flag::Outputの場合は
	if (filter.flag & 0x10) {

		// 最後尾のフィルタを先頭に移動します。
		table_filters.unshift(table_filters.pop());
	}

	// テーブルフィルタの配列を返します。
	return table_filters;
}

//
// テーブル行をテーブル要素に追加します。
//
function add_table_rows(table_element, json) {

	// テーブルフィルタの配列を作成しますす。
	const table_filters = create_table_filters(json);

	// テーブルフィルタを走査します。
	for (let table_filter of table_filters) {

		// テーブルフィルタの要素を追加します。
		table_element.appendChild(table_filter.tr_element);

		// テーブルトラックを走査します。
		for (let table_track of table_filter.table_tracks) {

			// テーブルトラックの要素を追加します。
			table_element.appendChild(table_track.tr_element);
		}

		// テーブルチェックを走査します。
		for (let table_check of table_filter.table_checks) {

			// テーブルチェックの要素を追加します。
			table_element.appendChild(table_check.tr_element);
		}
	}
}
