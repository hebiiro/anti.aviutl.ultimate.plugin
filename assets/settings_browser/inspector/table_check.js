//
// テーブルチェック生成関数のコレクションです。
//
const table_check_creator = {};

//
// テーブルフィルタにテーブルチェックを追加します。
//
function pre_create_table_check(table_filter, filter, exdata) {

	if (filter.id in table_check_creator) {
		table_check_creator[filter.id]({
			"table_filter": table_filter,
			"filter": filter,
			"exdata": exdata });
	}
}

//
// テーブルチェックのIDを作成して返します。
//
function table_check_id(args, name) {
	return `${name}_${args.table_filter.index}`;
}

//
// td要素を作成して返します。
//
function create_td_element(args, index) {

	// tr要素を作成します。
	const tr_element = create_element(null, "tr");

	// td要素を作成してtr要素に追加します。
	const td_element = create_element(tr_element, "td",
		[ [ "class", "check fixed02" ], [ "colspan", args.table_filter.col_count ] ]);

	// テーブルフィルタにテーブルチェックを追加します。
	args.table_filter.table_checks.push(
		{ "index": index, "tr_element": tr_element});

	// td要素を返します。
	return td_element;
}

//
// テキスト要素を作成して返します。
//
function create_text_element(args, parent_element, text) {
	return parent_element.append(text);
}

//
// select要素を作成して返します。
//
function create_select_element(args, parent_element, id_name, names, option_value, option_name) {

	const id = table_check_id(args, id_name);

	function is_selected(i, name) {
		if (option_name)
			return name == option_name;
		else
			return i == option_value;
	}

	const select_element = create_element(
		parent_element, "select", [ [ "class", "space" ], [ "id", id ], [ "name", id ] ]);
	for (let i = 0; i < names.length; i++) {
		const selected = is_selected(i, names[i]) ? "selected" : "";
		create_element(select_element, "option",
			[ [ "value", i ], [ selected, selected ] ], names[i]);
	}
	return select_element;
}

//
// text入力要素を作成して返します。
//
function create_textbox_element(args, parent_element, id_name, value, style) {
	const id = table_check_id(args, id_name);
	const attrs = [
		[ "type", "text" ],
		[ "class", "space" ],
		[ "style", style ],
		[ "id", id ],
		[ "name", id ],
		[ "value", value ] ];

	return create_element(parent_element, "input", attrs);
}

//
// check入力要素を作成して返します。
//
function create_checkbox_element(args, parent_element, id_name, name, value) {
	const id = table_check_id(args, id_name);
	const span_element = create_element(parent_element, "span");
	const checked = value ? "checked" : "";
	create_element(span_element, "input",
		[ [ "type", "checkbox" ], [ "class", "space" ], [ "id", id ], [ "name", id ], [ checked, checked ] ]);
	create_element(span_element, "label", [ [ "for", id ] ], name);
	return span_element;
}

//
// color入力要素を作成して返します。
//
function create_color_element(args, parent_element, id_name, value) {
	const id = table_check_id(args, id_name);
	return create_element(parent_element, "input",
		[ [ "type", "color" ], [ "class", "space" ], [ "id", id ], [ "name", id ], [ "value", value ] ]);
}

//
// file入力要素を作成して返します。
//
function create_file_element(args, parent_element, id_name, value) {
	const id = table_check_id(args, id_name);
	return create_element(parent_element, "input",
		[ [ "type", "file" ], [ "class", "space" ], [ "id", id ], [ "name", id ], [ "value", value ] ]);
}

//
// textarea要素を作成して返します。
//
function create_textarea_element(args, parent_element, id_name, value, style) {
	const id = table_check_id(args, id_name);
	return create_element(parent_element, "textarea",
		[ [ "rows", "20" ], [ "cols", "54" ], [ "id", id ], [ "name", id ], [ "style", style ] ], value);
}

//
// シーン用のselect要素を作成して返します。
//
function create_scene_select_element(args, parent_element, id_name, value) {
	const id = table_check_id(args, id_name);
	const select_element = create_element(parent_element,
		"select", [ [ "class", "space" ], [ "id", id ], [ "name", id ] ]);
	for (let i = 0; i < 50; i++) {
		const selected = (i == value) ? "selected" : "";
		create_element(select_element, "option",
			[ [ "value", i ], [ selected, selected ] ], `Scene ${i}`);
	}
	return select_element;
}

