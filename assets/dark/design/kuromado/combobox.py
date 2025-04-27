import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはコンボボックスを描画します。
#
class ComboBox(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ComboBox, self).__init__(core.VSCLASS_COMBOBOX)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_COMBOBOX)
		self.add_theme(core.VSCLASS_EXPLORER_COMBOBOX)
		self.add_theme(core.VSCLASS_CFD_COMBOBOX)
		self.add_theme(core.VSCLASS_ADDRESSCOMPOSITED_COMBOBOX)

		# 描画関数を追加します。
		self.add_handler(core.CP_BORDER, core.CBXS_NORMAL, self.draw_combobox, STUFF_NORMAL)
		self.add_handler(core.CP_BORDER, core.CBXS_DISABLED, self.draw_combobox, STUFF_DISABLED)
		self.add_handler(core.CP_BORDER, core.CBXS_HOT, self.draw_combobox, STUFF_HOT)
		self.add_handler(core.CP_BORDER, core.CBXS_PRESSED, self.draw_combobox, STUFF_PRESSED)
		self.add_handler(core.CP_READONLY, core.CBXS_NORMAL, self.draw_combobox, STUFF_NORMAL)
		self.add_handler(core.CP_READONLY, core.CBXS_DISABLED, self.draw_combobox, STUFF_DISABLED)
		self.add_handler(core.CP_READONLY, core.CBXS_HOT, self.draw_combobox, STUFF_HOT)
		self.add_handler(core.CP_READONLY, core.CBXS_PRESSED, self.draw_combobox, STUFF_PRESSED)
		self.add_handler(core.CP_DROPDOWNBUTTONRIGHT, core.CBXSR_NORMAL, self.draw_drop_down_button_right, STUFF_NORMAL)
		self.add_handler(core.CP_DROPDOWNBUTTONRIGHT, core.CBXSR_DISABLED, self.draw_drop_down_button_right, STUFF_DISABLED)
		self.add_handler(core.CP_DROPDOWNBUTTONRIGHT, core.CBXSR_HOT, self.draw_drop_down_button_right, STUFF_HOT)
		self.add_handler(core.CP_DROPDOWNBUTTONRIGHT, core.CBXSR_PRESSED, self.draw_drop_down_button_right, STUFF_PRESSED)
		self.add_handler(core.CP_DROPDOWNBUTTONLEFT, core.CBXSR_NORMAL, self.draw_drop_down_button_left, STUFF_NORMAL)
		self.add_handler(core.CP_DROPDOWNBUTTONLEFT, core.CBXSR_DISABLED, self.draw_drop_down_button_left, STUFF_DISABLED)
		self.add_handler(core.CP_DROPDOWNBUTTONLEFT, core.CBXSR_HOT, self.draw_drop_down_button_left, STUFF_HOT)
		self.add_handler(core.CP_DROPDOWNBUTTONLEFT, core.CBXSR_PRESSED, self.draw_drop_down_button_left, STUFF_PRESSED)

	#
	# コンボボックスを描画します。
	#
	def draw_combobox(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.gdiplus.draw_round_all(args.dc, args.rc, stuff)
		return True

	#
	# ドロップダウンボタンを描画します。
	#
	def draw_drop_down_button(self, args, stuff_name, attrs, rc):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			return core.draw_icon(args, stuff, 'Meiryo', '\uE015', rc, font_weight=900)
			#return core.draw_icon(args, stuff, 'Webdings', '\u0036', rc)
			#return core.draw_icon(args, stuff, 'Meiryo', '🔽')
			#return core.draw_icon(args, stuff, 'Meiryo', '⏬')

	#
	# 右側ドロップダウンボタンを描画します。
	#
	def draw_drop_down_button_right(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			width = rc.width
			rc.top = int((rc.top + rc.bottom - width) / 2) - 1
			rc.bottom = rc.top + width
			rc.offset(-2, 0)
			return self.draw_drop_down_button(args, stuff_name, attrs, rc)

	#
	# 左側ドロップダウンボタンを描画します。
	#
	def draw_drop_down_button_left(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			width = rc.width
			rc.top = int((rc.top + rc.bottom - width) / 2) - 1
			rc.bottom = rc.top + width
			rc.offset(2, 0)
			return self.draw_drop_down_button(args, stuff_name, attrs, rc)
