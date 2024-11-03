//
// 編集対象のファイルパスです。
// .html_localファイルが存在するフォルダからの相対パスです。
//
let c_config_file_path = '../../../patch.aul.json';

function set_bool_value(selector, value) { document.querySelector(selector).checked = value; }
function get_bool_value(selector) { return document.querySelector(selector).checked; }

function get_int_value(selector) { return document.querySelector(selector).value - 0; }
function set_int_value(selector, value) { document.querySelector(selector).value = value; }

function get_float_value(selector) { return document.querySelector(selector).value - 0; }
function set_float_value(selector, value) { document.querySelector(selector).value = value; }

function get_color_value(selector) { return document.querySelector(selector).value; }
function set_color_value(selector, value) { document.querySelector(selector).value = value; }

function get_patch_color_value(selector) { return document.querySelector(selector).value.substr(1); }
function set_patch_color_value(selector, value) { document.querySelector(selector).value = '#' + value; }

//
// 指定されたセレクタに該当するすべての要素のスタイルを設定します。
//
function set_style(selector, style, value)
{
	document.querySelectorAll(selector).forEach(element => {
		element.style[style] = value;
	});
}

//
// htmlコンテンツからjsonデータを作成して返します。
//
function get_contents_as_json()
{
	// jsonオブジェクトを作成します。
	let json = {};
	json["console"] = {};
	json["console"]["visible"] = get_bool_value('#json #console #visible');
	json["console"]["rect"] =[
		get_int_value('#json #console #rect_0'),
		get_int_value('#json #console #rect_1'),
		get_int_value('#json #console #rect_2'),
		get_int_value('#json #console #rect_3')
	];
	json["theme_cc"] = {};
	json["theme_cc"]["layer"] = {};
	json["theme_cc"]["layer"]["height_large"] = get_int_value('#height_large');
	json["theme_cc"]["layer"]["height_medium"] = get_int_value('#height_medium');
	json["theme_cc"]["layer"]["height_small"] = get_int_value('#height_small');

	const link_col = [
		get_patch_color_value('#link_col_0'),
		get_patch_color_value('#link_col_1')
	];
	json["theme_cc"]["layer"]["link_col"] = (link_col[0] != link_col[1]) ? link_col : link_col[0];

	const clipping_col = [
		get_patch_color_value('#clipping_col_0'),
		get_patch_color_value('#clipping_col_1')
	];
	json["theme_cc"]["layer"]["clipping_col"] = (clipping_col[0] != clipping_col[1]) ? clipping_col : clipping_col[0];

	json["theme_cc"]["layer"]["lock_col"] = [
		get_patch_color_value('#lock_col_0'),
		get_patch_color_value('#lock_col_1')
	];
	json["theme_cc"]["layer"]["hide_alpha"] = get_float_value('#hide_alpha');

	json["theme_cc"]["object"] = {};
	json["theme_cc"]["object"]["media_col"] = [
		get_patch_color_value('#media_col_0'),
		get_patch_color_value('#media_col_1'),
		get_patch_color_value('#media_col_2')
	];
	json["theme_cc"]["object"]["mfilter_col"] = [
		get_patch_color_value('#mfilter_col_0'),
		get_patch_color_value('#mfilter_col_1'),
		get_patch_color_value('#mfilter_col_2')
	];
	json["theme_cc"]["object"]["audio_col"] = [
		get_patch_color_value('#audio_col_0'),
		get_patch_color_value('#audio_col_1'),
		get_patch_color_value('#audio_col_2')
	];
	json["theme_cc"]["object"]["afilter_col"] = [
		get_patch_color_value('#afilter_col_0'),
		get_patch_color_value('#afilter_col_1'),
		get_patch_color_value('#afilter_col_2')
	];
	json["theme_cc"]["object"]["control_col"] = [
		get_patch_color_value('#control_col_0'),
		get_patch_color_value('#control_col_1'),
		get_patch_color_value('#control_col_2')
	];
	json["theme_cc"]["object"]["inactive_col"] = [
		get_patch_color_value('#inactive_col_0'),
		get_patch_color_value('#inactive_col_1'),
		get_patch_color_value('#inactive_col_2')
	];
	json["theme_cc"]["object"]["clipping_col"] = get_patch_color_value('#clipping_col');
	json["theme_cc"]["object"]["clipping_height"] = get_int_value('#clipping_height');
	json["theme_cc"]["object"]["disp_dialog_col"] = get_patch_color_value('#disp_dialog_col');
	json["theme_cc"]["object"]["midpt_size"] = [
		get_int_value('#midpt_size_0'),
		get_int_value('#midpt_size_1'),
		get_int_value('#midpt_size_2')
	];
	json["theme_cc"]["object"]["name_col"] = [
		get_patch_color_value('#name_col_0'),
		get_patch_color_value('#name_col_1')
	];

	json["theme_cc"]["timeline"] = {};
	json["theme_cc"]["timeline"]["scale_col"] = [
		get_patch_color_value('#json #theme_cc #timeline #scale_col_0'),
		get_patch_color_value('#json #theme_cc #timeline #scale_col_1')
	];
	json["theme_cc"]["timeline"]["bpm_grid_col"] = [
		get_patch_color_value('#json #theme_cc #timeline #bpm_grid_col_0'),
		get_patch_color_value('#json #theme_cc #timeline #bpm_grid_col_1')
	];

	json["theme_cc"]["scrollbar"] = {};
	json["theme_cc"]["scrollbar"]["vertical_thumb_min"] = get_int_value('#vertical_thumb_min');
	json["theme_cc"]["scrollbar"]["horizontal_thumb_min"] = get_int_value('#horizontal_thumb_min');

	json["redo"] = {};
	json["redo"]["shift"] = get_bool_value('#redo_shift');

	json["fast_exeditwindow"] = {};
	json["fast_exeditwindow"]["step"] = get_int_value('#fast_exeditwindow_step');

	json["fast_text"] = {};
	json["fast_text"]["release_time"] = get_int_value('#fast_text_release_time');

	json["fast_border"] = {};
	json["fast_border"]["use_space"] = get_int_value('#fast_border_use_space');

	json["scene_cache"] = {};
	json["scene_cache"]["threshold_time"] = get_int_value('#scene_cache_threshold_time');

	json["scene_vram"] = {};
	json["scene_vram"]["patch_level"] = get_int_value('#scene_vram_patch_level');

	json["switch"] = {};
	json["switch"]["keyconfig"] = get_bool_value('#switch_keyconfig');
	json["switch"]["access_key"] = get_bool_value('#switch_access_key');
	json["switch"]["colorpalette_cache"] = get_bool_value('#switch_colorpalette_cache');
	json["switch"]["fileinfo"] = get_bool_value('#switch_fileinfo');
	json["switch"]["aup_layer_setting"] = get_bool_value('#switch_aup_layer_setting');
	json["switch"]["aup_scene_setting"] = get_bool_value('#switch_aup_scene_setting');
	json["switch"]["exo_aviutlfilter"] = get_bool_value('#switch_exo_aviutlfilter');
	json["switch"]["exo_sceneidx"] = get_bool_value('#switch_exo_sceneidx');
	json["switch"]["exo_trackparam"] = get_bool_value('#switch_exo_trackparam');
//	json["switch"]["exo_track_minusval"] = get_bool_value('#switch_exo_track_minusval');
	json["switch"]["exo_midpt_and_tra"] = get_bool_value('#switch_exo_midpt_and_tra');
	json["switch"]["exo_specialcolorconv"] = get_bool_value('#switch_exo_specialcolorconv');
	json["switch"]["exa_fold_gui"] = get_bool_value('#switch_exa_fold_gui');
	json["switch"]["str_minusval"] = get_bool_value('#switch_str_minusval');
	json["switch"]["tra_aviutlfilter"] = get_bool_value('#switch_tra_aviutlfilter');
	json["switch"]["tra_change_drawfilter"] = get_bool_value('#switch_tra_change_drawfilter');
	json["switch"]["tra_specified_speed"] = get_bool_value('#switch_tra_specified_speed');
	json["switch"]["text_op_size"] = get_bool_value('#switch_text_op_size');
	json["switch"]["ignore_media_param_reset"] = get_bool_value('#switch_ignore_media_param_reset');
	json["switch"]["failed_sjis_msgbox"] = get_bool_value('#switch_failed_sjis_msgbox');
	json["switch"]["failed_longer_path"] = get_bool_value('#switch_failed_longer_path');
	json["switch"]["failed_file_drop"] = get_bool_value('#switch_failed_file_drop');
	json["switch"]["theme_cc"] = get_bool_value('#switch_theme_cc');
	json["switch"]["exeditwindow_sizing"] = get_bool_value('#switch_exeditwindow_sizing');
	json["switch"]["settingdialog_move"] = get_bool_value('#switch_settingdialog_move');
	json["switch"]["obj_text"] = get_bool_value('#switch_obj_text');
	json["switch"]["obj_audiofile"] = get_bool_value('#switch_obj_audiofile');
	json["switch"]["obj_colorcorrection"] = get_bool_value('#switch_obj_colorcorrection');
	json["switch"]["obj_glow"] = get_bool_value('#switch_obj_glow');
	json["switch"]["obj_sharp"] = get_bool_value('#switch_obj_sharp');
	json["switch"]["obj_mask"] = get_bool_value('#switch_obj_mask');
	json["switch"]["obj_rotation"] = get_bool_value('#switch_obj_rotation');
	json["switch"]["obj_resize"] = get_bool_value('#switch_obj_resize');
	json["switch"]["obj_lensblur"] = get_bool_value('#switch_obj_lensblur');
	json["switch"]["obj_imageloop"] = get_bool_value('#switch_obj_imageloop');
	json["switch"]["obj_noise"] = get_bool_value('#switch_obj_noise');
	json["switch"]["obj_specialcolorconv"] = get_bool_value('#switch_obj_specialcolorconv');
	json["switch"]["obj_portionfilter"] = get_bool_value('#switch_obj_portionfilter');
	json["switch"]["settingdialog_excolorconfig"] = get_bool_value('#switch_settingdialog_excolorconfig');
	json["switch"]["settingdialog_chromakey"] = get_bool_value('#switch_settingdialog_chromakey');
	json["switch"]["r_click_menu_split"] = get_bool_value('#switch_r_click_menu_split');
	json["switch"]["blend"] = get_bool_value('#switch_blend');
	json["switch"]["add_extension"] = get_bool_value('#switch_add_extension');
	json["switch"]["new_project_editbox"] = get_bool_value('#switch_new_project_editbox');
	json["switch"]["scene_cache"] = get_bool_value('#switch_scene_cache');
	json["switch"]["playback_speed"] = get_bool_value('#switch_playback_speed');
	json["switch"]["setting_new_project"] = get_bool_value('#switch_setting_new_project');
	json["switch"]["shared_cache"] = get_bool_value('#switch_shared_cache');
	json["switch"]["warning_duplicate"] = get_bool_value('#switch_warning_duplicate');
	json["switch"]["rendering"] = get_bool_value('#switch_rendering');
	json["switch"]["yc_rgb_conv"] = get_bool_value('#switch_yc_rgb_conv');
	json["switch"]["camera_scene"] = get_bool_value('#switch_camera_scene');
	json["switch"]["group_camera_scene"] = get_bool_value('#switch_group_camera_scene');
	json["switch"]["change_disp_scene"] = get_bool_value('#switch_change_disp_scene');
	json["switch"]["right_trackbar"] = get_bool_value('#switch_right_trackbar');
	json["switch"]["group_id"] = get_bool_value('#switch_group_id');
	json["switch"]["borderonly_text"] = get_bool_value('#switch_borderonly_text');
	json["switch"]["patch_script_sort"] = get_bool_value('#switch_patch_script_sort');
	json["switch"]["undo"] = get_bool_value('#switch_undo');
	json["switch"]["undo.redo"] = get_bool_value('#switch_undo_redo');
	json["switch"]["console"] = get_bool_value('#switch_console');
	json["switch"]["console.escape"] = get_bool_value('#switch_console_escape');
	json["switch"]["console.input"] = get_bool_value('#switch_console_input');
	json["switch"]["console.debug_string"] = get_bool_value('#switch_console_debug_string');
	json["switch"]["console.debug_string.time"] = get_bool_value('#switch_console_debug_string_time');
	json["switch"]["lua"] = get_bool_value('#switch_lua');
	json["switch"]["lua.env"] = get_bool_value('#switch_lua_env');
	json["switch"]["lua.path"] = get_bool_value('#switch_lua_path');
	json["switch"]["lua.getvalue"] = get_bool_value('#switch_lua_getvalue');
	json["switch"]["lua.rand"] = get_bool_value('#switch_lua_rand');
	json["switch"]["lua.randex"] = get_bool_value('#switch_lua_randex');
	json["switch"]["fast"] = get_bool_value('#switch_fast');
	json["switch"]["fast.exeditwindow"] = get_bool_value('#switch_fast_exeditwindow');
	json["switch"]["fast.settingdialog"] = get_bool_value('#switch_fast_settingdialog');
	json["switch"]["fast.text"] = get_bool_value('#switch_fast_text');
	json["switch"]["fast.textborder"] = get_bool_value('#switch_fast_textborder');
	json["switch"]["fast.create_figure"] = get_bool_value('#switch_fast_create_figure');
	json["switch"]["fast.border"] = get_bool_value('#switch_fast_border');
	json["switch"]["fast.blur"] = get_bool_value('#switch_fast_blur');
	json["switch"]["fast.glow"] = get_bool_value('#switch_fast_glow');
	json["switch"]["fast.colorkey"] = get_bool_value('#switch_fast_colorkey');
	json["switch"]["fast.chromakey"] = get_bool_value('#switch_fast_chromakey');
	json["switch"]["fast.cl"] = get_bool_value('#switch_fast_cl');
	json["switch"]["fast.radiationalblur"] = get_bool_value('#switch_fast_radiationalblur');
	json["switch"]["fast.polartransform"] = get_bool_value('#switch_fast_polartransform');
	json["switch"]["fast.displacementmap"] = get_bool_value('#switch_fast_displacementmap');
	json["switch"]["fast.flash"] = get_bool_value('#switch_fast_flash');
	json["switch"]["fast.directionalblur"] = get_bool_value('#switch_fast_directionalblur');
	json["switch"]["fast.lensblur"] = get_bool_value('#switch_fast_lensblur');

	return json;
}

