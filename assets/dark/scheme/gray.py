import dark
import core
import utils

color = utils.CLR(128)
tone = utils.Tone(color)
tone_array = (tone, tone, tone)

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(tone_array, tone_array, tone_array)
else:
	utils.create_light_tone_stuffs(tone_array, tone_array, tone_array)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
