#pragma once
#include "resource.h"
#include "Hive.h"

namespace fgo::audio_graph_gui
{
	struct ConfigDialog : Tools::Dialog2
	{
		inline static const LPCTSTR PROP_NAME_COLOR_BUTTON = _T("ColorButton");

		inline static BOOL isColorButton(HWND control)
		{
			return !!::GetProp(control, PROP_NAME_COLOR_BUTTON);
		}

		inline static BOOL initColorButton(HWND dialog, UINT id)
		{
			HWND control = ::GetDlgItem(dialog, id);

			return ::SetProp(control, PROP_NAME_COLOR_BUTTON, (HANDLE)1);
		}

		inline static BOOL initLineStyleComboBox(HWND dialog, UINT id)
		{
			HWND control = ::GetDlgItem(dialog, id);

			ComboBox_AddString(control, _T("通常"));
			ComboBox_AddString(control, _T("ソリッド"));
			ComboBox_AddString(control, _T("ダッシュ"));
			ComboBox_AddString(control, _T("ドット"));
			ComboBox_AddString(control, _T("グロー"));

			return TRUE;
		}

		inline static BOOL browse(HWND hwnd, UINT id, LPCTSTR filter)
		{
			TCHAR fileName[MAX_PATH] = {};
			::GetDlgItemText(hwnd, id, fileName, MAX_PATH);

			OPENFILENAME ofn = { sizeof(ofn) };
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.hwndOwner = hwnd;
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;

			if (!::GetOpenFileName(&ofn))
				return FALSE;

			::SetDlgItemText(hwnd, id, fileName);

			return TRUE;
		}

		ConfigDialog(HWND parent)
		{
			constexpr LPCTSTR floatFormat = _T("%.3f");

			create(hive.instance, MAKEINTRESOURCE(IDD_CONFIG), parent);

			setText(IDC_FONT_DEFAULT, design.fontDefault);
			setText(IDC_FONT_DEFAULT_2, design.fontDefault2);
			setText(IDC_IMAGE_FILE_NAME, design.image.fileName);
			setInt(IDC_IMAGE_X, design.image.offset.x);
			setInt(IDC_IMAGE_Y, design.image.offset.y);
			setFloat(IDC_IMAGE_SCALE, design.image.scale, floatFormat);
			setFloat(IDC_IMAGE_ANGLE, design.image.angle, floatFormat);
			setFloat(IDC_IMAGE_ALPHA, design.image.alpha, floatFormat);
			setUInt(IDC_BACKGROUND_FILL_START, design.background.fill.start);
			setUInt(IDC_BACKGROUND_FILL_END, design.background.fill.end);
			setUInt(IDC_VOLUME_LEVEL_FILL_START, design.volume.level.fill.start);
			setUInt(IDC_VOLUME_LEVEL_FILL_END, design.volume.level.fill.end);
			setUInt(IDC_VOLUME_PEAK_STROKE_COLOR, design.volume.peak.stroke.color);
			setUInt(IDC_VOLUME_PEAK_STROKE_WIDTH, design.volume.peak.stroke.width);
			setComboBox(IDC_VOLUME_PEAK_STROKE_STYLE, design.volume.peak.stroke.style);
			setUInt(IDC_VOLUME_ZEBRA_STROKE_COLOR, design.volume.zebra.stroke.color);
			setUInt(IDC_VOLUME_ZEBRA_STROKE_WIDTH, design.volume.zebra.stroke.width);
			setComboBox(IDC_VOLUME_ZEBRA_STROKE_STYLE, design.volume.zebra.stroke.style);
			setUInt(IDC_VOLUME_ZEBRA_STEP, design.volume.zebra.step);
			setUInt(IDC_SCALE_STROKE_COLOR, design.scale.stroke.color);
			setUInt(IDC_SCALE_STROKE_WIDTH, design.scale.stroke.width);
			setComboBox(IDC_SCALE_STROKE_STYLE, design.scale.stroke.style);
			setFloat(IDC_SCALE_TEXT_DILATE, design.scale.text.dilate, floatFormat);
			setFloat(IDC_SCALE_TEXT_BLUR, design.scale.text.blur, floatFormat);
			setUInt(IDC_SCALE_TEXT_COLOR, design.scale.text.color);
			setInt(IDC_SCALE_TEXT_HEIGHT, design.scale.text.height);
			setUInt(IDC_SCALE_TEXT_SHADOW_COLOR, design.scale.text.shadow.color);
			setFloat(IDC_SCALE_TEXT_SHADOW_DILATE, design.scale.text.shadow.dilate, floatFormat);
			setFloat(IDC_SCALE_TEXT_SHADOW_BLUR, design.scale.text.shadow.blur, floatFormat);
			setInt(IDC_SCALE_TEXT_SHADOW_OFFSET_X, design.scale.text.shadow.offset.x);
			setInt(IDC_SCALE_TEXT_SHADOW_OFFSET_Y, design.scale.text.shadow.offset.y);
			setInt(IDC_MIN_RANGE, hive.minRange);
			setInt(IDC_MAX_RANGE, hive.maxRange);
		}

		INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("");

					initColorButton(*this, IDC_BACKGROUND_FILL_START);
					initColorButton(*this, IDC_BACKGROUND_FILL_END);

					initColorButton(*this, IDC_VOLUME_LEVEL_FILL_START);
					initColorButton(*this, IDC_VOLUME_LEVEL_FILL_END);

					initColorButton(*this, IDC_VOLUME_PEAK_STROKE_COLOR);
					initLineStyleComboBox(*this, IDC_VOLUME_PEAK_STROKE_STYLE);

					initColorButton(*this, IDC_VOLUME_ZEBRA_STROKE_COLOR);
					initLineStyleComboBox(*this, IDC_VOLUME_ZEBRA_STROKE_STYLE);

					initColorButton(*this, IDC_SCALE_STROKE_COLOR);
					initLineStyleComboBox(*this, IDC_SCALE_STROKE_STYLE);

					initColorButton(*this, IDC_SCALE_TEXT_COLOR);
					initColorButton(*this, IDC_SCALE_TEXT_SHADOW_COLOR);

					break;
				}
			case WM_COMMAND:
				{
					UINT id = LOWORD(wParam);
					HWND control = (HWND)lParam;

					switch (id)
					{
					case IDC_FONT_DEFAULT_BROWSE:
					case IDC_FONT_DEFAULT_2_BROWSE:
						{
							browse(hwnd, id - 1,
								_T("Font Files {*.ttf;*.ttc;*.otf}\0*.ttf;*.ttc;*.otf\0")
								_T("All Files {*.*}\0*.*\0"));

							return TRUE;
						}
					case IDC_IMAGE_FILE_NAME_BROWSE:
						{
							browse(hwnd, IDC_IMAGE_FILE_NAME,
								_T("PNG Files {*.png}\0*.png\0")
								_T("All Files {*.*}\0*.*\0"));

							return TRUE;
						}
					}

					if (isColorButton(control))
					{
						DWORD rgba = ::GetDlgItemInt(hwnd, id, 0, FALSE);
						COLORREF color = Tools::MyColor::getCOLORREF(rgba);

						static COLORREF customColors[16] = {};
						CHOOSECOLOR cc { sizeof(cc) };
						cc.hwndOwner = hwnd;
						cc.lpCustColors = customColors;
						cc.rgbResult = color;
						cc.Flags = CC_RGBINIT | CC_FULLOPEN;
						if (!::ChooseColor(&cc)) return TRUE;

						color = Tools::MyColor::getDWORD(cc.rgbResult, rgba);
						::SetDlgItemInt(hwnd, id, color, FALSE);

						::InvalidateRect(control, 0, FALSE);

						return TRUE;
					}

					break;
				}
			case WM_DRAWITEM:
				{
					UINT id = wParam;
					DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;

					if (isColorButton(dis->hwndItem))
					{
						DWORD rgba = ::GetDlgItemInt(hwnd, id, 0, FALSE);
						COLORREF color = Tools::MyColor::getCOLORREF(rgba);
						HBRUSH brush = ::CreateSolidBrush(color);
						::FillRect(dis->hDC, &dis->rcItem, brush);
						::DeleteObject(brush);

						return TRUE;
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
