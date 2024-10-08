import dark
import core
import utils
import importlib

# このスキームはダークモードのみです。
importlib.import_module(__package__ + ".dark_mode")
"""
if (dark.hive.dark_mode):
	# ダークモードモジュールを読み込みます。
	importlib.import_module(__package__ + ".dark_mode")
else:
	# ライトモードモジュールを読み込みます。
	importlib.import_module(__package__ + ".light_mode")
"""

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
