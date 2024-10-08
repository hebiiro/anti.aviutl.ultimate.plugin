import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはツールチップを描画します。
#
class ToolTip(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ToolTip, self).__init__(core.VSCLASS_TOOLTIP)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_TOOLTIP)
		self.add_theme(core.VSCLASS_EXPLORER_TOOLTIP)

		# 描画関数を追加します。
		self.add_handler(core.TTP_STANDARD, 0, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_STANDARD, core.TTSS_NORMAL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_STANDARD, core.TTSS_LINK, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_STANDARDTITLE, 0, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_STANDARDTITLE, core.TTSS_NORMAL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_STANDARDTITLE, core.TTSS_LINK, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOON, 0, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOON, core.TTBS_NORMAL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOON, core.TTBS_LINK, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONTITLE, 0, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONTITLE, core.TTBS_NORMAL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONTITLE, core.TTBS_LINK, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_CLOSE, 0, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_CLOSE, core.TTCS_NORMAL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_CLOSE, core.TTCS_HOT, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_CLOSE, core.TTCS_PRESSED, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONSTEM, 0, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONSTEM, core.TTBSS_POINTINGUPLEFTWALL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONSTEM, core.TTBSS_POINTINGUPCENTERED, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONSTEM, core.TTBSS_POINTINGUPRIGHTWALL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONSTEM, core.TTBSS_POINTINGDOWNRIGHTWALL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONSTEM, core.TTBSS_POINTINGDOWNCENTERED, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_BALLOONSTEM, core.TTBSS_POINTINGDOWNLEFTWALL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_WRENCH, 0, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_WRENCH, core.TTWS_NORMAL, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_WRENCH, core.TTWS_HOT, self.draw_tooltip, STUFF_NORMAL)
		self.add_handler(core.TTP_WRENCH, core.TTWS_PRESSED, self.draw_tooltip, STUFF_NORMAL)

	#
	# ツールチップを描画します。
	#
	def draw_tooltip(self, args, stuff_name, attrs):
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.painter.draw_rect(args.dc, args.rc, stuff)
		return True
