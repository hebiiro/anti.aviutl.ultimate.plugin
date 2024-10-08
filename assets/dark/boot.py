import sys
import importlib
import dark

#
# 指定されたスキームモジュールを立ち上げます。
#
def boot_scheme_module(scheme_module_name):
	print(scheme_module_name + 'を読み込みます')
	current_scheme_module = importlib.import_module(scheme_module_name)
	print(str(current_scheme_module) + 'を読み込みました')
	return current_scheme_module

#
# 指定されたスキンモジュールを立ち上げます。
#
def boot_skin_module(skin_module_name):
	print(skin_module_name + 'を読み込みます')
	current_skin_module = importlib.import_module(skin_module_name)
	print(str(current_skin_module) + 'を読み込みました')
	return current_skin_module