//
// color_yc用の入力要素を作成して返します。
//
function create_color_yc_element(args, td_element, id_name, color_yc, status) {
	if (status) {
		const style = "width: 3em;";
		create_text_element(args, td_element, "Y");
		create_textbox_element(args, td_element, `${id_name}-y`, color_yc.y, style);
		create_text_element(args, td_element, "Cb");
		create_textbox_element(args, td_element, `${id_name}-cb`, color_yc.cb, style);
		create_text_element(args, td_element, "Cr");
		create_textbox_element(args, td_element, `${id_name}-cr`, color_yc.cr, style);
	} else {
		create_element(td_element, "span", [ [ "class", "space"] ], "<未取得>");
	}
}

// 動画ファイル
table_check_creator[0] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// ループ再生
		create_checkbox_element(args, td_element, "loop",
			args.filter.check[0].name, args.filter.check[0].value);

		// アルファチャンネルを読み込む
		create_checkbox_element(args, td_element, "alpha",
			args.filter.check[1].name, args.filter.check[1].value);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// ファイル選択
		create_element(td_element, "br");
		create_file_element(args, td_element, "file", args.filter.check[0].name, ef.file);

		// ファイル名を追加します。
		create_element(td_element, "br");
		create_text_element(args, td_element, ef.file);
	}

	// 3行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		const style = "width: 6em;";

		// i1
		create_text_element(args, td_element, "i1");
		create_textbox_element(args, td_element, "i1", to_hex(ef.i1), style);

		// i2
		create_text_element(args, td_element, "i2");
		create_textbox_element(args, td_element, "i2", to_hex(ef.i2), style);

		// i3
		create_text_element(args, td_element, "i3");
		create_textbox_element(args, td_element, "i3", to_hex(ef.i3), style);
	}

	// 4行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// frame_n
		create_text_element(args, td_element, "frame_n");
		create_textbox_element(args, td_element, "frame_n", ef.frame_n);
		create_element(td_element, "br");

		// video_rate
		create_text_element(args, td_element, "video_rate");
		create_textbox_element(args, td_element, "video_rate", ef.video_rate);

		// video_scale
		create_text_element(args, td_element, "video_scale");
		create_textbox_element(args, td_element, "video_scale", ef.video_scale);
	}
};

// 画像ファイル
table_check_creator[1] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// ファイル選択
		create_element(td_element, "br");
		create_file_element(args, td_element, "file", args.filter.check[0].name, ef.file);

		// ファイル名を追加します。
		create_element(td_element, "br");
		create_text_element(args, td_element, ef.file);
	}
};

// 音声ファイル
table_check_creator[2] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// ループ再生
		create_checkbox_element(args, td_element, "loop",
			args.filter.check[0].name, args.filter.check[0].value);

		// 動画ファイルと連携
		create_checkbox_element(args, td_element, "link",
			args.filter.check[1].name, args.filter.check[1].value);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// ファイル選択
		create_element(td_element, "br");
		create_file_element(args, td_element, "file", args.filter.check[0].name, ef.file);

		// ファイル名を追加します。
		create_element(td_element, "br");
		create_text_element(args, td_element, ef.file);
	}

	// 3行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		const style = "width: 6em;";

		// i1
		create_text_element(args, td_element, "i1");
		create_textbox_element(args, td_element, "i1", to_hex(ef.i1), style);

		// i2
		create_text_element(args, td_element, "i2");
		create_textbox_element(args, td_element, "i2", to_hex(ef.i2), style);

		// i4
		create_text_element(args, td_element, "i4");
		create_textbox_element(args, td_element, "i4", to_hex(ef.i4), style);

		create_element(td_element, "br");

		// i5
		create_text_element(args, td_element, "i5");
		create_textbox_element(args, td_element, "i5", to_hex(ef.i5), style);

		// frame_n
		create_text_element(args, td_element, "frame_n");
		create_textbox_element(args, td_element, "frame_n", ef.frame_n);
	}
};

