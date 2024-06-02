# 🎉 アルティメットプラグイン

* [AviUtl](http://spring-fragrance.mints.ne.jp/aviutl/)のプラグインです。
* プラグイン数上限の問題を解決するために、既存の複数のプラグインの機能を一つにまとめています。

## ⬇️ ダウンロード

* [最新バージョンはこちら](https://github.com/hebiiro/anti.aviutl.ultimate.plugin/releases/latest/)

## 🚀 導入方法

1. r22以前のバージョンのファイルとフォルダをすべて削除してください。
1. 以下のファイルをaviutlフォルダに入れてください。
	<br>**※pluginsフォルダに入れても正常に動作しないので注意してください。**
	* ultimate.auf
	* ultimate.aul
	* ultimate(フォルダ)
1. エクスプローラで`assets/deploy_python.ps1`を右クリックして『PowerShellで実行』を選択します。
	1. これにより、`assets/python32`にPython環境が構築されます。
	1. 実行してもウィンドウがすぐに閉じてしまう場合は、『プロパティ』の中の『許可する』にチェックを入れて再度試してください。

## 💡 使い方

[wiki](https://github.com/hebiiro/anti.aviutl.ultimate.plugin/wiki/)を参照してください。

## 🔖 更新履歴

* r29 - 2024/06/02 ✨ [workspace] タブでレイアウトを切り替えられる機能を追加
* r28 - 2024/06/01 🩹 [dark] 外部プロセスでPythonをロードしていなかった問題に対応
* r27 - 2024/05/30 ✨ [dialog_size] ダイアログのレイアウトを変更する機能を追加
* r26 - 2024/05/29 🩹 [dark] 日本語を含むファイルパスを正しく処理できない問題に対応
* r25 - 2024/05/28 ⚡️ [dark] rigayaさんの出力プラグインに対応
* r24 - 2024/05/27 🩹 [ease_select] コンフィグファイルが存在しない場合に発生していた問題に対応
* r23 - 2024/05/27 ⚡️ Pythonの読み込み先を選択できる機能を追加
* r22 - 2024/05/25 💥 命名規則をスネークケースに変更
* r21 - 2024/02/18 [SingleOut]予め指定されているファイルに保存できるようにしました
* r20 - 2024/02/18 Nestのバグフィックス
* r19 - 2024/02/12 [Nest]キャプションの表示状態を切り替えられるようにしました
* r18 - 2024/01/29 [FontPreview][FontSelect]フォント名をプレビューにする機能を追加しました
* r17 - 2024/01/19 AudioGraphを追加しました
* r16 - 2024/01/05 ZoomSelectを追加しました
* r15 - 2023/12/31 FontSelectを追加しました
* r14 - 2023/12/30 FontPreviewを追加しました
* r13 - 2023/12/15 DarkとNestのバグフィックス
* r12 - 2023/12/13 EasingSelectを追加しました
* r11 - 2023/12/09 SceneSelectを追加しました
* r10 - 2023/12/08 [Nest]フィルタウィンドウがAviUtlの表示状態と同じになるように修正しました
* r9 - 2023/12/04 DirtyCheckを追加
* r8 - 2023/11/27 [Nest]サイズ変更をAPIフックではなくウィンドウメッセージで処理するように変更
* r7 - 2023/11/10 Darkをリファクタリング
* r6 - 2023/09/26 SingleOut・TextSplit・ItemAlignを追加
* r5 - 2023/09/17 ColorCode・DialogPosition・LastFrame・Namecage・EncodeGuardを追加
* r4 - 2023/09/16 ダブルクリックでペインを最大化できる機能を追加
* r3 - 2023/09/16 PSDToolKitをドッキングしているときフリーズする問題を緩和
* r2 - 2023/09/16 Darkを追加
* r1 - 2023/09/14 初版

## ⚗️ 動作確認

* (必須) AviUtl 1.10 & 拡張編集 0.92 http://spring-fragrance.mints.ne.jp/aviutl/
* (共存確認) patch.aul r43 謎さうなフォーク版58 https://scrapbox.io/nazosauna/patch.aul

## 💳 クレジット

* Microsoft Research Detours Package https://github.com/microsoft/Detours
* aviutl_exedit_sdk https://github.com/ePi5131/aviutl_exedit_sdk
* Boost https://www.boost.org/
* Python https://www.python.org/

## 👽️ 作成者情報

* 作成者 - 蛇色 (へびいろ)
* GitHub - https://github.com/hebiiro
* Twitter - https://x.com/io_hebiiro

## 🚨 免責事項

この作成物および同梱物を使用したことによって生じたすべての障害・損害・不具合等に関しては、私と私の関係者および私の所属するいかなる団体・組織とも、一切の責任を負いません。各自の責任においてご使用ください。
