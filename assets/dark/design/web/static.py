import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはスタティックコントロールを描画します。
#
class Static(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Static, self).__init__(core.VSCLASS_STATIC)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_STATIC)
		self.add_theme(core.VSCLASS_EXPLORER_STATIC)

		# 描画関数を追加します。
		self.add_handler(core.STAT_TEXT, 0, self.draw_text, STUFF_NORMAL)
		self.add_handler(core.STAT_TEXT, core.ETS_NORMAL, self.draw_text, STUFF_NORMAL)
		self.add_handler(core.STAT_TEXT, core.ETS_DISABLED, self.draw_text, STUFF_DISABLED)
		self.add_handler(core.STAT_ETCHED_EDGE, 0, self.draw_etched_edge, STUFF_NORMAL)

	#
	# 文字列を描画します。
	#
	def draw_text(self, args, stuff_name, attrs):
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str_fill(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True

	#
	# 窪みを描画します。
	#
	def draw_etched_edge(self, args, stuff_name, attrs):
		dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.etched)
		return True