// テキスト
table_check_creator[3] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 自動スクロール
		create_checkbox_element(args, td_element, "auto-scroll",
			args.filter.check[2].name, args.filter.check[2].value);

		create_element(td_element, "br");

		// 文字毎に個別オブジェクト
		create_checkbox_element(args, td_element, "each",
			args.filter.check[0].name, args.filter.check[0].value);

		if (args.filter.check[0].value) {
			// 移動座標上に表示する
			create_checkbox_element(args, td_element, "on-move-pos",
				args.filter.check[1].name, args.filter.check[1].value);
		}

		create_element(td_element, "br");

		// オブジェクトの長さを自動調節
		create_checkbox_element(args, td_element, "autoadjust",
			"オブジェクトの長さを自動調節", ef.autoadjust);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 文字色
		create_text_element(args, td_element, "文字色");
		create_color_element(args, td_element, "color", ef.color);

		// 影・縁色
		create_text_element(args, td_element, "影・縁色");
		create_color_element(args, td_element, "color2", ef.color2);

		// 太文字
		create_checkbox_element(args, td_element, "bold", "太文字", args.filter.check[3].value);

		// 斜体
		create_checkbox_element(args, td_element, "italic", "斜体", args.filter.check[4].value);
	}

	// 3行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		const style = "width: 2em;";

		// 字間
		create_text_element(args, td_element, "字間");
		create_textbox_element(args, td_element, "spacing_x", ef.spacing_x, style);

		// 行間
		create_text_element(args, td_element, "行間");
		create_textbox_element(args, td_element, "spacing_y", ef.spacing_y, style);

		const types = [
			"標準文字",
			"影付き文字",
			"影付き文字(薄)",
			"縁取り文字",
			"縁取り文字(細)",
			"縁のみ",
			"縁のみ(細)" ];

		// タイプ
		create_select_element(args, td_element, "type", types, ef.type);

		const aligns = [
			"左寄せ[上]",
			"中央寄せ[上]",
			"右寄せ[上]",

			"左寄せ[中]",
			"中央寄せ[中]",
			"右寄せ[中]",

			"左寄せ[下]",
			"中央寄せ[下]",
			"右寄せ[下]",

			"縦書 上寄[右]",
			"縦書 中央[右]",
			"縦書 下寄[右]",

			"縦書 上寄[中]",
			"縦書 中央[中]",
			"縦書 下寄[中]",

			"縦書 上寄[左]",
			"縦書 中央[左]",
			"縦書 下寄[左]" ];

		// アライン
		create_select_element(args, td_element, "align", aligns, ef.align);
	}

	// 4行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 高精度モード
		create_checkbox_element(args, td_element, "precision", "高精度モード", ef.precision);

		// 滑らかにする
		create_checkbox_element(args, td_element, "soft", "滑らかにする", ef.soft);

		// 等間隔モード
		create_checkbox_element(args, td_element, "monospace", "等間隔モード", ef.monospace);
	}

	// 5行目
	{

		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		const style = "width: 24em;";

		// フォント
		create_textbox_element(args, td_element, "font", ef.font, style);
	}

	// 6行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		const style = "width: stretch;";

		// テキスト
		create_textarea_element(args, td_element, "text", ef.text, style);
	}
};

// 図形
table_check_creator[4] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 図形の種類
		create_text_element(args, td_element, "図形の種類");
		create_select_element(args, td_element, "type",
			args.filter.check[0].name, ef.is_file ? 7 : ef.type);

		if (ef.is_file) {
			// ファイル選択
			create_element(td_element, "br");
			create_file_element(args, td_element, "name", args.filter.check[0].name, ef.name);

			// ファイル名を追加します。
			create_element(td_element, "br");
			create_text_element(args, td_element, ef.name);
		}
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 図形の色
		create_text_element(args, td_element, "図形の色");
		create_color_element(args, td_element, "color", ef.color);
	}
};

// フレームバッファ
table_check_creator[5] = (args) => {
}