//
// jsonデータでhtmlコンテンツを更新します。
//
function set_contents_as_json(json)
{
	if (json.console)
	{
		// console

		set_bool_value('#json #console #visible', json.console.visible);

		if (json.console.rect)
		{
			set_int_value('#json #console #rect_0', json.console.rect[0]);
			set_int_value('#json #console #rect_1', json.console.rect[1]);
			set_int_value('#json #console #rect_2', json.console.rect[2]);
			set_int_value('#json #console #rect_3', json.console.rect[3]);
		}
	}

	if (json['theme_cc'])
	{
		if (json['theme_cc']['layer'])
		{
			set_int_value('#height_large', json['theme_cc']['layer']['height_large']);
			set_int_value('#height_medium', json['theme_cc']['layer']['height_medium']);
			set_int_value('#height_small', json['theme_cc']['layer']['height_small']);

			if (Array.isArray(json['theme_cc']['layer']['link_col']))
			{
				set_patch_color_value('#link_col_0', json['theme_cc']['layer']['link_col'][0]);
				set_patch_color_value('#link_col_1', json['theme_cc']['layer']['link_col'][1]);
			}
			else
			{
				set_patch_color_value('#link_col_0', json['theme_cc']['layer']['link_col']);
				set_patch_color_value('#link_col_1', json['theme_cc']['layer']['link_col']);
			}

			if (Array.isArray(json['theme_cc']['layer']['clipping_col']))
			{
				set_patch_color_value('#clipping_col_0', json['theme_cc']['layer']['clipping_col'][0]);
				set_patch_color_value('#clipping_col_1', json['theme_cc']['layer']['clipping_col'][1]);
			}
			else
			{
				set_patch_color_value('#clipping_col_0', json['theme_cc']['layer']['clipping_col']);
				set_patch_color_value('#clipping_col_1', json['theme_cc']['layer']['clipping_col']);
			}

			set_patch_color_value('#lock_col_0', json['theme_cc']['layer']['lock_col'][0]);
			set_patch_color_value('#lock_col_1', json['theme_cc']['layer']['lock_col'][1]);

			set_float_value('#hide_alpha', json['theme_cc']['layer']['hide_alpha']);
		}

		if (json['theme_cc']['object'])
		{
			set_patch_color_value('#media_col_0', json['theme_cc']['object']['media_col'][0]);
			set_patch_color_value('#media_col_1', json['theme_cc']['object']['media_col'][1]);
			set_patch_color_value('#media_col_2', json['theme_cc']['object']['media_col'][2]);
			set_patch_color_value('#mfilter_col_0', json['theme_cc']['object']['mfilter_col'][0]);
			set_patch_color_value('#mfilter_col_1', json['theme_cc']['object']['mfilter_col'][1]);
			set_patch_color_value('#mfilter_col_2', json['theme_cc']['object']['mfilter_col'][2]);
			set_patch_color_value('#audio_col_0', json['theme_cc']['object']['audio_col'][0]);
			set_patch_color_value('#audio_col_1', json['theme_cc']['object']['audio_col'][1]);
			set_patch_color_value('#audio_col_2', json['theme_cc']['object']['audio_col'][2]);
			set_patch_color_value('#afilter_col_0', json['theme_cc']['object']['afilter_col'][0]);
			set_patch_color_value('#afilter_col_1', json['theme_cc']['object']['afilter_col'][1]);
			set_patch_color_value('#afilter_col_2', json['theme_cc']['object']['afilter_col'][2]);
			set_patch_color_value('#control_col_0', json['theme_cc']['object']['control_col'][0]);
			set_patch_color_value('#control_col_1', json['theme_cc']['object']['control_col'][1]);
			set_patch_color_value('#control_col_2', json['theme_cc']['object']['control_col'][2]);
			set_patch_color_value('#inactive_col_0', json['theme_cc']['object']['inactive_col'][0]);
			set_patch_color_value('#inactive_col_1', json['theme_cc']['object']['inactive_col'][1]);
			set_patch_color_value('#inactive_col_2', json['theme_cc']['object']['inactive_col'][2]);
			set_patch_color_value('#clipping_col', json['theme_cc']['object']['clipping_col']);
			set_int_value('#clipping_height', json['theme_cc']['object']['clipping_height']);
			set_patch_color_value('#disp_dialog_col', json['theme_cc']['object']['disp_dialog_col']);

			if (json['theme_cc']['object']['midpt_size'])
			{
				set_int_value('#midpt_size_0', json['theme_cc']['object']['midpt_size'][0]);
				set_int_value('#midpt_size_1', json['theme_cc']['object']['midpt_size'][1]);
				set_int_value('#midpt_size_2', json['theme_cc']['object']['midpt_size'][2]);
			}

			set_patch_color_value('#name_col_0', json['theme_cc']['object']['name_col'][0]);
			set_patch_color_value('#name_col_1', json['theme_cc']['object']['name_col'][1]);
		}

		if (json['theme_cc']['timeline'])
		{
			set_patch_color_value('#scale_col_0', json['theme_cc']['timeline']['scale_col'][0]);
			set_patch_color_value('#scale_col_1', json['theme_cc']['timeline']['scale_col'][1]);
			set_patch_color_value('#bpm_grid_col_0', json['theme_cc']['timeline']['bpm_grid_col'][0]);
			set_patch_color_value('#bpm_grid_col_1', json['theme_cc']['timeline']['bpm_grid_col'][1]);
		}

		if (json['theme_cc']['scrollbar'])
		{
			set_int_value('#vertical_thumb_min', json['theme_cc']['scrollbar']['vertical_thumb_min']);
			set_int_value('#horizontal_thumb_min', json['theme_cc']['scrollbar']['horizontal_thumb_min']);
		}
	}

	if (json['redo']) {
		set_bool_value('#redo_shift', json['redo']['shift']);
	}

	if (json['fast_exeditwindow']) {
		set_int_value('#fast_exeditwindow_step', json['fast_exeditwindow']['step']);
	}

	if (json['fast_text']) {
		set_int_value('#fast_text_release_time', json['fast_text']['release_time']);
	}

	if (json['fast_border']) {
		set_int_value('#fast_border_use_space', json['fast_border']['use_space']);
	}

	if (json['scene_cache']) {
		set_int_value('#scene_cache_threshold_time', json['scene_cache']['threshold_time']);
	}

	if (json['scene_vram']) {
		set_int_value('#scene_vram_patch_level', json['scene_vram']['patch_level']);
	}

	if (json['switch']) {
		set_bool_value('#switch_keyconfig', json['switch']['keyconfig']);
		set_bool_value('#switch_access_key', json['switch']['access_key']);
		set_bool_value('#switch_colorpalette_cache', json['switch']['colorpalette_cache']);
		set_bool_value('#switch_fileinfo', json['switch']['fileinfo']);
		set_bool_value('#switch_aup_layer_setting', json['switch']['aup_layer_setting']);
		set_bool_value('#switch_aup_scene_setting', json['switch']['aup_scene_setting']);
		set_bool_value('#switch_exo_aviutlfilter', json['switch']['exo_aviutlfilter']);
		set_bool_value('#switch_exo_sceneidx', json['switch']['exo_sceneidx']);
		set_bool_value('#switch_exo_trackparam', json['switch']['exo_trackparam']);
//		set_bool_value('#switch_exo_track_minusval', json['switch']['exo_track_minusval']);
		set_bool_value('#switch_exo_midpt_and_tra', json['switch']['exo_midpt_and_tra']);
		set_bool_value('#switch_exo_specialcolorconv', json['switch']['exo_specialcolorconv']);
		set_bool_value('#switch_exa_fold_gui', json['switch']['exa_fold_gui']);
		set_bool_value('#switch_str_minusval', json['switch']['str_minusval']);
		set_bool_value('#switch_tra_aviutlfilter', json['switch']['tra_aviutlfilter']);
		set_bool_value('#switch_tra_change_drawfilter', json['switch']['tra_change_drawfilter']);
		set_bool_value('#switch_tra_specified_speed', json['switch']['tra_specified_speed']);
		set_bool_value('#switch_text_op_size', json['switch']['text_op_size']);
		set_bool_value('#switch_ignore_media_param_reset', json['switch']['ignore_media_param_reset']);
		set_bool_value('#switch_failed_sjis_msgbox', json['switch']['failed_sjis_msgbox']);
		set_bool_value('#switch_failed_longer_path', json['switch']['failed_longer_path']);
		set_bool_value('#switch_failed_file_drop', json['switch']['failed_file_drop']);
		set_bool_value('#switch_theme_cc', json['switch']['theme_cc']);
		set_bool_value('#switch_exeditwindow_sizing', json['switch']['exeditwindow_sizing']);
		set_bool_value('#switch_settingdialog_move', json['switch']['settingdialog_move']);
		set_bool_value('#switch_obj_text', json['switch']['obj_text']);
		set_bool_value('#switch_obj_audiofile', json['switch']['obj_audiofile']);
		set_bool_value('#switch_obj_colorcorrection', json['switch']['obj_colorcorrection']);
		set_bool_value('#switch_obj_glow', json['switch']['obj_glow']);
		set_bool_value('#switch_obj_sharp', json['switch']['obj_sharp']);
		set_bool_value('#switch_obj_mask', json['switch']['obj_mask']);
		set_bool_value('#switch_obj_rotation', json['switch']['obj_rotation']);
		set_bool_value('#switch_obj_resize', json['switch']['obj_resize']);
		set_bool_value('#switch_obj_lensblur', json['switch']['obj_lensblur']);
		set_bool_value('#switch_obj_imageloop', json['switch']['obj_imageloop']);
		set_bool_value('#switch_obj_noise', json['switch']['obj_noise']);
		set_bool_value('#switch_obj_specialcolorconv', json['switch']['obj_specialcolorconv']);
		set_bool_value('#switch_obj_portionfilter', json['switch']['obj_portionfilter']);
		set_bool_value('#switch_settingdialog_excolorconfig', json['switch']['settingdialog_excolorconfig']);
		set_bool_value('#switch_settingdialog_chromakey', json['switch']['settingdialog_chromakey']);
		set_bool_value('#switch_r_click_menu_split', json['switch']['r_click_menu_split']);
		set_bool_value('#switch_blend', json['switch']['blend']);
		set_bool_value('#switch_add_extension', json['switch']['add_extension']);
		set_bool_value('#switch_new_project_editbox', json['switch']['new_project_editbox']);
		set_bool_value('#switch_scene_cache', json['switch']['scene_cache']);
		set_bool_value('#switch_playback_speed', json['switch']['playback_speed']);
		set_bool_value('#switch_setting_new_project', json['switch']['setting_new_project']);
		set_bool_value('#switch_shared_cache', json['switch']['shared_cache']);
		set_bool_value('#switch_warning_duplicate', json['switch']['warning_duplicate']);
		set_bool_value('#switch_rendering', json['switch']['rendering']);
		set_bool_value('#switch_yc_rgb_conv', json['switch']['yc_rgb_conv']);
		set_bool_value('#switch_camera_scene', json['switch']['camera_scene']);
		set_bool_value('#switch_group_camera_scene', json['switch']['group_camera_scene']);
		set_bool_value('#switch_change_disp_scene', json['switch']['change_disp_scene']);
		set_bool_value('#switch_right_trackbar', json['switch']['right_trackbar']);
		set_bool_value('#switch_group_id', json['switch']['group_id']);
		set_bool_value('#switch_borderonly_text', json['switch']['borderonly_text']);
		set_bool_value('#switch_patch_script_sort', json['switch']['patch_script_sort']);
		set_bool_value('#switch_undo', json['switch']['undo']);
		set_bool_value('#switch_undo_redo', json['switch']['undo.redo']);
		set_bool_value('#switch_console', json['switch']['console']);
		set_bool_value('#switch_console_escape', json['switch']['console.escape']);
		set_bool_value('#switch_console_input', json['switch']['console.input']);
		set_bool_value('#switch_console_debug_string', json['switch']['console.debug_string']);
		set_bool_value('#switch_console_debug_string_time', json['switch']['console.debug_string.time']);
		set_bool_value('#switch_lua', json['switch']['lua']);
		set_bool_value('#switch_lua_env', json['switch']['lua.env']);
		set_bool_value('#switch_lua_path', json['switch']['lua.path']);
		set_bool_value('#switch_lua_getvalue', json['switch']['lua.getvalue']);
		set_bool_value('#switch_lua_rand', json['switch']['lua.rand']);
		set_bool_value('#switch_lua_randex', json['switch']['lua.randex']);
		set_bool_value('#switch_fast', json['switch']['fast']);
		set_bool_value('#switch_fast_exeditwindow', json['switch']['fast.exeditwindow']);
		set_bool_value('#switch_fast_settingdialog', json['switch']['fast.settingdialog']);
		set_bool_value('#switch_fast_text', json['switch']['fast.text']);
		set_bool_value('#switch_fast_textborder', json['switch']['fast.textborder']);
		set_bool_value('#switch_fast_create_figure', json['switch']['fast.create_figure']);
		set_bool_value('#switch_fast_border', json['switch']['fast.border']);
		set_bool_value('#switch_fast_blur', json['switch']['fast.blur']);
		set_bool_value('#switch_fast_glow', json['switch']['fast.glow']);
		set_bool_value('#switch_fast_colorkey', json['switch']['fast.colorkey']);
		set_bool_value('#switch_fast_chromakey', json['switch']['fast.chromakey']);
		set_bool_value('#switch_fast_cl', json['switch']['fast.cl']);
		set_bool_value('#switch_fast_radiationalblur', json['switch']['fast.radiationalblur']);
		set_bool_value('#switch_fast_polartransform', json['switch']['fast.polartransform']);
		set_bool_value('#switch_fast_displacementmap', json['switch']['fast.displacementmap']);
		set_bool_value('#switch_fast_flash', json['switch']['fast.flash']);
		set_bool_value('#switch_fast_directionalblur', json['switch']['fast.directionalblur']);
		set_bool_value('#switch_fast_lensblur', json['switch']['fast.lensblur']);
	}
}

