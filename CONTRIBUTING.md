# アルティメットプラグインの開発者向けドキュメント

## 開発環境

* Windows 11 Home
* Visual Studio Community 2022

## プロジェクトの設定

* x86
* UNICODE文字セット
* `$(UserProfile)\source\repos\ultimate` ソリューションフォルダ
* `$(UserProfile)\source\packages` ライブラリ用フォルダ
* `$(UserProfile)\source\pack\ultimate` リリース用フォルダ
* `$(UserProfile)\source\test\aviutl` テスト用フォルダ

## 使用ライブラリ

* aviutl_exedit_sdk `$(UserProfile)\source\packages\aviutl_exedit_sdk`
* detours `$(UserProfile)\source\packages\detours.4.0.1`
* boost `$(UserProfile)\source\packages\boost.1.84.0`
* glad `$(UserProfile)\source\packages\glad.2.0.4`
* NanoVG `$(UserProfile)\source\packages\nanovg.rgb2hsv`

## ライブラリ

* `my` アルティメットプラグイン以外のライブラリ。
* `core` アルティメットプラグインのライブラリ。
* `share` アルティメットアドインのライブラリ。

## コーディングパターン

* スネークケース。
* 定数には`c_`を付ける。
* `class`、`private`、`protected`、`public`は可能な限り使用しない。
* `bool`、`true`、`false`はなるべく使用しない。
* コンストラクタ、デストラクタ、例外処理はなるべく使用しない。
* 初期化と後始末が必要なクラスにはinit()とexit()を定義する。
* `case`の本体が複数行なら`{}`で囲む。
* `{`はなるべく次の行に書く。
* `/**/`は使用しない。
* クラス、関数、変数のコメントは次のように書く。ただし、`この関数は`と`この変数は`は省略する。(数が多すぎるので)
```
	//
	// このクラスは〇〇です。
	//
```
* 処理に対するコメントは次のように書く。
```
	// メッセージボックスを表示します。
	::MessageBoxW(nullptr, L"hoge", L"foo", MB_OK | MB_ICONWARNING);
```
* ソースファイルの拡張子は`cpp`にする。
* ヘッダーファイルの拡張子は`hpp`にする。
* プロジェクトには必ず`main.def`、`main.cpp`を作成する。
* ソースファイルは可能な限り`main.cpp`のみにする。
* ヘッダーファイルでは他のファイルをインクルードしない。
* `main.cpp`で必要なすべてのヘッダーファイルをインクルードする。

## プロジェクトの主な構造

* `Hive` データを保持する。プロジェクト全体から参照される。
* `HookManager` フックを管理する。
* `AddinWindow` AviUtlプラグインウィンドウのように動作する。
* `AddinDialog` アドインウィンドウにコントロールを配置するための子ウィンドウ。
* `ConfigIO` プロジェクト全体の設定を読み込み・保存する。ほぼプロジェクト全体を参照する。
* `App` インターフェイスを実装し、他のクラスから相互参照される。
* `Addin` プロジェクト全体を参照する。他のクラスからは参照されない。
* 依存関係は次のようになることが多い。`Addin`->`App`->`ConfigIO`->`その他のクラス`->`Hive`

## 設定ファイル

* ファイルなしが初期値。
* なるべく`JSON`を使用する。