table_check_creator["音声波形表示"] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// タイプ
		create_text_element(args, td_element, "タイプ");
		create_select_element(args, td_element, "type", args.filter.check[0].name, ef.type);

		// 波形の色
		create_text_element(args, td_element, args.filter.check[1].name);
		create_color_element(args, td_element, "color", ef.color);

		create_element(td_element, "br");

		// 編集全体の音声を元にする
		create_checkbox_element(args, td_element, "all",
			args.filter.check[3].name, args.filter.check[3].value);

		create_element(td_element, "br");

		// ファイル選択
		create_file_element(args, td_element, "file", args.filter.check[2].name, ef.file);

		create_element(td_element, "br");

		// ファイル名を追加します。
		create_text_element(args, td_element, ef.file);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		const style = "width: 3em;";

		// 波形タイプ
		create_text_element(args, td_element, "波形タイプ");
		create_select_element(args, td_element, "mode", [ "モード0", "モード1" ], ef.mode);

		// mirror
		create_text_element(args, td_element, "ミラー");
		create_textbox_element(args, td_element, "mirror", ef.mirror, style);

		create_element(td_element, "br");

		// res_w
		create_text_element(args, td_element, "横解像度");
		create_textbox_element(args, td_element, "res_w", ef.res_w, style);

		// res_h
		create_text_element(args, td_element, "糧解像度");
		create_textbox_element(args, td_element, "res_h", ef.res_h, style);

		create_element(td_element, "br");

		// space_x
		create_text_element(args, td_element, "横スペース(%)");
		create_textbox_element(args, td_element, "space_x", ef.space_x, style);

		// space_y
		create_text_element(args, td_element, "縦スペース(%)");
		create_textbox_element(args, td_element, "space_y", ef.space_y, style);
	}

	// 3行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		const style = "width: 6em;";

		// i1
		create_text_element(args, td_element, "i1");
		create_textbox_element(args, td_element, "i1", to_hex(ef.i1), style);

		// i2
		create_text_element(args, td_element, "i2");
		create_textbox_element(args, td_element, "i2", to_hex(ef.i2), style);

		// i4
		create_text_element(args, td_element, "i4");
		create_textbox_element(args, td_element, "i4", to_hex(ef.i4), style);

		create_element(td_element, "br");

		// i5
		create_text_element(args, td_element, "i5");
		create_textbox_element(args, td_element, "i5", to_hex(ef.i5), style);

		// i10
		create_text_element(args, td_element, "i10");
		create_textbox_element(args, td_element, "i10", to_hex(ef.i10), style);

		// frame_n
		create_text_element(args, td_element, "frame_n");
		create_textbox_element(args, td_element, "frame_n", ef.frame_n);
	}
};

// シーン
table_check_creator[7] = (args) => {
}

// シーン(音声)
table_check_creator[8] = (args) => {
}

// 直前オブジェクト
table_check_creator[9] = (args) => {
}

// 標準描画
table_check_creator[10] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 合成モード
		create_text_element(args, td_element, "合成モード");
		create_select_element(args, td_element, "blend", args.filter.check[0].name, ef.blend);
	}
};

// 拡張描画
table_check_creator[11] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 合成モード
		create_text_element(args, td_element, "合成モード");
		create_select_element(args, td_element, "blend",
			args.filter.check[0].name, ef.blend);

		// 裏面を表示しない
		create_checkbox_element(args, td_element, "culling",
			args.filter.check[1].name, args.filter.check[1].value);
	}
};

// 標準再生
table_check_creator[12] = (args) => {
}

// パーティクル出力
table_check_creator[13] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 合成モード
		create_text_element(args, td_element, "合成モード");
		create_select_element(args, td_element, "blend", args.filter.check[3].name, ef.blend);

		create_element(td_element, "br");

		// 出力方向の基準を移動方向にする
		create_checkbox_element(args, td_element, "use-move-direction",
			args.filter.check[0].name, args.filter.check[0].value);

		// 移動範囲の座標からランダムに出力
		create_checkbox_element(args, td_element, "use-move-range",
			args.filter.check[1].name, args.filter.check[1].value);

		create_element(td_element, "br");

		// 3Dランダム回転
		create_checkbox_element(args, td_element, "use-3d",
			args.filter.check[2].name, args.filter.check[2].value);

		// 終了点で全て消えるように調節する
		create_checkbox_element(args, td_element, "sync-end",
			args.filter.check[4].name, args.filter.check[4].value);
	}
};

