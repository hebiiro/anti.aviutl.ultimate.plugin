# AviUtlチューナープラグイン

[AviUtl](http://spring-fragrance.mints.ne.jp/aviutl/)
のエディットボックスのサイズやフォントを変更したり、UNICODE文字列を入力できるようにしたりします。<br>
[最新バージョンをダウンロード](../../releases/latest/)

![綺麗にグラデーション](https://user-images.githubusercontent.com/96464759/152974130-bcda58c8-fdab-43fa-96fa-bfbe091975f2.png)

## 導入方法

以下のファイルを AviUtl の Plugins フォルダに入れてください。
* Last.auf
* Last (フォルダ)

# エディットボックスツイーカー

## 設定方法

AviUtl を起動する前に EditBoxTweaker.ini をテキストエディタで編集してください。

```ini
[Config]
unicodeInput=1 ; 複数行エディットボックスで UNICODE 文字を入力したい場合は 1 を指定します。
crlA=1 ; 複数行エディットボックスで Ctrl+A を有効にしたい場合は 1 を指定します。
delta=200 ; エディットボックスの高さに加算する値を指定します。例えば、200 を指定するとエディットボックスの高さが通常より 200 ピクセル高くなります。
font.name=Segoe UI ; エディットボックスで使用するフォントのフォント名を指定します。
font.height=14 ; フォントのサイズを指定します。
font.pitch=1 ; 固定幅を指定する場合は 1 を指定します。
```

# 拡張編集ツイーカー

## 設定方法

AviUtl を起動する前に ExEditTweaker.ini をテキストエディタで編集してください。<br>
カラーは ```#rrggbb``` ```#rgb``` ```r, g, b``` のいずれかの形式で指定できます。<br>

```ini
[Config]
gradientFill=0 ; グラデーション描画を変更する場合は 1 を指定します。ただし、patch.aul のグラデーション描画を無効にしている場合のみ有効になります。
gradientFill.inner.color=255, 255, 255 ; 内側の枠の色。
gradientFill.inner.size.cx=1 ; 内側の枠の横幅。0以下なら枠の左右は描画しない。
gradientFill.inner.size.cy=1 ; 内側の枠の縦幅。0以下なら枠の上下は描画しない。
gradientFill.outer.color=0, 0, 0 ; 外側の枠の色。指定の仕方は内側の枠と同じ。
gradientFill.outer.size.cx=1
gradientFill.outer.size.cy=1
selection.fill.color= ; 選択領域の色。
selection.edge.color= ; 選択領域端の色。
selection.background.color= ; 選択領域外の色。
layer.bound.left.color= ; レイヤー間ボーダーの左側の色。
layer.bound.right.color= ; レイヤー間ボーダーの右側の色。
layer.bound.top.color= ; レイヤー間ボーダーの上側の色。
layer.bound.bottom.color= ; レイヤー間ボーダーの下側の色。
layer.separator.color= ; レイヤーボタンとレイヤーの間の境界線の色。
```

## 更新履歴

* r1 - 2023/07/23 初版

## 動作確認

* (必須) AviUtl 1.10 & 拡張編集 0.92 http://spring-fragrance.mints.ne.jp/aviutl/
* (共存確認) patch.aul r43 謎さうなフォーク版29 https://scrapbox.io/nazosauna/patch.aul

## クレジット

* Microsoft Research Detours Package https://github.com/microsoft/Detours
* aviutl_exedit_sdk https://github.com/ePi5131/aviutl_exedit_sdk
* Common Library https://github.com/hebiiro/Common-Library

## 作成者情報

* 作成者 - 蛇色 (へびいろ)
* GitHub - https://github.com/hebiiro
* Twitter - https://twitter.com/io_hebiiro

## 免責事項

この作成物および同梱物を使用したことによって生じたすべての障害・損害・不具合等に関しては、私と私の関係者および私の所属するいかなる団体・組織とも、一切の責任を負いません。各自の責任においてご使用ください。
