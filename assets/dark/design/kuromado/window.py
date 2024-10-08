import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはウィンドウの基本クラスです。
#
class Window(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self, sub_name):
		# スーパークラスのコンストラクタを呼び出します。
		super(Window, self).__init__(core.VSCLASS_WINDOW + "_" + sub_name)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_WINDOW)
		self.add_theme(core.VSCLASS_EXPLORER_WINDOW)
