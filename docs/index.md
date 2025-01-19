---
layout: default
title: アルティメットプラグインのお部屋
description: ここはアルティメットプラグインの説明所です。
lang: ja_JP
---

# 🎉アルティメットプラグイン

* [aviutl](https://spring-fragrance.mints.ne.jp/aviutl/)のプラグインです。

## ⚠事前準備

### `r39以前からバージョンアップする場合`

1. 予め`aviutlフォルダ`内の`plugins/ultimate/assets/dark`フォルダを削除しておいてください。
	* またはzip内の`install.exe`を使用して旧バージョンをアンインストールしてください。

## 🚀導入方法

1. [アルティメットプラグインをダウンロード](https://github.com/hebiiro/anti.aviutl.ultimate.plugin/releases/latest/)します。
1. zip内の`plugins`フォルダを`aviutlフォルダ`内にコピーします。
	* またはzip内の`install.exe`を使用して新バージョンをインストールします。[install.exeの使い方はこちら](./contents/utils/install.html)
1. [`python実行環境`をダウンロード](https://github.com/hebiiro/anti.aviutl.ultimate.plugin/releases/tag/r36)します。
1. `python実行環境`のzip内の`plugins`フォルダも`aviutlフォルダ`内にコピーします。

## 🔖更新履歴

* [更新履歴のページ](./contents/history.html)を参照してください。

## 🔧設定方法

* 基本的には`config`フォルダ内のファイルをテキストエディタで編集します。
* アドインによってはGUIで設定できる場合もあります。
* アドインによっては`config`フォルダ内にコンフィグファイルが存在しない場合は、 
`assets/default`フォルダ内のコンフィグファイルを読み込む場合があります。
* aviutlが起動しないなどの問題が発生する場合は`config`フォルダ内の`jsonファイル`を削除して設定をリセットしてください。

### 📝ultimate.auf.json

1. aviutlを一旦終了します。
1. `plugins\ultimate\ui\アルティメットプラグインの設定.html_local`ファイルを実行します。
	* `.html_local`ファイルは[『ローカルウェブアプリ』](./contents/utils/local_web_app.html)で実行する必要があります。
1. もしくは`config/ultimate.auf.json`をテキストエディタで編集します。
	* ファイルが存在しない場合は`config/default`フォルダにあるものを`config`フォルダにコピーしてから編集してください。
	* `python_file_name` ✏️python312.dllのパスを指定します。
		* ✏️デフォルトでは`ultimate.auf`からの相対パスで`ultimate/assets/python32/python312.dll`を読み込むようになっています。
		* ✏️システムに存在するPython(32bit版)を使用する場合はこの値を空文字列に書き換えてください。
	* `addin`
		* `active`
			* ✏️`true`にするとそのアドインが読み込まれます。
			* ✏️`false`にするとそのアドインは読み込まれません。
		* `name`
			* ✏️アドインの名前です。
		* `args`
			* ✏️アドインに与える引数です。
			* ✏️主に開発者用のオプションです。
			* ✏️デバッグバージョンで`debug`を与えるとデバッグトレースが有効になりますが、リリースバージョンでは何の影響ありません。
1. aviutlを起動します。

## 🚀ユーティリティの一覧

| 内部名 | 一般名 | 機能 |
|:---|:---|:---|
| [install](./contents/utils/install.html) | インストーラ | アルティメットプラグインをインストールします。 |
| [local_web_app](./contents/utils/local_web_app.html) | ローカルウェブアプリ | `.local_html`ファイルを開くのに使用します。 |

## 🚀アドインの一覧

| 内部名 | 一般名 | 機能 |
|:---|:---|:---|
| [dark](./contents/addin/dark.html) | ダークモード化 | aviutlをダークモード化します。 |
| [workspace](./contents/addin/workspace.html) | ワークスペース化 | aviutlをワークスペース化します。 |
| [filer](./contents/addin/filer.html) | ファイラ | aviutl内でエクスプローラを表示します。 |
| [reboot](./contents/addin/reboot.html) | リブート | aviutlを再起動するなどの特殊なコマンドを実行します。 |
| [item_copy](./contents/addin/item_copy.html) | アイテムコピー | タイムラインの選択アイテムをexoファイルに保存します。 |
| [item_wave](./contents/addin/item_wave.html) | アイテム波形 | タイムラインアイテム内に音声波形を表示します。 |
| [settings_browser](./contents/addin/settings_browser.html) | 設定ブラウザ | 選択アイテムの設定をブラウザで表示します。 |
|---|---|---|
| [filter_copy](./contents/addin/filter_copy.html) | フィルタコピー | 設定ダイアログのフィルタをコピペできるようにします。 |
| [filter_drag](./contents/addin/filter_drag.html) | フィルタドラッグ | 設定ダイアログのフィルタをドラッグで移動できるようにします。 |
| [dirty_check](./contents/addin/dirty_check.html) | 終了確認 | 終了する前にプロジェクトの変更を確認します。 |
| [output_check](./contents/addin/output_check.html) | 出力確認 | 動画を出力する前に設定を確認します。 |
| [text_drop](./contents/addin/text_drop.html) | テキストドロップ | ドラッグアンドドロップでテキストオブジェクトを作成します。 |
| [text_split](./contents/addin/text_split.html) | テキスト分解 | テキストオブジェクトを文字毎に別々のオブジェクトに分解します。 |
| [item_align](./contents/addin/item_align.html) | アイテム整列 | タイムラインのアイテムを整列します。 |
| [image_export](./contents/addin/image_export.html) | 画像エクスポート | 現在のフレームまたは選択アイテムを画像としてエクスポートします。 |
| [scene_select](./contents/addin/scene_select.html) | シーン選択 | シーンをボタンで選択できるようにします。 |
| [ease_select](./contents/addin/ease_select.html) | イージング選択 | イージングをGUIで選択できるようにします。 |
| [zoom_select](./contents/addin/zoom_select.html) | ズーム選択 | タイムラインのズーム値をボタンで選択できるようにします。 |
| [font_tree](./contents/addin/font_tree.html) | フォントツリー | フォントをツリーコントロールから選択できるようにします。 |
| [font_preview](./contents/addin/font_preview.html) | フォントプレビュー | フォントのプレビューを確認しながら選択できるようにします。 |
| [font_select](./contents/addin/font_select.html) | フォント選択 | カスタムメニューからフォントを選択できるようにします。 |
| [note](./contents/addin/note.html) | メモ帳 | aviutl内にリッチエディットコントロールを表示します。 |
| [clipboard_viewer](./contents/addin/clipboard_viewer.html) | クリップボード画像表示 | クリップボードにコピーされた画像を表示します。 |
| [one_for_text](./contents/addin/one_for_text.html) | テキスト一括編集 | テキストオブジェクトのテキストを一括編集できるようにします。 |
|---|---|---|
| [namecage](./contents/addin/namecage.html) | アイテム名常時表示 | アイテム名がタイムラインから出ないようにします。 |
| [last_frame](./contents/addin/last_frame.html) | 最終フレーム自動調整 | 最終フレームを自動的に調整します。 |
| [color_code](./contents/addin/color_code.html) | カラーコード追加 | 「色の選択」ダイアログにカラーコードを追加します。 |
| [editbox_tweaker](./contents/addin/editbox_tweaker.html) | エディットボックス微調整 | エディットボックスの挙動を微調整します。 |
| [exedit_tweaker](./contents/addin/exedit_tweaker.html) | 拡張編集微調整 | 拡張編集の挙動を微調整します。 |
| [dialog_position](./contents/addin/dialog_position.html) | ダイアログ位置調整 | ダイアログがモニター外に表示されないように調整します。 |
| [dialog_size](./contents/addin/dialog_size.html) | ダイアログサイズ調整 | DPIに合わせてダイアログのサイズを調整します。 |
| [optima](./contents/addin/optima.html) | 最適化 | aviutl等の一部の処理を最適化します。 |

## 🎞️紹介動画

<script type="application/javascript" src="https://embed.nicovideo.jp/watch/sm44120795/script"></script>
<noscript><a href="https://www.nicovideo.jp/watch/sm44120795">AviUtlプラグイン/アルティメットプラグイン/大改造!!究極ビフォーアフター</a></noscript>

<script type="application/javascript" src="https://embed.nicovideo.jp/watch/sm44124352/script"></script>
<noscript><a href="https://www.nicovideo.jp/watch/sm44124352">AviUtlプラグイン/アルティメットプラグイン/補足説明</a></noscript>
