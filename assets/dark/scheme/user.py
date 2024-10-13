import dark
import core
import utils

#
# 初期化関数です。
# この関数はダークモジュールから呼ばれるAPIです。
#
def dark_init(args):
	if (dark.hive.dark_mode):
		utils.create_dark_tone_stuffs(
			# 塗り潰し
			(
				utils.Tone(args.dark_color[0].fill),
				utils.Tone(args.dark_color[1].fill),
				utils.Tone(args.dark_color[2].fill)
			),
			# 縁
			(
				utils.Tone(args.dark_color[0].border),
				utils.Tone(args.dark_color[1].border),
				utils.Tone(args.dark_color[2].border)
			),
			# 文字
			(
				utils.Tone(args.dark_color[0].text),
				utils.Tone(args.dark_color[1].text),
				utils.Tone(args.dark_color[2].text)
			)
		)
	else:
		utils.create_light_tone_stuffs(
			# 塗り潰し
			(
				utils.Tone(args.light_color[0].fill),
				utils.Tone(args.light_color[1].fill),
				utils.Tone(args.light_color[2].fill)
			),
			# 縁
			(
				utils.Tone(args.light_color[0].border),
				utils.Tone(args.light_color[1].border),
				utils.Tone(args.light_color[2].border)
			),
			# 文字
			(
				utils.Tone(args.light_color[0].text),
				utils.Tone(args.light_color[1].text),
				utils.Tone(args.light_color[2].text)
			)
		)

#
# 指定された名前のスタッフを返します。
# この関数はスキンモジュールから呼ばれるAPIです。
#
def dark_get_stuff(stuff_name):
	return utils.get_stuff(stuff_name)
