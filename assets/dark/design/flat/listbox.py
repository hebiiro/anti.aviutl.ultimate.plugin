import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはリストボックスを描画します。
#
class ListBox(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ListBox, self).__init__(core.VSCLASS_LISTBOX)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_LISTBOX)
		self.add_theme(core.VSCLASS_EXPLORER_LISTBOX)
		self.add_theme(core.VSCLASS_COMBOBOX_LISTBOX)

		# 描画関数を追加します。
		self.add_handler(core.EP_EDITTEXT, core.ETS_NORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(core.EP_EDITTEXT, core.ETS_DISABLED, self.draw_item, STUFF_DISABLED)
		self.add_handler(core.EP_EDITTEXT, core.ETS_SELECTED, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.EP_EDITTEXT, core.ETS_HOT, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.EP_EDITTEXT, core.ETS_FOCUSED, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.EP_EDITTEXT, core.ETS_READONLY, self.draw_item, STUFF_READONLY)
		self.add_handler(core.EP_EDITTEXT, core.ETS_ASSIST, self.draw_item, STUFF_DISABLED)
		self.add_handler(core.EP_EDITTEXT, core.ETS_CUEBANNER, self.draw_item, STUFF_DISABLED)

	#
	# アイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str_fill(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True
