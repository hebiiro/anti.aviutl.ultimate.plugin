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
# このクラスは拡張編集を描画します。
#
class ExEdit(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ExEdit, self).__init__(core.VSCLASS_WINDOW + "_EXEDIT")

		# テーマを追加します。
		self.add_theme(core.VSCLASS_WINDOW)
		self.add_theme(core.VSCLASS_EXPLORER_WINDOW)

		# 描画関数を追加します。
		self.add_handler(core.WP_EXEDIT, 0, self.draw_background, STUFF_EXEDIT_BACKGROUND)
		self.add_handler(core.WP_EXEDIT, core.EES_SELECTION_FILL, None, STUFF_EXEDIT_SELECTION_FILL)
		self.add_handler(core.WP_EXEDIT, core.EES_SELECTION_EDGE, None, STUFF_EXEDIT_SELECTION_EDGE)
		self.add_handler(core.WP_EXEDIT, core.EES_SELECTION_BACKGROUND, None, STUFF_EXEDIT_SELECTION_BACKGROUND)
		self.add_handler(core.WP_EXEDIT, core.EES_SCALE_PRIMARY, self.draw_scale, STUFF_EXEDIT_SCALE_PRIMARY)
		self.add_handler(core.WP_EXEDIT, core.EES_SCALE_SECONDARY, self.draw_scale, STUFF_EXEDIT_SCALE_SECONDARY)
		self.add_handler(core.WP_EXEDIT, core.EES_SCENE_BUTTON_EDGE, self.draw_scene_button_edge, STUFF_EXEDIT_SCENE_BUTTON_EDGE)
		self.add_handler(core.WP_EXEDIT, core.EES_SCENE_BUTTON, self.draw_scene_button, STUFF_EXEDIT_SCENE_BUTTON)
		self.add_handler(core.WP_EXEDIT, core.EES_EVEN_LAYER_BUTTON_EDGE, self.draw_layer_button_edge, STUFF_EXEDIT_EVEN_LAYER_BUTTON_EDGE)
		self.add_handler(core.WP_EXEDIT, core.EES_EVEN_LAYER_BUTTON_ACTIVE, self.draw_layer_button, STUFF_EXEDIT_EVEN_LAYER_BUTTON_ACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_EVEN_LAYER_BUTTON_INACTIVE, self.draw_layer_button, STUFF_EXEDIT_EVEN_LAYER_BUTTON_INACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_EVEN_LAYER_BACKGROUND_ACTIVE, self.draw_layer_background, STUFF_EXEDIT_EVEN_LAYER_BACKGROUND_ACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_EVEN_LAYER_BACKGROUND_INACTIVE, self.draw_layer_background, STUFF_EXEDIT_EVEN_LAYER_BACKGROUND_INACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_ODD_LAYER_BUTTON_EDGE, self.draw_layer_button_edge, STUFF_EXEDIT_ODD_LAYER_BUTTON_EDGE)
		self.add_handler(core.WP_EXEDIT, core.EES_ODD_LAYER_BUTTON_ACTIVE, self.draw_layer_button, STUFF_EXEDIT_ODD_LAYER_BUTTON_ACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_ODD_LAYER_BUTTON_INACTIVE, self.draw_layer_button, STUFF_EXEDIT_ODD_LAYER_BUTTON_INACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_ODD_LAYER_BACKGROUND_ACTIVE, self.draw_layer_background, STUFF_EXEDIT_ODD_LAYER_BACKGROUND_ACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_ODD_LAYER_BACKGROUND_INACTIVE, self.draw_layer_background, STUFF_EXEDIT_ODD_LAYER_BACKGROUND_INACTIVE)
		self.add_handler(core.WP_EXEDIT, core.EES_LAYER_LINE_SEPARATOR, None, STUFF_EXEDIT_LAYER_LINE_SEPARATOR)
		self.add_handler(core.WP_EXEDIT, core.EES_LAYER_LINE_LEFT, None, STUFF_EXEDIT_LAYER_LINE_LEFT)
		self.add_handler(core.WP_EXEDIT, core.EES_LAYER_LINE_TOP, None, STUFF_EXEDIT_LAYER_LINE_TOP)
		self.add_handler(core.WP_EXEDIT, core.EES_LAYER_LINE_RIGHT, None, STUFF_EXEDIT_LAYER_LINE_RIGHT)
		self.add_handler(core.WP_EXEDIT, core.EES_LAYER_LINE_BOTTOM, None, STUFF_EXEDIT_LAYER_LINE_BOTTOM)

	#
	# 背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		dark.exports.painter.fill_rect(args.dc, args.rc, self.get_stuff(stuff_name))
		return True

	#
	# シーン選択ボタンの縁を描画します。
	#
	def draw_scene_button_edge(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		# Trueを返してデフォルトの処理を抑制します。
		return True

	#
	# シーン選択ボタンを描画します。
	#
	def draw_scene_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				if (args.options & win32con.ETO_OPAQUE):
					args.options &= ~win32con.ETO_OPAQUE
					if (stuff_name == STUFF_HOT or stuff_name == STUFF_PRESSED):
						draw_underline(args, stuff)
					else:
						draw_pattern(args, stuff)
				dark.draw_str(args, stuff)
			else:
				pass
		return True

	#
	# レイヤーボタンの縁を描画します。
	#
	def draw_layer_button_edge(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		# Trueを返してデフォルトの処理を抑制します。
		return True

	#
	# レイヤーボタンを描画します。
	#
	def draw_layer_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				if (hasattr(args, 'options')):
					if (args.options & win32con.ETO_OPAQUE):
						args.options &= ~win32con.ETO_OPAQUE
				rc = dark.RECT(args.rc)
				rc.inflate(1, 1)
				dark.exports.painter.fill_rect(args.dc, rc, self.get_stuff(STUFF_EXEDIT_DUMMY))
				rc.deflate(0, 1)
				rc.left += 1
				if (stuff_name == STUFF_HOT or stuff_name == STUFF_PRESSED):
					draw_underline(args, stuff)
				else:
					draw_pattern(args, stuff)
				dark.draw_str(args, stuff)
			else:
				pass
		return True

	#
	# レイヤー背景を描画します。
	#
	def draw_layer_background(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.inflate(1, 1)
			dark.exports.gdiplus.draw_round_all(args.dc, rc, stuff)
		return True

	#
	# 目盛りを描画します。
	#
	def draw_scale(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				pass
		return True
