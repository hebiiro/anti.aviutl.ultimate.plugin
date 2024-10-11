import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスは拡張編集を描画します。
#
class ExEdit(base.ExEdit):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ExEdit, self).__init__()

	#
	# シーン選択ボタンを描画します。
	#
	def draw_scene_button(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				if (args.options & win32con.ETO_OPAQUE):
					args.options &= ~win32con.ETO_OPAQUE
					if (stuff_name == STUFF_HOT or stuff_name == STUFF_PRESSED):
						# 下線を描画します。
						draw_underline(args, stuff)
					else:
						# 模様を描画します。
						draw_pattern(args, stuff)
				dark.draw_str(args, stuff)
		return True

	#
	# レイヤーボタンを描画します。
	#
	def draw_layer_button(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
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
					# 下線を描画します。
					draw_underline(args, stuff)
				else:
					# 模様を描画します。
					draw_pattern(args, stuff)
				dark.draw_str(args, stuff)
		return True
