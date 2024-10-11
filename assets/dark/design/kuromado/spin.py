import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはスピンボタンを描画します。
#
class Spin(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Spin, self).__init__(core.VSCLASS_SPIN)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_SPIN)
		self.add_theme(core.VSCLASS_EXPLORER_SPIN)

		# 描画関数を追加します。
		self.add_handler(core.SPNP_UP, core.UPS_NORMAL, self.draw_spin_button, STUFF_NORMAL, icon='\uE014', pressed=False)
		self.add_handler(core.SPNP_UP, core.UPS_DISABLED, self.draw_spin_button, STUFF_DISABLED, icon='\uE014', pressed=False)
		self.add_handler(core.SPNP_UP, core.UPS_HOT, self.draw_spin_button, STUFF_HOT, icon='\uE014', pressed=False)
		self.add_handler(core.SPNP_UP, core.UPS_PRESSED, self.draw_spin_button, STUFF_PRESSED, icon='\uE014', pressed=True)
		self.add_handler(core.SPNP_DOWN, core.DNS_NORMAL, self.draw_spin_button, STUFF_NORMAL, icon='\uE015', pressed=False)
		self.add_handler(core.SPNP_DOWN, core.DNS_DISABLED, self.draw_spin_button, STUFF_DISABLED, icon='\uE015', pressed=False)
		self.add_handler(core.SPNP_DOWN, core.DNS_HOT, self.draw_spin_button, STUFF_HOT, icon='\uE015', pressed=False)
		self.add_handler(core.SPNP_DOWN, core.DNS_PRESSED, self.draw_spin_button, STUFF_PRESSED, icon='\uE015', pressed=True)
		self.add_handler(core.SPNP_UPHORZ, core.UPHZS_NORMAL, self.draw_spin_button, STUFF_NORMAL, icon='\uE013', pressed=False)
		self.add_handler(core.SPNP_UPHORZ, core.UPHZS_DISABLED, self.draw_spin_button, STUFF_DISABLED, icon='\uE013', pressed=False)
		self.add_handler(core.SPNP_UPHORZ, core.UPHZS_HOT, self.draw_spin_button, STUFF_HOT, icon='\uE013', pressed=False)
		self.add_handler(core.SPNP_UPHORZ, core.UPHZS_PRESSED, self.draw_spin_button, STUFF_PRESSED, icon='\uE013', pressed=True)
		self.add_handler(core.SPNP_DOWNHORZ, core.DNHZS_NORMAL, self.draw_spin_button, STUFF_NORMAL, icon='\uE012', pressed=False)
		self.add_handler(core.SPNP_DOWNHORZ, core.DNHZS_DISABLED, self.draw_spin_button, STUFF_DISABLED, icon='\uE012', pressed=False)
		self.add_handler(core.SPNP_DOWNHORZ, core.DNHZS_HOT, self.draw_spin_button, STUFF_HOT, icon='\uE012', pressed=False)
		self.add_handler(core.SPNP_DOWNHORZ, core.DNHZS_PRESSED, self.draw_spin_button, STUFF_PRESSED, icon='\uE012', pressed=True)

	#
	# アイコンを描画します。
	#
	def draw_icon(self, args, stuff, attrs, rc):
		rc = dark.RECT(args.rc)
		rc.offset(0, -1)
		core.draw_icon(args, stuff, 'Meiryo', attrs.icon, rc, font_weight=900)

	#
	# スピンボタンを描画します。
	#
	def draw_spin_button(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			dark.exports.painter.fill_rect(args.dc, rc, self.get_stuff(STUFF_BACKGROUND))
			dark.exports.gdiplus.draw_round_all(args.dc, rc, stuff)
			if (attrs.pressed): rc.offset(1, 1)
			self.draw_icon(args, stuff, attrs, rc)
		return True
