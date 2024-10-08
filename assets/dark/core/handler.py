import sys
import dark
from core.const import *
from core.function import *

#
# このクラスはハンドラーです。
#
class Handler:
	#
	# コンストラクタです。
	#
	def __init__(self, func, attrs, stuff_name):
		self.func = func # 描画関数です。
		self.attrs = attrs # 追加属性の辞書です。
		self.stuff_name = stuff_name # スタッフの名前です。
