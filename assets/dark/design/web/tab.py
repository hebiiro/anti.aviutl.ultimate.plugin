import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *
from .function import *

#
# このクラスはタブコントロールを描画します。
#
class Tab(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Tab, self).__init__(core.VSCLASS_TAB)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_TAB)
		self.add_theme(core.VSCLASS_EXPLORER_TAB)

		# 描画関数を追加します。
		self.add_handler(0, 0, self.draw_background, STUFF_BACKGROUND)
		self.add_handler(core.TABP_PANE, 0, self.draw_pane, STUFF_BACKGROUND)
		self.add_handler(core.TABP_TOPTABITEM, core.TIS_NORMAL, self.draw_item, STUFF_NORMAL, selected=False)
		self.add_handler(core.TABP_TOPTABITEM, core.TIS_DISABLED, self.draw_item, STUFF_DISABLED, selected=False)
		self.add_handler(core.TABP_TOPTABITEM, core.TIS_HOT, self.draw_item, STUFF_HOT, selected=False)
		self.add_handler(core.TABP_TOPTABITEM, core.TIS_SELECTED, self.draw_item, STUFF_SELECTED, selected=True)
		self.add_handler(core.TABP_TOPTABITEM, core.TIS_FOCUSED, self.draw_item, STUFF_SELECTED, selected=False)
		self.add_handler(core.TABP_TOPTABITEMLEFTEDGE, core.TIS_NORMAL, self.draw_item, STUFF_NORMAL, selected=False)
		self.add_handler(core.TABP_TOPTABITEMLEFTEDGE, core.TIS_DISABLED, self.draw_item, STUFF_DISABLED, selected=False)
		self.add_handler(core.TABP_TOPTABITEMLEFTEDGE, core.TIS_HOT, self.draw_item, STUFF_HOT, selected=False)
		self.add_handler(core.TABP_TOPTABITEMLEFTEDGE, core.TIS_SELECTED, self.draw_item, STUFF_SELECTED, selected=True)
		self.add_handler(core.TABP_TOPTABITEMLEFTEDGE, core.TIS_FOCUSED, self.draw_item, STUFF_SELECTED, selected=False)
		self.add_handler(core.TABP_TOPTABITEMRIGHTEDGE, core.TIS_NORMAL, self.draw_item, STUFF_NORMAL, selected=False)
		self.add_handler(core.TABP_TOPTABITEMRIGHTEDGE, core.TIS_DISABLED, self.draw_item, STUFF_DISABLED, selected=False)
		self.add_handler(core.TABP_TOPTABITEMRIGHTEDGE, core.TIS_HOT, self.draw_item, STUFF_HOT, selected=False)
		self.add_handler(core.TABP_TOPTABITEMRIGHTEDGE, core.TIS_SELECTED, self.draw_item, STUFF_SELECTED, selected=True)
		self.add_handler(core.TABP_TOPTABITEMRIGHTEDGE, core.TIS_FOCUSED, self.draw_item, STUFF_SELECTED, selected=False)
		self.add_handler(core.TABP_TOPTABITEMBOTHEDGE, core.TIS_NORMAL, self.draw_item, STUFF_NORMAL, selected=False)
		self.add_handler(core.TABP_TOPTABITEMBOTHEDGE, core.TIS_DISABLED, self.draw_item, STUFF_DISABLED, selected=False)
		self.add_handler(core.TABP_TOPTABITEMBOTHEDGE, core.TIS_HOT, self.draw_item, STUFF_HOT, selected=False)
		self.add_handler(core.TABP_TOPTABITEMBOTHEDGE, core.TIS_SELECTED, self.draw_item, STUFF_SELECTED, selected=True)
		self.add_handler(core.TABP_TOPTABITEMBOTHEDGE, core.TIS_FOCUSED, self.draw_item, STUFF_SELECTED, selected=False)

	#
	# 背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
			if (stuff.accent.color != core.CLR_NONE):
				rc = dark.RECT(args.rc)
				rc.top = rc.bottom - 1
				dark.exports.painter.fill_rect(args.dc, rc, stuff.accent.color)
		return True

	#
	# ペインを描画します。
	#
	def draw_pane(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (stuff.accent.color != core.CLR_NONE):
				rc = dark.RECT(args.rc)
				rc.top = rc.bottom - 2
				#dark.exports.painter.fill_rect(args.dc, rc, stuff.accent.color)
		return True

	#
	# アイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			if (not attrs.selected): rc.inflate(0, 2)
			rc.bottom -= 4
			if (hasattr(args, 'text')):
				dark.draw_str(args, rc, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, rc, stuff)
				if (attrs.selected):
					draw_underline(args, stuff, rc)
				else:
					draw_pattern(args, stuff, rc)
		return True
