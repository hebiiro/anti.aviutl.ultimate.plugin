import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはコンボボックスを描画します。
#
class ComboBox(base.ComboBox):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ComboBox, self).__init__()

	#
	# コンボボックスを描画します。
	#
	def draw_combobox(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				# フラットに見えるように描画します。
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
				if (stuff_name == STUFF_HOT or stuff_name == STUFF_PRESSED):
					dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.etched)
		return True
