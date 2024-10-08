import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはメニューを描画します。(実際の描画はサブクラスで行います)
#
class Menu(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self, sub_name):
		# スーパークラスのコンストラクタを呼び出します。
		super(Menu, self).__init__(core.VSCLASS_MENU + "_" + sub_name)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_MENU, True)
		self.add_theme(core.VSCLASS_MENU)
		self.add_theme(core.VSCLASS_EXPLORER_MENU, True)
		self.add_theme(core.VSCLASS_EXPLORER_MENU)
