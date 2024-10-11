import sys
import dark
from core.const import *
from core.function import *
from core.handler import *

#
# 辞書をクラスに変換して返します。
#
def dict_to_class(d):
	class Attrs(object): pass
	p = Attrs.__new__(Attrs)
	p.__dict__.update(d)
	return p

#
# このクラスは描画関数を管理します。
# (1) add_control()を呼び出してコントロールを追加します。
# (2) register()を呼び出して保持しているコントロールのregister()を呼び出します。
#
class Manager():
	#
	# コンストラクタです。
	#
	def __init__(self):
		self.controls = {} # コントロールの辞書です。
		self.draw_figure_handlers = {} # 図形描画関数の辞書です。
		self.draw_text_handlers = {} # 文字列描画関数の辞書です。
		self.text_out_handlers = {} # 文字列出力関数の辞書です。

	#
	# コントロールを追加します。
	#
	def add_control(self, control):
		self.controls[control.name] = control
		return control

	#
	# 各描画関数を登録します。
	#
	def register(self):
		for control in self.controls.values():
			control.register(self)
		return self.dark_draw_figure, self.dark_draw_text, self.dark_text_out

	#
	# 図形描画関数を登録します。
	#
	def register_draw_figure_handler(self, theme, part_id, state_id, handler):
		self.draw_figure_handlers[dark.VisualStyle(theme, part_id, state_id)] = handler
		return True

	#
	# 文字列描画関数を登録します。
	#
	def register_draw_text_handler(self, theme, part_id, state_id, handler):
		self.draw_text_handlers[dark.VisualStyle(theme, part_id, state_id)] = handler
		return True

	#
	# 文字列出力関数を登録します。
	#
	def register_text_out_handler(self, theme, part_id, state_id, handler):
		self.text_out_handlers[dark.VisualStyle(theme, part_id, state_id)] = handler
		return True

	#
	# ビジュアルスタイル用のスタッフを登録します。
	#
	def register_visual_style_stuff(self, theme, part_id, state_id, stuff):
		dark.exports.visual_style_manager.set_stuff(dark.VisualStyle(theme, part_id, state_id), stuff)
		return True

	#
	# 図形を描画します。
	#
	def dark_draw_figure(self, args):
		if (debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}(theme = {args.vs.theme:#010x}, part_id = {args.vs.part_id}, state_id = {args.vs.state_id}, {dark.str(args)})')
		handler = self.draw_figure_handlers.get(args.vs, None)
		if handler: return handler.func(args, handler.stuff_name, dict_to_class(handler.attrs))
		return False

	#
	# 文字列を描画します。
	#
	def dark_draw_text(self, args):
		if (debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}(theme = {args.vs.theme:#010x}, part_id = {args.vs.part_id}, state_id = {args.vs.state_id}, {dark.str(args)})')
		handler = self.draw_text_handlers.get(args.vs, None)
		if handler: return handler.func(args, handler.stuff_name, dict_to_class(handler.attrs))
		return False

	#
	# 文字列を出力します。
	#
	def dark_text_out(self, args):
		if (debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}(theme = {args.vs.theme:#010x}, part_id = {args.vs.part_id}, state_id = {args.vs.state_id}, {dark.str(args)})')
		if (args.dx == None): args.dx = 0
		handler = self.text_out_handlers.get(args.vs, None)
		if handler: return handler.func(args, handler.stuff_name, dict_to_class(handler.attrs))
		return False
