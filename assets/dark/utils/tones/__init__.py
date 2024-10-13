import dark
import core

#
# トーン配列の辞書です。
#
tones = {}

#
# 指定された名前とインデックスのトーンの色を返します。
#
def get_tone_color(tone_name, tone_index):
	return tones[tone_name][tone_index].color

#
# 指定された名前のトーン配列を設定します。
#
def set_tone_array(tone_name, tone_array):
	tones[tone_name] = tone_array