//
// プレビューを更新します。
//
function refresh_preview()
{
	//
	// グラデーション値を返します。
	//
	function get_gradient_value(selector)
	{
		return `linear-gradient(to right, ${get_color_value(selector + '_0')}, ${get_color_value(selector + '_1')})`;
	}

	//
	// RGBをカラー値に変換して返します。
	//
	function rgb_to_color_value(rgb)
	{
		function to_hex(v) { return ('00' + v.toString(16)).substr(-2); }

		return '#' + to_hex(rgb.r) + to_hex(rgb.g) + to_hex(rgb.b);
	}

	// プレビューにタイムラインの高さを適用します。
	const height = Math.round(get_int_value('#height_medium'));
	set_style('.preview > div > div', 'font-size', height - 4 + 'px');
	set_style('.preview > div > div > span', 'margin-top', '4px');
//	set_style('.preview > div > div > span', 'font-size', height - 4 + 'px');
//	set_style('.preview > div > div > span', 'line-height', '100%');

	// プレビューにオブジェクトの文字色を適用します。
	set_style('.preview .active-layer', 'color', get_color_value('#name_col_0'));
	set_style('.preview .active-layer .layer-title', 'color', '#000');
	set_style('.preview .inactive-layer', 'color', get_color_value('#name_col_1'));
	set_style('.preview .inactive-layer', 'color', get_color_value('#name_col_1'));

	// プレビューにオブジェクトの背景色を適用します。
	set_style('.preview .media-object span', 'background', get_gradient_value('#media_col'));
	set_style('.preview .audio-object span', 'background', get_gradient_value('#audio_col'));
	set_style('.preview .control-object span', 'background', get_gradient_value('#control_col'));
	set_style('.preview .selected-media-object span', 'background-color', get_color_value('#media_col_2'));
	set_style('.preview .selected-audio-object span', 'background-color', get_color_value('#audio_col_2'));
	set_style('.preview .selected-control-object span', 'background-color', get_color_value('#control_col_2'));
	set_style('.preview .mfilter-object span', 'background', get_gradient_value('#mfilter_col'));
	set_style('.preview .afilter-object span', 'background', get_gradient_value('#afilter_col'));
	set_style('.preview .inactive-object span', 'background', get_gradient_value('#inactive_col'));
	set_style('.preview .selected-mfilter-object span', 'background-color', get_color_value('#mfilter_col_2'));
	set_style('.preview .selected-afilter-object span', 'background-color', get_color_value('#afilter_col_2'));
	set_style('.preview .selected-inactive-object span', 'background-color', get_color_value('#inactive_col_2'));

	// プレビューにタイムラインの背景色を適用します。
	const hide_alpha = get_float_value('#hide_alpha');
	const base = { 'r': 0xf0, 'g': 0xf0, 'b': 0xf0 };
	const base_string = rgb_to_color_value(base);
	const inactive = {
		'r': base.r * hide_alpha,
		'g': base.g * hide_alpha,
		'b': base.b * hide_alpha
	};
	const inactive_string = rgb_to_color_value(inactive);
	set_style('.preview .active-layer', 'background-color', base_string);
	set_style('.preview .inactive-layer', 'background-color', inactive_string);

	// プレビューに透明度を適用します。
//	set_style('.preview .inactive-layer', 'opacity', get_float_value('#hide_alpha'));
}

