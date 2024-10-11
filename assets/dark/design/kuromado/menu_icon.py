import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *
from .menu import *

#
# このクラスはメニューのアイコンを描画します。
#
class MenuIcon(Menu):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(MenuIcon, self).__init__("ICON")

		# 描画関数を追加します。
		self.add_handler(core.MENU_POPUPCHECK, core.MC_CHECKMARKNORMAL, self.draw_checkmark, STUFF_NORMAL)
		self.add_handler(core.MENU_POPUPCHECK, core.MC_CHECKMARKDISABLED, self.draw_checkmark, STUFF_DISABLED)
		self.add_handler(core.MENU_POPUPCHECK, core.MC_BULLETNORMAL, self.draw_bullet, STUFF_NORMAL)
		self.add_handler(core.MENU_POPUPCHECK, core.MC_BULLETDISABLED, self.draw_bullet, STUFF_DISABLED)
		self.add_handler(core.MENU_POPUPSUBMENU, core.MSM_NORMAL, self.draw_sub_menu, STUFF_NORMAL)
		self.add_handler(core.MENU_POPUPSUBMENU, core.MSM_DISABLED, self.draw_sub_menu, STUFF_DISABLED)
		self.add_handler(core.MENU_SYSTEMCLOSE, core.MSYSC_NORMAL, self.draw_close, STUFF_NORMAL)
		self.add_handler(core.MENU_SYSTEMCLOSE, core.MSYSC_DISABLED, self.draw_close, STUFF_DISABLED)
		self.add_handler(core.MENU_SYSTEMMAXIMIZE, core.MSYSMX_NORMAL, self.draw_maximize, STUFF_NORMAL)
		self.add_handler(core.MENU_SYSTEMMAXIMIZE, core.MSYSMX_DISABLED, self.draw_maximize, STUFF_DISABLED)
		self.add_handler(core.MENU_SYSTEMMINIMIZE, core.MSYSMN_NORMAL, self.draw_minimize, STUFF_NORMAL)
		self.add_handler(core.MENU_SYSTEMMINIMIZE, core.MSYSMN_DISABLED, self.draw_minimize, STUFF_DISABLED)
		self.add_handler(core.MENU_SYSTEMRESTORE, core.MSYSR_NORMAL, self.draw_restore, STUFF_NORMAL)
		self.add_handler(core.MENU_SYSTEMRESTORE, core.MSYSR_DISABLED, self.draw_restore, STUFF_DISABLED)

	#
	# チェックマークを描画します。
	#
	def draw_checkmark(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		stuff = self.get_stuff(stuff_name)
		#return core.draw_icon(args, stuff, 'Webdings', '\u0061')
		#return core.draw_icon(args, stuff, 'メイリオ', '✔️')
		#return core.draw_icon(args, stuff, 'メイリオ', '☑')
		return core.draw_icon(args, stuff, 'メイリオ', '✅')

	#
	# ラジオボタンを描画します。
	#
	def draw_bullet(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		stuff = self.get_stuff(stuff_name)
		#return core.draw_icon(args, stuff, 'Webdings', '\u003D')
		#return core.draw_icon(args, stuff, 'メイリオ', '⭕')
		#return core.draw_icon(args, stuff, 'メイリオ', '⭗')
		return core.draw_icon(args, stuff, 'メイリオ', '⬤')
		#return core.draw_icon(args, stuff, 'メイリオ', '🞇')
		#return core.draw_icon(args, stuff, 'メイリオ', '🔘')
		#return core.draw_icon(args, stuff, 'メイリオ', '🔴')
		#return core.draw_icon(args, stuff, 'メイリオ', '⚔️')
		#return core.draw_icon(args, stuff, 'メイリオ', '☠️')
		#return core.draw_icon(args, stuff, 'メイリオ', '🗡️')
		#return core.draw_icon(args, stuff, 'メイリオ', '🔪')
		#return core.draw_icon(args, stuff, 'メイリオ', '🌸')
		#return core.draw_icon(args, stuff, 'メイリオ', '🎌')
		#return core.draw_icon(args, stuff, 'メイリオ', '🇰🇷')
		#return core.draw_icon(args, stuff, 'メイリオ', '🇯🇵')
		#return core.draw_icon(args, stuff, 'メイリオ', '❌')
		#return core.draw_icon(args, stuff, 'メイリオ', '❎')

	#
	# サブメニューマークを描画します。
	#
	def draw_sub_menu(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		stuff = self.get_stuff(stuff_name)
		#return core.draw_icon(args, stuff, 'Webdings', '\u0034')
		#return core.draw_icon(args, stuff, 'メイリオ', '>')
		#return core.draw_icon(args, stuff, 'メイリオ', '﹥')
		#return core.draw_icon(args, stuff, 'メイリオ', '＞')
		#return core.draw_icon(args, stuff, 'メイリオ', '≫')
		#return core.draw_icon(args, stuff, 'メイリオ', '▶')
		rc = dark.RECT(args.rc)
		rc.inflate(6, 6)
		rc.offset(0, -2)
		return core.draw_icon(args, stuff, 'Meiryo', '\uE013', rc)

	#
	# 閉じるマークを描画します。
	#
	def draw_close(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		stuff = self.get_stuff(stuff_name)
		return core.draw_icon(args, stuff, 'Webdings', '\u0072')

	#
	# 最大化マークを描画します。
	#
	def draw_maximize(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		stuff = self.get_stuff(stuff_name)
		return core.draw_icon(args, stuff, 'Webdings', '\u0031')

	#
	# 最小化マークを描画します。
	#
	def draw_minimize(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		stuff = self.get_stuff(stuff_name)
		return core.draw_icon(args, stuff, 'Webdings', '\u0030')

	#
	# 元のサイズに戻すマークを描画します。
	#
	def draw_restore(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		stuff = self.get_stuff(stuff_name)
		return core.draw_icon(args, stuff, 'Webdings', '\u0032')