// シーンチェンジ
table_check_creator[14] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	function get_check0_name() {
		switch (ef.name) {
			case "ドア": return "手前に開く";
			case "リール回転": return "横回転";
		}
	}

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 反転
		create_checkbox_element(args, td_element, "invert",
			args.filter.check[0].name, args.filter.check[0].value);

		// タイプ
		create_text_element(args, td_element, "タイプ");
		create_select_element(args, td_element, "type",
			args.filter.check[2].name, ef.type, ef.name);

		const check0_name = get_check0_name();
		if (check0_name) {
			// 追加オプション
			create_checkbox_element(args, td_element, "check0",
				check0_name, args.filter.check[1].value);
		}
	}
/*
	// 本来ならこのように分岐してef.paramを解析する必要があります。
	switch (ef.name) {
		case "図形ワイプ": {
			break;
		}
		case "図形で隠す": {
			break;
		}
		case "図形で隠す(放射)": {
			break;
		}
		case "砕け散る": {
			break;
		}
	}
*/
	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// filter
		create_text_element(args, td_element, "filter");
		create_textbox_element(args, td_element, "filter", ef.filter);

		// param
		create_text_element(args, td_element, "param");
		create_textbox_element(args, td_element, "param", ef.param);
	}
};

// 色調補正
// 色調補正(エフェクト)
table_check_creator[15] =
table_check_creator[16] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 飽和する
		create_checkbox_element(args, td_element, "saturate",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// クリッピング
table_check_creator[17] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 中心の位置を変更
		create_checkbox_element(args, td_element, "change_center_pos",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// ぼかし
table_check_creator[18] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// サイズ固定
		create_checkbox_element(args, td_element, "fixed_size",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// 境界ぼかし
table_check_creator[19] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 透明度の境界をぼかす
		create_checkbox_element(args, td_element, "with_alpha",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// ぼかし(エフェクト)
table_check_creator[20] = (args) => {
};

// モザイク
table_check_creator[21] =
table_check_creator[22] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// タイル風
		create_checkbox_element(args, td_element, "tile",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// 発光
table_check_creator[23] =
table_check_creator[24] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 光色
		create_text_element(args, td_element, "光色");
		create_color_element(args, td_element, "color", ef.color);

		// サイズ固定
		create_checkbox_element(args, td_element, "fixed_size",
			args.filter.check[1].name, args.filter.check[1].value);
	}
};

// 閃光
table_check_creator[25] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// モード
		create_text_element(args, td_element, "モード");
		create_select_element(args, td_element, "mode", args.filter.check[0].name, ef.mode);

		// 光色
		create_text_element(args, td_element, "光色");
		create_color_element(args, td_element, "color", ef.color);

		// サイズ固定
		create_checkbox_element(args, td_element, "fixed_size",
			args.filter.check[2].name, args.filter.check[2].value);
	}
};

// 拡散光
table_check_creator[26] =
table_check_creator[27] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// サイズ固定
		create_checkbox_element(args, td_element, "fixed_size",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// グロー
table_check_creator[28] =
table_check_creator[29] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 形状
		create_text_element(args, td_element, "形状");
		create_select_element(args, td_element, "type", args.filter.check[0].name, ef.type);

		// 光色
		create_text_element(args, td_element, "光色");
		create_color_element(args, td_element, "color", ef.color);

		// 光成分のみ
		create_checkbox_element(args, td_element, "light_only",
			args.filter.check[2].name, args.filter.check[2].value);
	}
};

// クロマキー
table_check_creator[30] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 色彩補正
		create_checkbox_element(args, td_element, "color-correct",
			args.filter.check[0].name, args.filter.check[0].value);

		if (args.filter.check[0].value) {
			// 透過補正
			create_checkbox_element(args, td_element, "alpha-correct",
				args.filter.check[1].name, args.filter.check[1].value);
		}

		create_element(td_element, "br");

		// キー色
		create_color_yc_element(args, td_element, "color", ef.color, ef.status);
	}
};

