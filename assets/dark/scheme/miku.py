import dark
import core
import utils

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(80, 80, 80)),
			utils.Tone(core.RGB(60, 150, 130)),
			utils.Tone(core.RGB(60, 150, 130))
		),
		# 縁
		(
			utils.Tone(core.RGB(80, 80, 80)),
			utils.Tone(core.RGB(60, 150, 130)),
			utils.Tone(core.RGB(60, 150, 130))
		),
		# 文字
		(
			utils.Tone(core.RGB(130, 130, 130)),
			utils.Tone(core.RGB(80, 240, 80)),
			utils.Tone(core.RGB(80, 240, 80))
		)
	)
else:
	utils.create_light_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(240, 240, 240)),
			utils.Tone(core.RGB(160, 240, 200)),
			utils.Tone(core.RGB(160, 240, 200))
		),
		# 縁
		(
			utils.Tone(core.RGB(150, 150, 150)),
			utils.Tone(core.RGB(150, 200, 150)),
			utils.Tone(core.RGB(150, 200, 150))
		),
		# 文字
		(
			utils.Tone(core.RGB(120, 120, 120)),
			utils.Tone(core.RGB(0, 150, 50)),
			utils.Tone(core.RGB(0, 150, 50))
		)
	)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
