//
// 数値を16進数文字列に変換して返します。
//
function to_hex(value) {
	function to_hex_short(value) {
		return ("000" + (value & 0xFFFF).toString(16).toUpperCase()).substr(-4);
	}
	return "0x" + to_hex_short(value >> 16) + to_hex_short(value);
}

//
// 要素を作成して返します。
//
function create_element(parent_element, tag_name, attrs, value) {
	const element = document.createElement(tag_name);
	if (parent_element) parent_element.appendChild(element);
	if (attrs) {
		for (let attr of attrs) {
			if (attr[0]) element.setAttribute(attr[0], attr[1]);
		}
	}
	if (value != null) element.append(value);
	return element;
}

//
// 指定されたスクリプトをインポートします。
//
function import_script(src) {
	const script = document.createElement("script");
	script.src = src;
	document.head.appendChild(script);
}
