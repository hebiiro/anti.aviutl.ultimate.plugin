import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはコモンアイテムズダイアログを描画します。
#
class CommonItemsDialog(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(CommonItemsDialog, self).__init__(core.VSCLASS_COMMONITEMSDIALOG)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_COMMONITEMSDIALOG, True)
		self.add_theme(core.VSCLASS_NAVBARCOMPOSITED_COMMONITEMSDIALOG, True)
		self.add_theme(core.VSCLASS_INACTIVENAVBARCOMPOSITED_COMMONITEMSDIALOG, True)

		# 描画関数を追加します。
		self.add_handler(1, 0, self.draw_background, STUFF_BACKGROUND)

	#
	# 背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True