// カラーキー
table_check_creator[31] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// キー色
		create_color_yc_element(args, td_element, "color", ef.color, ef.status);
	}
};

// ルミナンスキー
table_check_creator[32] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// タイプ
		create_text_element(args, td_element, "タイプ");
		create_select_element(args, td_element, "type", args.filter.check[0].name, ef.type);
	}
};

// ライト
table_check_creator[33] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 逆光
		create_checkbox_element(args, td_element, "backlight",
			args.filter.check[0].name, args.filter.check[0].value);

		create_element(td_element, "br");

		// 光色
		create_text_element(args, td_element, "光色");
		create_color_element(args, td_element, "color", ef.color);
	}
};

// シャドー
table_check_creator[34] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 影を別オブジェクトで描画
		create_checkbox_element(args, td_element, "apart",
			args.filter.check[0].name, args.filter.check[0].value);

		create_element(td_element, "br");

		// 影色
		create_text_element(args, td_element, "影色");
		create_color_element(args, td_element, "color", ef.color);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// パターン画像ファイル
		create_file_element(args, td_element, "file", args.filter.check[2].name, ef.file);
		create_element(td_element, "br");
		create_text_element(args, td_element, ef.file);
	}
};

// 縁取り
table_check_creator[35] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 縁色
		create_text_element(args, td_element, "縁色");
		create_color_element(args, td_element, "color", ef.color);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// パターン画像ファイル
		create_file_element(args, td_element, "file", args.filter.check[1].name, ef.file);
		create_element(td_element, "br");
		create_text_element(args, td_element, ef.file);
	}
};

// 凸エッジ
table_check_creator[36] = (args) => {
};

// エッジ抽出
table_check_creator[37] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 輝度エッジを抽出
		create_checkbox_element(args, td_element, "brightness",
			args.filter.check[0].name, args.filter.check[0].value);

		// 透明度エッジを抽出
		create_checkbox_element(args, td_element, "alpha",
			args.filter.check[1].name, args.filter.check[1].value);

		create_element(td_element, "br");

		// 縁色
		create_text_element(args, td_element, "縁色");
		create_color_element(args, td_element, "color", ef.color);
	}
};

// シャープ
table_check_creator[38] = (args) => {
};

// フェード
table_check_creator[39] = (args) => {
};

// ワイプ
table_check_creator[40] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 反転(イン)
		create_checkbox_element(args, td_element, "invert-in",
			args.filter.check[0].name, args.filter.check[0].value);

		// 反転(アウト)
		create_checkbox_element(args, td_element, "invert-out",
			args.filter.check[1].name, args.filter.check[1].value);

		create_element(td_element, "br");

		// タイプ
		create_text_element(args, td_element, "タイプ");
		create_select_element(args, td_element, "type", args.filter.check[2].name, ef.type);
	}
};

// マスク
table_check_creator[41] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		let type = ef.type;
		switch (ef.name.charAt(0)) {
			case "*": type = 7; break;
			case ":": type = 8; break;
		}

		// マスクの種類
		create_text_element(args, td_element, "マスクの種類");
		create_select_element(args, td_element, "type", args.filter.check[0].name, type);

		switch (type) {
			case 7: {

				create_element(td_element, "br");

				// ファイル選択
				create_file_element(args, td_element, "file", ef.name.substring(1));
				create_element(td_element, "br");
				create_text_element(args, td_element, ef.name);

				break;
			}
			case 8: {

				create_element(td_element, "br");

				// シーン選択
				create_scene_select_element(args, td_element, "scene", ef.name.substring(1));

				break;
			}
		}
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// マスクの反転
		create_checkbox_element(args, td_element, "invert",
			args.filter.check[1].name, args.filter.check[1].value);

		// 元のサイズに合わせる
		create_checkbox_element(args, td_element, "orig-size",
			args.filter.check[2].name, args.filter.check[2].value);
	}
};

// 斜めクリッピング
table_check_creator[42] = (args) => {
};

// 放射ブラー
table_check_creator[43] =
table_check_creator[44] = (args) => {
};

// 方向ブラー
table_check_creator[45] =
table_check_creator[46] = (args) => {
};

