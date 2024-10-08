import dark
import core
import utils

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(255, 120, 160)),
			utils.Tone(core.RGB(100, 140, 240)),
			utils.Tone(core.RGB(100, 140, 240))
		),
		# 縁
		(
			utils.Tone(core.RGB(255, 120, 160)),
			utils.Tone(core.RGB(100, 140, 240)),
			utils.Tone(core.RGB(100, 140, 240))
		),
		# 文字
		(
			utils.Tone(core.RGB(160, 40, 80)),
			utils.Tone(core.RGB(150, 200, 255)),
			utils.Tone(core.RGB(150, 200, 255))
		)
	)
else:
	utils.create_light_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(255, 190, 230)),
			utils.Tone(core.RGB(100, 140, 240)),
			utils.Tone(core.RGB(100, 140, 240))
		),
		# 縁
		(
			utils.Tone(core.RGB(255, 190, 230)),
			utils.Tone(core.RGB(100, 140, 240)),
			utils.Tone(core.RGB(100, 140, 240))
		),
		# 文字
		(
			utils.Tone(core.RGB(160, 40, 80)),
			utils.Tone(core.RGB(0, 120, 240)),
			utils.Tone(core.RGB(0, 120, 240))
		)
	)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