//
// 初期化処理を実行します。
//
window.addEventListener('load', event => {
	// プレビュー用のイベントリスナーを追加します。
	document.querySelectorAll('input').forEach(v => v.addEventListener('input', event => {
		// プレビューを更新します。
		refresh_preview();
	}));
	// 読み込み用のイベントリスナーを追加します。
	document.getElementById('read_button').addEventListener('click', event => {
		// 読み込みリクエスト用のjsonを構築します。
		const json = {
			"request_read_json": {
				"path": c_config_file_path
			}
		};
		// ホストにファイルの読み込みをリクエストします。
		window.chrome.webview.postMessage(json);
	});
	// 書き込み用のイベントリスナーを追加します。
	document.getElementById('write_button').addEventListener('click', event => {
		// コンテンツのjsonを取得します。
		const data = get_contents_as_json();
		// 書き込みリクエスト用のjsonを構築します。
		const json = {
			"request_write_json": {
				"path": c_config_file_path,
				"data": data
			}
		};
		// ホストにファイルの書き込みをリクエストします。
		window.chrome.webview.postMessage(json);
	});
	// リセット用のイベントリスナーを追加します。
	document.getElementById('reset_button').addEventListener('click', event => {
		// すべてのinputを初期値に戻します。
		document.getElementById('form').reset();
		// プレビューを更新します。
		refresh_preview();
		// ステータスを更新します。
		document.getElementById('status').innerHTML = `コンテンツをリセットしました`;
	});
	// ホストが存在する場合は
	if (window.chrome && window.chrome.webview) {
		// メッセージ用のイベントリスナーを追加します。
		window.chrome.webview.addEventListener('message', event => {
			// 読み込みレスポンスの場合は
			if ('response_read_json' in event.data) {
				// レスポンスデータ(json)を取得します。
				const json = event.data['response_read_json'];
				if (json['path'] == c_config_file_path) {
					// ホストから受け取ったデータでコンテンツを更新します。
					set_contents_as_json(event.data['response_read_json']['data']);
					// プレビューを更新します。
					refresh_preview();
					// ステータスを更新します。
					document.getElementById('status').innerHTML = `${json['abs_path']}を読み込みました`;
				}
			}
			// 書き込みレスポンスの場合は
			if ('response_write_json' in event.data) {
				// レスポンスデータ(json)を取得します。
				const json = event.data['response_write_json'];
				if (json['path'] == c_config_file_path) {
					// ステータスを更新します。
					document.getElementById('status').innerHTML = `${json['abs_path']}に書き込みました`;
				}
			}
		});
		// ホストにコンフィグファイルの読み込みをリクエストします。
		document.getElementById('read_button').dispatchEvent(new Event('click'));
	}
});
