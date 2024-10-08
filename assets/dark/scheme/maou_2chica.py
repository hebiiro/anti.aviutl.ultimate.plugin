import dark
import core
import utils

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(90, 90, 120)),
			utils.Tone(core.RGB(60, 60, 130)),
			utils.Tone(core.RGB(90, 90, 120))
		),
		# 縁
		(
			utils.Tone(core.RGB(90, 90, 120)),
			utils.Tone(core.RGB(60, 60, 130)),
			utils.Tone(core.RGB(90, 90, 120))
		),
		# 文字
		(
			utils.Tone(core.RGB(120, 120, 140)),
			utils.Tone(core.RGB(120, 120, 240)),
			utils.Tone(core.RGB(120, 120, 240))
		)
	)
else:
	utils.create_light_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(160, 160, 200)),
			utils.Tone(core.RGB(120, 120, 240)),
			utils.Tone(core.RGB(120, 120, 240))
		),
		# 縁
		(
			utils.Tone(core.RGB(160, 160, 200)),
			utils.Tone(core.RGB(120, 120, 240)),
			utils.Tone(core.RGB(120, 120, 240))
		),
		# 文字
		(
			utils.Tone(core.RGB(60, 60, 60)),
			utils.Tone(core.RGB(60, 60, 60)),
			utils.Tone(core.RGB(60, 60, 60))
		)
	)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