// レンズブラー
table_check_creator[47] =
table_check_creator[48] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// サイズ固定
		create_checkbox_element(args, td_element, "fixed_size",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// モーションブラー
table_check_creator[49] =
table_check_creator[50] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 残像
		create_checkbox_element(args, td_element, "afterimage",
			args.filter.check[0].name, args.filter.check[0].value);

		// オフスクリーン描画
		create_checkbox_element(args, td_element, "offscreen",
			args.filter.check[1].name, args.filter.check[1].value);

		// 出力時に分解能を上げる
		create_checkbox_element(args, td_element, "high-output",
			args.filter.check[2].name, args.filter.check[2].value);
	}
};

// 座標
table_check_creator[51] = (args) => {
};

// 拡大率
table_check_creator[52] = (args) => {
};

// 透明度
table_check_creator[53] = (args) => {
};

// 回転
table_check_creator[54] = (args) => {
};

// 領域拡張
table_check_creator[55] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 塗りつぶし
		create_checkbox_element(args, td_element, "fill",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// リサイズ
table_check_creator[56] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 補間なし
		create_checkbox_element(args, td_element, "no_interpolation",
			args.filter.check[0].name, args.filter.check[0].value);

		// ドット数でサイズ指定
		create_checkbox_element(args, td_element, "dot-size",
			args.filter.check[1].name, args.filter.check[1].value);
	}
};

// ローテーション
table_check_creator[57] = (args) => {
};

// 振動
table_check_creator[58] =
table_check_creator[59] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// ランダムに強さを変える
		create_checkbox_element(args, td_element, "random",
			args.filter.check[0].name, args.filter.check[0].value);

		// 複雑に振動
		create_checkbox_element(args, td_element, "complex",
			args.filter.check[1].name, args.filter.check[1].value);
	}
};

// 反転
table_check_creator[60] =
table_check_creator[61] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 上下反転
		create_checkbox_element(args, td_element, "vert",
			args.filter.check[0].name, args.filter.check[0].value);

		// 左右反転
		create_checkbox_element(args, td_element, "horz",
			args.filter.check[1].name, args.filter.check[1].value);

		// 輝度反転
		create_checkbox_element(args, td_element, "lightness",
			args.filter.check[2].name, args.filter.check[2].value);

		// 色相反転
		create_checkbox_element(args, td_element, "hue",
			args.filter.check[3].name, args.filter.check[3].value);

		// 透明度反転
		create_checkbox_element(args, td_element, "alpha",
			args.filter.check[4].name, args.filter.check[4].value);
	}
};

// ミラー
table_check_creator[62] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// ミラーの方向
		create_text_element(args, td_element, "ミラーの方向");
		create_select_element(args, td_element, "direction", args.filter.check[0].name, ef.type);

		// 中心の位置を変更
		create_checkbox_element(args, td_element, "change-center",
			args.filter.check[1].name, args.filter.check[1].value);
	}
};

// ラスター
table_check_creator[63] =
table_check_creator[64] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 縦ラスター
		create_checkbox_element(args, td_element, "vert",
			args.filter.check[0].name, args.filter.check[0].value);

		// ランダム振幅
		create_checkbox_element(args, td_element, "random",
			args.filter.check[1].name, args.filter.check[1].value);
	}
};

// 波紋
table_check_creator[65] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);
		const style = "width: 3em;";

		// 波紋数
		create_text_element(args, td_element, "波紋数");
		create_textbox_element(args, td_element, "add", ef.add, style);

		// 波紋間隔
		create_text_element(args, td_element, "波紋間隔");
		create_textbox_element(args, td_element, "interval", ef.interval, style);

		// 増幅減衰回数
		create_text_element(args, td_element, "増幅減衰回数");
		create_textbox_element(args, td_element, "num", ef.num, style);
	}
};

