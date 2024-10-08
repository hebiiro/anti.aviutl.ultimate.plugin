import dark
import core

#
# スタッフの辞書です。
#
stuffs = {}

#
# 指定された名前のスタッフを返します。
#
def get_stuff(stuff_name):
	return stuffs[stuff_name]

#
# 指定された名前のスタッフが存在しない場合はTrueを返します。
#
def stuff_not_found(stuff_name):
	return stuff_name not in stuffs

#
# 指定された名前のスタッフを作成して返します。
#
def create_stuff(stuff_name, source_stuff_name = None):
	if (source_stuff_name == None):
		stuff = dark.Stuff()
	else:
		stuff = dark.Stuff(stuffs[source_stuff_name])
	stuffs[stuff_name] = stuff
	return stuff

#
# 指定された名前の新しいスタッフを作成して返します。
# 既存のスタッフが存在する場合はNoneを返します。
#
def new_stuff(stuff_name, source_stuff_name = None):
	if (stuff_not_found(stuff_name)):
		return create_stuff(stuff_name, source_stuff_name)
	else:
		return None
