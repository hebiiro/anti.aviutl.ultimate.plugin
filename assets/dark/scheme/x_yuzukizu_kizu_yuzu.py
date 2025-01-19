import dark
import core
import utils

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(80, 80, 80)),
			utils.Tone(core.RGB(120, 80, 200)),
			utils.Tone(core.RGB(200, 100, 0))
		),
		# 縁
		(
			utils.Tone(core.RGB(120, 120, 120)),
			utils.Tone(core.RGB(120, 80, 200)),
			utils.Tone(core.RGB(200, 100, 0))
		),
		# 文字
		(
			utils.Tone(core.RGB(120, 60, 0)),
			utils.Tone(core.RGB(120, 120, 240)),
			utils.Tone(core.RGB(240, 120, 0))
		)
	)
else:
	utils.create_light_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(240, 240, 240)),
			utils.Tone(core.RGB(120, 100, 250)),
			utils.Tone(core.RGB(240, 120, 0))
		),
		# 縁
		(
			utils.Tone(core.RGB(120, 120, 120)),
			utils.Tone(core.RGB(120, 100, 250)),
			utils.Tone(core.RGB(240, 120, 0))
		),
		# 文字
		(
			utils.Tone(core.RGB(120, 60, 0)),
			utils.Tone(core.RGB(60, 40, 240)),
			utils.Tone(core.RGB(240, 120, 0))
		)
	)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
