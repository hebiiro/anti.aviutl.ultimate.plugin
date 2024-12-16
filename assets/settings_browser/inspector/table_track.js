//
// テーブルフィルタにテーブルトラックを追加します。
//
function pre_create_table_track(table_filter, filter) {

	// トラックが存在しない場合は何もしません。
	if (!filter.track) return;

	// トラックを走査します。
	for (let k = 0; k < filter.track.length; k++) {

		// トラックを取得します。
		const track = filter.track[k];

		// テーブル行要素を作成します。
		const tr_element = create_element(null, "tr");

		// テーブル行要素にトラック名を追加します。
		create_element(tr_element, "th",
			[ [ "class", "track fixed_left" ] ], track.name);

		// テーブルトラックを追加します。
		table_filter.table_tracks.push({
			"index": k, "tr_element": tr_element});
	}
}
