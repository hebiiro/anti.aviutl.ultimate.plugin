﻿# 🎉『フォントプレビュー』アドイン

* フォントのプレビューを確認しながら選択できるようにします。

## 💡使い方

1. 拡張編集でテキストオブジェクトを作成し、選択します。
1. `設定ダイアログ`でフォントのコンボボックスをクリックします。
1. ドロップダウンリストがフォントのプレビューになっているはずなのでクリックして選択します。

### 💡フォント名をコピーする

1. `設定ダイアログ`でフォントのコンボボックスを右クリックします。
1. コンテキストメニューが表示されます。
1. `フォント名をコピー`を選択します。

## 🔧設定方法

1. `aviutlウィンドウ`のメニューで`編集`➡`アルティメットプラグイン`➡`フォントプレビュー`を選択します。
1. `フォントプレビュー`ウィンドウが表示されます。

### 📝設定項目

* `サンプルテキスト(複数行)` ✏️プレビュー用のサンプルテキストを複数行で指定します。
* `アイテムの高さ` ✏️リストボックス内のアイテムの高さ(一行の高さ)を指定します。
* `フォントの高さ` ✏️プレビュー用フォントの高さを指定します。
* `ウィンドウのサイズ` ✏️リストボックス全体のサイズを指定します。
ただし、リストボックスがモニター外に出ないように自動的に調整される場合があります。
* `単一行にまとめる` ✏️フォント名とサンプルテキストをまとめて描画します。
* `描画モード` ✏️プレビューの描画モードを指定します。
	* `自動` ✏️`ダークモード化`アドインが存在するか自動で判別し、存在する場合はテーマで描画します。
	* `システムカラー` ✏️システムカラーで描画します。
	* `テーマ` ✏️`自動`と同じです。`ダークモード化`アドインが存在する場合はテーマで描画します。
		それ以外の場合はシステムカラーで描画します。
	* `カスタムカラー` ✏️ユーザー指定のカスタムカラーで描画します。
* `通常` ✏️通常状態のアイテムの設定です。
* `選択` ✏️選択状態のアイテムの設定です。
	* `背景` ✏️背景色を指定します。`描画モード`で`カスタムカラー`を選択しているときのみ有効です。
	* `フォント名` ✏️フォント名の色を指定します。`描画モード`で`カスタムカラー`を選択しているときのみ有効です。
	* `プレビュー` ✏️プレビューの色を指定します。`描画モード`で`カスタムカラー`を選択しているときのみ有効です。
* `コンテキストメニューを使用する` ✏️フォントのコンボボックスを右クリックしたときコンテキストメニューを表示するかどうかを指定します。
	* `Shiftキーが必要`
	* `Ctrlキーが必要`
	* `Altキーが必要`
	* `Winキーが必要`
		✏️各キーを押しながら右クリックしたときだけコンテキストメニューを表示するようにします。
