import dark
import core
import utils

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(0, 70, 50)),
			utils.Tone(core.RGB(0, 120, 50)),
			utils.Tone(core.RGB(0, 120, 50))
		),
		# 縁
		(
			utils.Tone(core.RGB(0, 70, 50)),
			utils.Tone(core.RGB(0, 120, 50)),
			utils.Tone(core.RGB(0, 120, 50))
		),
		# 文字
		(
			utils.Tone(core.RGB(0, 60, 30)),
			utils.Tone(core.RGB(0, 240, 120)),
			utils.Tone(core.RGB(0, 240, 120))
		)
	)
else:
	utils.create_light_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(0, 160, 0)),
			utils.Tone(core.RGB(0, 160, 80)),
			utils.Tone(core.RGB(0, 160, 80))
		),
		# 縁
		(
			utils.Tone(core.RGB(0, 160, 0)),
			utils.Tone(core.RGB(0, 160, 80)),
			utils.Tone(core.RGB(0, 160, 80))
		),
		# 文字
		(
			utils.Tone(core.RGB(0, 40, 0)),
			utils.Tone(core.RGB(0, 120, 0)),
			utils.Tone(core.RGB(0, 120, 0))
		)
	)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
