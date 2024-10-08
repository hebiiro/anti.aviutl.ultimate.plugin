import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはヘッダーコントロールを描画します。
#
class Header(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Header, self).__init__(core.VSCLASS_HEADER)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_HEADER)
		self.add_theme(core.VSCLASS_EXPLORER_HEADER)
		self.add_theme(core.VSCLASS_CFD_HEADER)
		self.add_theme(core.VSCLASS_ITEMSVIEW_HEADER, True)

		# 描画関数を追加します。
		self.add_handler(0, 1, self.draw_background, STUFF_BACKGROUND)

		self.add_handler(core.HP_HEADERITEM, core.HIS_NORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERITEM, core.HIS_SORTEDNORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERITEM, core.HIS_ICONNORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERITEM, core.HIS_ICONSORTEDNORMAL, self.draw_item, STUFF_NORMAL)

		self.add_handler(core.HP_HEADERITEM, core.HIS_HOT, self.draw_item, STUFF_HOT)
		self.add_handler(core.HP_HEADERITEM, core.HIS_SORTEDHOT, self.draw_item, STUFF_HOT)
		self.add_handler(core.HP_HEADERITEM, core.HIS_ICONHOT, self.draw_item, STUFF_HOT)
		self.add_handler(core.HP_HEADERITEM, core.HIS_ICONSORTEDHOT, self.draw_item, STUFF_HOT)

		self.add_handler(core.HP_HEADERITEM, core.HIS_PRESSED, self.draw_item, STUFF_PRESSED)
		self.add_handler(core.HP_HEADERITEM, core.HIS_SORTEDPRESSED, self.draw_item, STUFF_PRESSED)
		self.add_handler(core.HP_HEADERITEM, core.HIS_ICONPRESSED, self.draw_item, STUFF_PRESSED)
		self.add_handler(core.HP_HEADERITEM, core.HIS_ICONSORTEDPRESSED, self.draw_item, STUFF_PRESSED)

		self.add_handler(core.HP_HEADERITEMLEFT, core.HDDFS_NORMAL, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERITEMLEFT, core.HDDFS_SOFTHOT, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERITEMLEFT, core.HDDFS_HOT, self.draw_drop_down_button, STUFF_HOT)

		self.add_handler(core.HP_HEADERITEMRIGHT, core.HDDFS_NORMAL, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERITEMRIGHT, core.HDDFS_SOFTHOT, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERITEMRIGHT, core.HDDFS_HOT, self.draw_drop_down_button, STUFF_HOT)

		self.add_handler(core.HP_HEADERDROPDOWN, core.HDDFS_NORMAL, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERDROPDOWN, core.HDDFS_SOFTHOT, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERDROPDOWN, core.HDDFS_HOT, self.draw_drop_down_button, STUFF_HOT)

		self.add_handler(core.HP_HEADERDROPDOWNFILTER, core.HDDFS_NORMAL, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERDROPDOWNFILTER, core.HDDFS_SOFTHOT, self.draw_drop_down_button, STUFF_NORMAL)
		self.add_handler(core.HP_HEADERDROPDOWNFILTER, core.HDDFS_HOT, self.draw_drop_down_button, STUFF_HOT)

		self.add_handler(core.HP_HEADERSORTARROW, core.HSAS_SORTEDUP, self.draw_sort_button, STUFF_NORMAL, font_name='Webdings', icon_text='\u0036')
		self.add_handler(core.HP_HEADERSORTARROW, core.HSAS_SORTEDDOWN, self.draw_sort_button, STUFF_NORMAL, font_name='Webdings', icon_text='\u0035')

	#
	# 背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True

	#
	# アイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
				if (stuff_name == STUFF_PRESSED):
					dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.sunken)
				else:
					dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
		return True

	#
	# ドロップダウンボタンを描画します。
	#
	def draw_drop_down_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			width = rc.width
			rc.top = int((rc.top + rc.bottom - width) / 2)
			rc.bottom = rc.top + width
			return core.draw_icon(args, stuff, 'Webdings', '\u0036', rc)
		return True

	#
	# ソートボタンを描画します。
	#
	def draw_sort_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (type(args.rc) != type(None)):
				rc = dark.RECT(args.rc)
				rc.top -= 2
				rc.bottom += 6
				return core.draw_icon(args, stuff, attrs.font_name, attrs.icon_text, rc)
		return True
