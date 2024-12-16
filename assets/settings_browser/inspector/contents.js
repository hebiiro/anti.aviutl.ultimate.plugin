//
// コンテンツ要素に先頭部分を追加します。
//
function add_contents_header(contents_element, json) {

	// 先頭の中間点を取得します。
	const midpt = json.midpt[0];

	// ヘッダー要素を作成します。
	const div_element = create_element(
		contents_element, "div", [ [ "class", "header" ] ]);

	// オブジェクトの表示名要素を作成します。
	span_element = create_element(div_element, "span",
		[ [ "class", "display_name" ] ], `name=${midpt.display_name}`);

	// オブジェクトのインデックス要素を作成します。
	span_element = create_element(div_element, "span",
		[ [ "class", "index" ] ], `index=${midpt.index}`);

	// オブジェクトの所属レイヤー要素を作成します。
	span_element = create_element(div_element, "span",
		[ [ "class", "layer" ] ], `layer=${midpt.layer_set}`);

	// オブジェクトの所属シーン要素を作成します。
	span_element = create_element(div_element, "span",
		[ [ "class", "scene" ] ], `scene=${midpt.scene_set}`);

	// オブジェクトの所属グループ要素を作成します。
	span_element = create_element(div_element, "span",
		[ [ "class", "group" ] ], `group=${midpt.group_belong}`);
}

//
// コンテンツ要素に本体部分を追加します。
//
function add_contents_body(contents_element, json) {

	// テーブル要素を作成します。
	const table_element = create_element(
		contents_element, "table", [ [ "id", "main_table" ]]);

	// テーブルヘッダーをテーブル要素に追加します。
	add_table_headers(table_element, json);

	// テーブル行をテーブル要素に追加します。
	add_table_rows(table_element, json);
}

//
// 受け取ったセッティングでコンテンツ全体を構築します。
//
function build_contents(json) {

	// jsonが無効の場合は何もしません。
	if (!json) return false;

	{
		// コンテンツ要素を取得します。
		const contents_element = document.getElementById("contents");

		// コンテンツ要素が存在する場合は削除します。
		if (contents_element) contents_element.remove();
	}

	// コンテンツ要素を作成します。
	const contents_element = create_element(
		document.body, "div", [ [ "id", "contents" ]]);

	// コンテンツ要素に先頭部分を追加します。
	add_contents_header(contents_element, json);

	// コンテンツ要素に本体部分を追加します。
	add_contents_body(contents_element, json);
}
