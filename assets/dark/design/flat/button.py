import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはボタンを描画します。
#
class Button(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Button, self).__init__(core.VSCLASS_BUTTON)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_BUTTON)
		self.add_theme(core.VSCLASS_EXPLORER_BUTTON)

		# 描画関数を追加します。
		self.add_handler(core.BP_PUSHBUTTON, core.PBS_NORMAL, self.draw_push_button, STUFF_NORMAL, pressed=False)
		self.add_handler(core.BP_PUSHBUTTON, core.PBS_DISABLED, self.draw_push_button, STUFF_DISABLED, pressed=False)
		self.add_handler(core.BP_PUSHBUTTON, core.PBS_DEFAULTED, self.draw_push_button, STUFF_NORMAL, pressed=False)
		self.add_handler(core.BP_PUSHBUTTON, core.PBS_DEFAULTED_ANIMATING, self.draw_push_button, STUFF_NORMAL, pressed=False)
		self.add_handler(core.BP_PUSHBUTTON, core.PBS_HOT, self.draw_push_button, STUFF_HOT, pressed=False)
		self.add_handler(core.BP_PUSHBUTTON, core.PBS_PRESSED, self.draw_push_button, STUFF_PRESSED, pressed=True)

		self.add_handler(core.BP_RADIOBUTTON, core.RBS_UNCHECKEDNORMAL, self.draw_radio_button, STUFF_NORMAL, pressed=False, checked=False)
		self.add_handler(core.BP_RADIOBUTTON, core.RBS_UNCHECKEDDISABLED, self.draw_radio_button, STUFF_DISABLED, pressed=False, checked=False)
		self.add_handler(core.BP_RADIOBUTTON, core.RBS_UNCHECKEDHOT, self.draw_radio_button, STUFF_HOT, pressed=False, checked=False)
		self.add_handler(core.BP_RADIOBUTTON, core.RBS_UNCHECKEDPRESSED, self.draw_radio_button, STUFF_PRESSED, pressed=True, checked=False)

		self.add_handler(core.BP_RADIOBUTTON, core.RBS_CHECKEDNORMAL, self.draw_radio_button, STUFF_NORMAL, pressed=False, checked=True)
		self.add_handler(core.BP_RADIOBUTTON, core.RBS_CHECKEDDISABLED, self.draw_radio_button, STUFF_DISABLED, pressed=False, checked=True)
		self.add_handler(core.BP_RADIOBUTTON, core.RBS_CHECKEDHOT, self.draw_radio_button, STUFF_HOT, pressed=False, checked=True)
		self.add_handler(core.BP_RADIOBUTTON, core.RBS_CHECKEDPRESSED, self.draw_radio_button, STUFF_PRESSED, pressed=True, checked=True)

		self.add_handler(core.BP_CHECKBOX, core.CBS_UNCHECKEDNORMAL, self.draw_checkbox, STUFF_NORMAL, pressed=False, checked=False)
		self.add_handler(core.BP_CHECKBOX, core.CBS_UNCHECKEDDISABLED, self.draw_checkbox, STUFF_DISABLED, pressed=False, checked=False)
		self.add_handler(core.BP_CHECKBOX, core.CBS_UNCHECKEDHOT, self.draw_checkbox, STUFF_HOT, pressed=False, checked=False)
		self.add_handler(core.BP_CHECKBOX, core.CBS_UNCHECKEDPRESSED, self.draw_checkbox, STUFF_NORMAL, pressed=True, checked=False)

		self.add_handler(core.BP_CHECKBOX, core.CBS_CHECKEDNORMAL, self.draw_checkbox, STUFF_NORMAL, pressed=False, checked=True)
		self.add_handler(core.BP_CHECKBOX, core.CBS_CHECKEDDISABLED, self.draw_checkbox, STUFF_DISABLED, pressed=False, checked=True)
		self.add_handler(core.BP_CHECKBOX, core.CBS_CHECKEDHOT, self.draw_checkbox, STUFF_HOT, pressed=False, checked=True)
		self.add_handler(core.BP_CHECKBOX, core.CBS_CHECKEDPRESSED, self.draw_checkbox, STUFF_NORMAL, pressed=True, checked=True)

		self.add_handler(core.BP_GROUPBOX, core.GBS_NORMAL, self.draw_groupbox, STUFF_NORMAL)
		self.add_handler(core.BP_GROUPBOX, core.GBS_DISABLED, self.draw_groupbox, STUFF_DISABLED)

	#
	# プッシュボタンを描画します。
	#
	def draw_push_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.inflate(1, 1)
			if (hasattr(args, 'text')):
				if (attrs.pressed): rc.offset(1, 1)
				dark.draw_str(args, rc, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
				if (stuff_name == STUFF_HOT):
					dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.etched)
				elif (attrs.pressed):
					dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.sunken)
		return True

	#
	# ラジオボタンを描画します。
	#
	def draw_radio_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.inflate(0, 1)
			if (attrs.pressed): rc.offset(1, 1)
			if (hasattr(args, 'text')):
				dark.draw_str(args, rc, stuff)
			else:
				#rc.offset(2, 0)
				if (attrs.checked): core.draw_icon(args, self.get_stuff(STUFF_CHECKED), 'Webdings', '\u003D', rc)
				core.draw_icon(args, stuff, 'Webdings', '\u0063', rc)
		return True

	#
	# チェックボックスを描画します。
	#
	def draw_checkbox(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.inflate(0, 1)
			if (attrs.pressed): rc.offset(1, 1)
			if (hasattr(args, 'text')):
				dark.draw_str(args, rc, stuff)
			else:
				#rc.offset(2, 0)
				if (attrs.checked): core.draw_icon(args, self.get_stuff(STUFF_CHECKED), 'Webdings', '\u0061', rc)
				core.draw_icon(args, stuff, 'Webdings', '\u0063', rc)
		return True

	#
	# グループボックスを描画します。
	#
	def draw_groupbox(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = dark.Stuff(self.get_stuff(stuff_name))
			if (hasattr(args, 'text')):
				dark.draw_str_fill(args, args.rc, stuff)
			else:
				dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.etched)
		return True
