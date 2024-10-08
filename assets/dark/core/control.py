import sys
import dark
from core.const import *
from core.function import *
from core.handler import *

#
# このクラスはwin32のコントロールです。
# (1) add_theme()を呼び出してテーマハンドルを追加します。
# (2) add_..._handler()を呼び出して描画関数を追加します。
# (3) get_stuff()を呼び出してスタッフのパラメータを設定します。
# (4) register()を呼び出して保持している描画関数をマネージャに登録します。
#
class Control:
	#
	# コンストラクタです。
	#
	def __init__(self, name):
		self.name = name # このコントロールの名前です。
		self.themes = [] # テーマハンドルのリストです。
		self.draw_figure_handlers = {} # 図形描画関数の辞書です。
		self.draw_text_handlers = {} # 文字列描画関数の辞書です。
		self.text_out_handlers = {} # 文字列出力関数の辞書です。
		self.visual_style_stuffs = {} # ビジュアルスタイル用のスタッフの辞書です。
		self.stuffs = {} # 描画に使用する使用するスタッフのキャッシュの辞書です。

	#
	# テーマハンドルを追加します。
	#
	def add_theme(self, vsclass_name, for_dpi = False):
		self.themes.append(dark.exports.theme_manager.get_theme_data(vsclass_name, for_dpi))

	#
	# 描画関数を追加します。
	#
	def add_handler(self, part_id, state_id, func, figure_stuff_name, text_stuff_name = None, vs_stuff_name = None, **attrs):
		if (not text_stuff_name): text_stuff_name = figure_stuff_name
		if (not vs_stuff_name): vs_stuff_name = figure_stuff_name
		if (func):
			self.draw_figure_handlers[(part_id, state_id)] = Handler(func, attrs, figure_stuff_name)
			self.draw_text_handlers[(part_id, state_id)] = Handler(func, attrs, text_stuff_name)
			self.text_out_handlers[(part_id, state_id)] = Handler(func, attrs, text_stuff_name)
		if (vs_stuff_name):
			self.visual_style_stuffs[(part_id, state_id)] = vs_stuff_name

	#
	# 指定されたスタッフの名前を装飾して返します。
	# 内部的に使用されます。
	#
	def decorate_stuff_name(self, stuff_name):
		return f"{self.name}_{stuff_name}"

	#
	# 指定された名前のスタッフを返します。
	#
	def get_stuff(self, stuff_name):
		return self.stuffs[stuff_name]

	#
	# 指定された名前のスタッフを設定します。
	#
	def set_stuff(self, stuff_name, stuff):
		dark.exports.stuff_manager.set_stuff(self.decorate_stuff_name(stuff_name), stuff)
		self.stuffs[stuff_name] = stuff;
		return stuff;

	#
	# 各描画関数を登録します。
	#
	def register(self, manager):
		for theme in self.themes:
			# 図形描画関数を登録します。
			for k, v in self.draw_figure_handlers.items():
				manager.register_draw_figure_handler(theme, k[0], k[1], v)
			# 文字列描画関数を登録します。
			for k, v in self.draw_text_handlers.items():
				manager.register_draw_text_handler(theme, k[0], k[1], v)
			# 文字列出力関数を登録します。
			for k, v in self.text_out_handlers.items():
				manager.register_text_out_handler(theme, k[0], k[1], v)
			# ビジュアルスタイル用のスタッフを登録します。
			for k, v in self.visual_style_stuffs.items():
				stuff = self.get_stuff(v)
				if (stuff): manager.register_visual_style_stuff(theme, k[0], k[1], stuff)
