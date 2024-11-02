import dark
import core
import utils
from core import CLR_NONE
from utils import CLR
from utils import add_color as ADD

#
# このクラスはパレットです。
#
class Palette():
	#
	# コンストラクタです。
	#
	def __init__(self, fill, fill_sub, text, border, place_holder):
		self.fill = fill
		self.fill_sub = fill_sub
		self.text = text
		self.border = border

# よく使う変数を定義します。
fill_mode = dark.Stuff.Fill.Mode.c_bottom_right

# ダークモジュールの設定値を取得します。
ellipse = dark.hive.ellipse
border_width = dark.hive.border_width
shadow_density = dark.hive.shadow_density / 100
