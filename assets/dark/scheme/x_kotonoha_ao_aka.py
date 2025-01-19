import dark
import core
import utils

if (dark.hive.dark_mode):
	utils.create_dark_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(100, 150, 250)),
			utils.Tone(core.RGB(250, 100, 150)),
			utils.Tone(core.RGB(100, 150, 250))
		),
		# 縁
		(
			utils.Tone(core.RGB(100, 150, 250)),
			utils.Tone(core.RGB(250, 100, 150)),
			utils.Tone(core.RGB(100, 150, 250))
		),
		# 文字
		(
			utils.Tone(core.RGB(100, 150, 250)),
			utils.Tone(core.RGB(250, 250, 250)),
			utils.Tone(core.RGB(100, 150, 250))
		)
	)
else:
	utils.create_light_tone_stuffs(
		# 塗り潰し
		(
			utils.Tone(core.RGB(100, 150, 250)),
			utils.Tone(core.RGB(250, 150, 200)),
			utils.Tone(core.RGB(100, 150, 250))
		),
		# 縁
		(
			utils.Tone(core.RGB(100, 150, 250)),
			utils.Tone(core.RGB(250, 100, 150)),
			utils.Tone(core.RGB(100, 150, 250))
		),
		# 文字
		(
			utils.Tone(core.RGB(100, 150, 250)),
			utils.Tone(core.RGB(0, 50, 150)),
			utils.Tone(core.RGB(100, 150, 250))
		)
	)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
