//
// 初期化処理を実行します。
//
window.addEventListener("load", event => {

	// ウェブメッセージのリスナーを追加します。
	window.chrome.webview.addEventListener("message", event => {
		build_contents(event.data["response_settings"]);
	});

	// セッティングをリクエストします。
	window.chrome.webview.postMessage("request_settings");
});
