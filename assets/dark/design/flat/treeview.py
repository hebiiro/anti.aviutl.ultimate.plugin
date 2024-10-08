import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはツリービューを描画します。
#
class TreeView(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(TreeView, self).__init__(core.VSCLASS_TREEVIEW)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_TREEVIEW)
		self.add_theme(core.VSCLASS_EXPLORER_TREEVIEW)
		self.add_theme(core.VSCLASS_CFD_TREEVIEW)

		# 描画関数を追加します。
		self.add_handler(core.TVP_TREEITEM, core.TREIS_NORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(core.TVP_TREEITEM, core.TREIS_DISABLED, self.draw_item, STUFF_DISABLED)
		self.add_handler(core.TVP_TREEITEM, core.TREIS_HOT, self.draw_item, STUFF_HOT)
		self.add_handler(core.TVP_TREEITEM, core.TREIS_SELECTED, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.TVP_TREEITEM, core.TREIS_SELECTEDNOTFOCUS, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.TVP_TREEITEM, core.TREIS_HOTSELECTED, self.draw_item, STUFF_SELECTED)
		self.add_handler(core.TVP_GLYPH, core.GLPS_CLOSED, self.draw_glyph, STUFF_NORMAL, font_name='Webdings', icon_text='\u0034')
		self.add_handler(core.TVP_GLYPH, core.GLPS_OPENED, self.draw_glyph, STUFF_NORMAL, font_name='Webdings', icon_text='\u0036')
		self.add_handler(core.TVP_HOTGLYPH, core.HGLPS_CLOSED, self.draw_glyph, STUFF_HOT, font_name='Webdings', icon_text='\u0034')
		self.add_handler(core.TVP_HOTGLYPH, core.HGLPS_OPENED, self.draw_glyph, STUFF_HOT, font_name='Webdings', icon_text='\u0036')

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

	#
	# グリフ(ボタン)を描画します。
	#
	def draw_glyph(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.gdiplus.draw_round_all(args.dc, args.rc, stuff)
			core.draw_icon(args, stuff, attrs.font_name, attrs.icon_text)
		return True
