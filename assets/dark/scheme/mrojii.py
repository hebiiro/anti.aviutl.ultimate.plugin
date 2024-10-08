import dark
import core
import utils

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(utils.CLR(70)),
			utils.Tone(utils.CLR(65)),
			utils.Tone(utils.CLR(65))
		),
		# 縁
		(
			utils.Tone(utils.CLR(70)),
			utils.Tone(utils.CLR(65)),
			utils.Tone(utils.CLR(65))
		),
		# 文字
		(
			utils.Tone(utils.CLR(120)),
			utils.Tone(utils.CLR(160)),
			utils.Tone(utils.CLR(160))
		)
	)
else:
	utils.create_light_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(utils.CLR(70)),
			utils.Tone(utils.CLR(65)),
			utils.Tone(utils.CLR(65))
		),
		# 縁
		(
			utils.Tone(utils.CLR(70)),
			utils.Tone(utils.CLR(65)),
			utils.Tone(utils.CLR(65))
		),
		# 文字
		(
			utils.Tone(utils.CLR(120)),
			utils.Tone(utils.CLR(50)),
			utils.Tone(utils.CLR(50))
		)
	)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
