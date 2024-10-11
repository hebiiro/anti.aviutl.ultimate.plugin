import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはリストビューを描画します。
#
class ListView(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ListView, self).__init__(core.VSCLASS_LISTVIEW)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_LISTVIEW)
		self.add_theme(core.VSCLASS_EXPLORER_LISTVIEW)
		self.add_theme(core.VSCLASS_CFD_LISTVIEW)
		self.add_theme(core.VSCLASS_ITEMSVIEW_LISTVIEW, True)

		# 描画関数を追加します。
		self.add_handler(core.LVP_LISTITEM, 0, None, STUFF_SEPARATOR)
		self.add_handler(core.LVP_LISTITEM, core.LISS_NORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(core.LVP_LISTITEM, core.LISS_DISABLED, self.draw_item, STUFF_DISABLED)
		self.add_handler(core.LVP_LISTITEM, core.LISS_HOT, self.draw_item, STUFF_HOT)
		self.add_handler(core.LVP_LISTITEM, core.LISS_SELECTED, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.LVP_LISTITEM, core.LISS_SELECTEDNOTFOCUS, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.LVP_LISTITEM, core.LISS_HOTSELECTED, self.draw_item, STUFF_SELECTED)

	#
	# アイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str_fill(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True