// 画像ループ
table_check_creator[66] =
table_check_creator[67] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 個別オブジェクト
		create_checkbox_element(args, td_element, "each",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// 極座標変換
table_check_creator[68] = (args) => {
};

// ディスプレイスメントマップ
table_check_creator[69] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		let type = ef.type;
		switch (ef.name.charAt(0)) {
			case "*": type = 7; break;
			case ":": type = 8; break;
		}

		// マップの種類
		create_text_element(args, td_element, "マップの種類");
		create_select_element(args, td_element, "type", args.filter.check[0].name, type);

		switch (type) {
			case 7: {

				create_element(td_element, "br");

				// ファイル選択
				create_file_element(args, td_element, "file", ef.name.substring(1));
				create_element(td_element, "br");
				create_text_element(args, td_element, ef.name);

				break;
			}
			case 8: {

				create_element(td_element, "br");

				// シーン選択
				create_scene_select_element(args, td_element, "scene", ef.name.substring(1));

				break;
			}
		}
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 変形方法
		create_text_element(args, td_element, "変形方法");
		create_select_element(args, td_element, "calc", args.filter.check[1].name, ef.calc);

		// 元のサイズに合わせる
		create_checkbox_element(args, td_element, "orig-size",
			args.filter.check[2].name, args.filter.check[2].value);
	}
};

// ノイズ
table_check_creator[70] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 合成モード
		create_text_element(args, td_element, "合成モード");
		create_select_element(args, td_element, "mode", args.filter.check[0].name, ef.mode);

		// ノイズの種類
		create_text_element(args, td_element, "ノイズの種類");
		create_select_element(args, td_element, "type", args.filter.check[1].name, ef.type);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);
		const style = "width: 3em;";

		// シード
		create_text_element(args, td_element, "シード");
		create_textbox_element(args, td_element, "seed", ef.seed, style);
	}
};

// 色ずれ
table_check_creator[71] =
table_check_creator[72] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 色ずれの種類
		create_text_element(args, td_element, "色ずれの種類");
		create_select_element(args, td_element, "type", args.filter.check[0].name, ef.type);
	}
};

// 単色化
table_check_creator[73] =
table_check_creator[74] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 色の設定
		create_text_element(args, td_element, args.filter.check[0].name);
		create_color_element(args, td_element, "color", ef.color);

		// 輝度を保持する
		create_checkbox_element(args, td_element, "keep-luminance",
			args.filter.check[1].name, args.filter.check[1].value);
	}
};

// グラデーション
table_check_creator[75] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 合成モード
		create_text_element(args, td_element, "合成モード");
		create_select_element(args, td_element, "blend", args.filter.check[0].name, ef.blend);

		// グラデーションの形状
		create_text_element(args, td_element, "グラデーションの形状");
		create_select_element(args, td_element, "type", args.filter.check[1].name, ef.type);
	}

	// 2行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 開始色
		create_text_element(args, td_element, args.filter.check[2].name);
		if (ef.no_color) {
			create_element(td_element, "span", [ [ "class", "space"] ], "<未取得>");
		} else {
			create_color_element(args, td_element, "color", ef.color);
		}

		// 終了色
		create_text_element(args, td_element, args.filter.check[3].name);
		if (ef.no_color2) {
			create_element(td_element, "span", [ [ "class", "space"] ], "<未取得>");
		} else {
			create_color_element(args, td_element, "color2", ef.color2);
		}
	}
};

// 拡張色設定
table_check_creator[76] =
table_check_creator[77] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// RGB⇔HSV
		create_checkbox_element(args, td_element, "rgb-hsv",
			args.filter.check[0].name, args.filter.check[0].value);
	}
};

// 特定色域変換
table_check_creator[78] = (args) => {

	const ef = args.exdata.exdata_ptr;
	let td_index = 0;

	// 1行目
	{
		// td要素を作成します。
		const td_element = create_td_element(args, td_index++);

		// 変換前の色
		create_element(td_element, "span", [ [ "class", "space"] ], "変換前の色");
		create_color_yc_element(args, td_element, "color_yc", ef.color_yc, ef.color_yc.status);

		create_element(td_element, "br");

		// 変換後の色
		create_element(td_element, "span", [ [ "class", "space"] ], "変換後の色");
		create_color_yc_element(args, td_element, "color_yc2", ef.color_yc2, ef.color_yc2.status);
	}
};

// クリッピング＆リサイズ(エフェクト)
table_check_creator[103] = (args) => {
};
