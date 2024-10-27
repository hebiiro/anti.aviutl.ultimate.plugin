# 🎉 『拡張編集微調整』アドイン

* 拡張編集のタイムライン描画を微調整します。
* ただし、タイムラインアイテムに対する微調整はpatch.aulのグラデーション描画を無効にしている場合のみ有効になります。

![綺麗にグラデーション](https://user-images.githubusercontent.com/96464759/152974130-bcda58c8-fdab-43fa-96fa-bfbe091975f2.png)

## 🔧 設定方法

* AviUtlを起動する前に`config/exedit_tweaker.aua.json`をテキストエディタで編集してください。
* 色は`#rrggbb` `#rgb` `r, g, b`のいずれかの形式で指定できます。

* `gradient_fill`
	* `enabled` ✏️ `true`の場合はタイムラインアイテムの描画を変更します。
	* `edge` ✏️ タイムラインアイテムの縁の設定です。
		* `inner` ✏️ 縁の内側の設定です。
		* `outer` ✏️ 縁の外側の設定です。
			* `size`
				* `cx` ✏️ 縁の幅です。
				* `cy` ✏️ 縁の高さです。
			* `color` ✏️ 縁の色です。
	* `selection`
		* `fill`
			* `color` ✏️ 選択範囲の色です。
		* `stroke`
			* `color` ✏️ 選択範囲の縁の色です。
		* `background`
			* `color` ✏️ 選択範囲の背景色です。
	* `layer`
		* `bound`
			* `left`
				* `color` ✏️ レイヤー境界左側の色です。
			* `top`
				* `color` ✏️ レイヤー境界上側の色です。
			* `right`
				* `color` ✏️ レイヤー境界右側の色です。
			* `bottom`
				* `color` ✏️ レイヤー境界下側の色です。
		* `separator`
			* `color` ✏️ セパレータの色です。
