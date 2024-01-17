#pragma once

namespace fgo::audio_graph_gui
{
	inline struct Design : Tools::DesignBase
	{
		_bstr_t fontDefault;
		_bstr_t fontDefault2;
		Image image;
		struct Left {
			Image image;
		} left;
		struct Right {
			Image image;
		} right;
		struct Background {
			GradientFill fill;
		} background;
		struct Scale {
			Stroke stroke;
			Text text;
		} scale;
		struct Volume {
			struct Level {
				GradientFill fill;
			} level;
			struct Peak {
				Stroke stroke;
			} peak;
			struct Zebra {
				int step;
				Stroke stroke;
			} zebra;
		} volume;

		Design()
		{
			MY_TRACE_FUNC("");

			fontDefault = L"C:\\Windows\\Fonts\\segoeui.ttf";
			fontDefault2 = L"C:\\Windows\\Fonts\\meiryo.ttc";
			image.fileName = L"";
			image.offset.x = 0;
			image.offset.y = 0;
			image.align.x = AlignX::center;
			image.align.y = AlignY::center;
			image.scaleMode = ScaleMode::normal;
			image.scale = 1.0f;
			image.angle = 0.0f;
			image.alpha = 1.0f;
			left.image.fileName = L"";
			left.image.offset.x = 0;
			left.image.offset.y = 0;
			left.image.align.x = AlignX::center;
			left.image.align.y = AlignY::center;
			left.image.scaleMode = ScaleMode::normal;
			left.image.scale = 1.0f;
			left.image.angle = 0.0f;
			left.image.alpha = 1.0f;
			right.image.fileName = L"";
			right.image.offset.x = 0;
			right.image.offset.y = 0;
			right.image.align.x = AlignX::center;
			right.image.align.y = AlignY::center;
			right.image.scaleMode = ScaleMode::normal;
			right.image.scale = 1.0f;
			right.image.angle = 0.0f;
			right.image.alpha = 1.0f;
			background.fill.start = nvgRGBAf(0.1f, 0.1f, 0.1f, 1.0f);
			background.fill.end = nvgRGBAf(0.1f, 0.1f, 0.1f, 1.0f);
			volume.level.fill.start = nvgRGBAf(0.8f, 0.8f, 0.0f, 0.9f);
			volume.level.fill.end = nvgRGBAf(0.8f, 0.4f, 0.0f, 0.9f);
			volume.peak.stroke.style = NVG_LINE_SOLID;
			volume.peak.stroke.width = 2;
			volume.peak.stroke.color = nvgRGBAf(0.8f, 0.0f, 0.0f, 0.9f);
			volume.zebra.step = 3;
			volume.zebra.stroke.style = NVG_LINE_SOLID;
			volume.zebra.stroke.width = 1;
			volume.zebra.stroke.color = nvgRGBAf(0.0f, 0.0f, 0.0f, 0.9f);
			scale.stroke.style = NVG_LINE_SOLID;
			scale.stroke.width = 1;
			scale.stroke.color = nvgRGBAf(0.8f, 0.8f, 0.8f, 0.9f);
			scale.text.height = 16;
			scale.text.dilate = 0.0f;
			scale.text.blur = 0.0f;
			scale.text.color = nvgRGBAf(0.8f, 0.8f, 0.8f, 1.0f);
			scale.text.shadow.dilate = 1.0f;
			scale.text.shadow.blur = 1.0f;
			scale.text.shadow.color = nvgRGBAf(0.0f, 0.0f, 0.0f, 0.4f);
			scale.text.shadow.offset = { 0, 0 };
		}

		~Design()
		{
			MY_TRACE_FUNC("");
		}

		BOOL load(LPCWSTR fileName)
		{
			MY_TRACE_FUNC("%ws", fileName);

			getPrivateProfileBSTR(fileName, L"Design", L"fontDefault", fontDefault);
			getPrivateProfileBSTR(fileName, L"Design", L"fontDefault2", fontDefault2);

			getPrivateProfileBSTR(fileName, L"Design", L"image.fileName", image.fileName);
			getPrivateProfileInt(fileName, L"Design", L"image.offset.x", image.offset.x);
			getPrivateProfileInt(fileName, L"Design", L"image.offset.y", image.offset.y);
			getPrivateProfileLabel(fileName, L"Design", L"image.align.x", image.align.x, AlignX::labels);
			getPrivateProfileLabel(fileName, L"Design", L"image.align.y", image.align.y, AlignY::labels);
			getPrivateProfileLabel(fileName, L"Design", L"image.scaleMode", image.scaleMode, ScaleMode::labels);
			getPrivateProfileReal(fileName, L"Design", L"image.scale", image.scale);
			getPrivateProfileReal(fileName, L"Design", L"image.angle", image.angle);
			getPrivateProfileReal(fileName, L"Design", L"image.alpha", image.alpha);

			getPrivateProfileBSTR(fileName, L"Design", L"left.image.fileName", left.image.fileName);
			getPrivateProfileInt(fileName, L"Design", L"left.image.offset.x", left.image.offset.x);
			getPrivateProfileInt(fileName, L"Design", L"left.image.offset.y", left.image.offset.y);
			getPrivateProfileLabel(fileName, L"Design", L"left.image.align.x", left.image.align.x, AlignX::labels);
			getPrivateProfileLabel(fileName, L"Design", L"left.image.align.y", left.image.align.y, AlignY::labels);
			getPrivateProfileLabel(fileName, L"Design", L"left.image.scaleMode", left.image.scaleMode, ScaleMode::labels);
			getPrivateProfileReal(fileName, L"Design", L"left.image.scale", left.image.scale);
			getPrivateProfileReal(fileName, L"Design", L"left.image.angle", left.image.angle);
			getPrivateProfileReal(fileName, L"Design", L"left.image.alpha", left.image.alpha);

			getPrivateProfileBSTR(fileName, L"Design", L"right.image.fileName", right.image.fileName);
			getPrivateProfileInt(fileName, L"Design", L"right.image.offset.x", right.image.offset.x);
			getPrivateProfileInt(fileName, L"Design", L"right.image.offset.y", right.image.offset.y);
			getPrivateProfileLabel(fileName, L"Design", L"right.image.align.x", right.image.align.x, AlignX::labels);
			getPrivateProfileLabel(fileName, L"Design", L"right.image.align.y", right.image.align.y, AlignY::labels);
			getPrivateProfileLabel(fileName, L"Design", L"right.image.scaleMode", right.image.scaleMode, ScaleMode::labels);
			getPrivateProfileReal(fileName, L"Design", L"right.image.scale", right.image.scale);
			getPrivateProfileReal(fileName, L"Design", L"right.image.angle", right.image.angle);
			getPrivateProfileReal(fileName, L"Design", L"right.image.alpha", right.image.alpha);

			getPrivateProfileColor(fileName, L"Design", L"background.fill.start", background.fill.start);
			getPrivateProfileColor(fileName, L"Design", L"background.fill.end", background.fill.end);

			getPrivateProfileLabel(fileName, L"Design", L"scale.stroke.style", scale.stroke.style, LineStyle::labels);
			getPrivateProfileInt(fileName, L"Design", L"scale.stroke.width", scale.stroke.width);
			getPrivateProfileColor(fileName, L"Design", L"scale.stroke.color", scale.stroke.color);
			getPrivateProfileInt(fileName, L"Design", L"scale.text.height", scale.text.height);
			getPrivateProfileReal(fileName, L"Design", L"scale.text.dilate", scale.text.dilate);
			getPrivateProfileReal(fileName, L"Design", L"scale.text.blur", scale.text.blur);
			getPrivateProfileColor(fileName, L"Design", L"scale.text.color", scale.text.color);
			getPrivateProfileReal(fileName, L"Design", L"scale.text.shadow.dilate", scale.text.shadow.dilate);
			getPrivateProfileReal(fileName, L"Design", L"scale.text.shadow.blur", scale.text.shadow.blur);
			getPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.x", scale.text.shadow.offset.x);
			getPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.y", scale.text.shadow.offset.y);
			getPrivateProfileColor(fileName, L"Design", L"scale.text.shadow.color", scale.text.shadow.color);

			getPrivateProfileColor(fileName, L"Design", L"volume.level.fill.start", volume.level.fill.start);
			getPrivateProfileColor(fileName, L"Design", L"volume.level.fill.end", volume.level.fill.end);
			getPrivateProfileLabel(fileName, L"Design", L"volume.peak.stroke.style", volume.peak.stroke.style, LineStyle::labels);
			getPrivateProfileInt(fileName, L"Design", L"volume.peak.stroke.width", volume.peak.stroke.width);
			getPrivateProfileColor(fileName, L"Design", L"volume.peak.stroke.color", volume.peak.stroke.color);
			getPrivateProfileInt(fileName, L"Design", L"volume.zebra.step", volume.zebra.step);
			getPrivateProfileLabel(fileName, L"Design", L"volume.zebra.stroke.style", volume.zebra.stroke.style, LineStyle::labels);
			getPrivateProfileInt(fileName, L"Design", L"volume.zebra.stroke.width", volume.zebra.stroke.width);
			getPrivateProfileColor(fileName, L"Design", L"volume.zebra.stroke.color", volume.zebra.stroke.color);

			return TRUE;
		}

		BOOL save(LPCWSTR fileName)
		{
			MY_TRACE_FUNC("%ws", fileName);

			setPrivateProfileBSTR(fileName, L"Design", L"fontDefault", fontDefault);
			setPrivateProfileBSTR(fileName, L"Design", L"fontDefault2", fontDefault2);

			setPrivateProfileBSTR(fileName, L"Design", L"image.fileName", image.fileName);
			setPrivateProfileInt(fileName, L"Design", L"image.offset.x", image.offset.x);
			setPrivateProfileInt(fileName, L"Design", L"image.offset.y", image.offset.y);
			setPrivateProfileLabel(fileName, L"Design", L"image.align.x", image.align.x, AlignX::labels);
			setPrivateProfileLabel(fileName, L"Design", L"image.align.y", image.align.y, AlignY::labels);
			setPrivateProfileLabel(fileName, L"Design", L"image.scaleMode", image.scaleMode, ScaleMode::labels);
			setPrivateProfileReal(fileName, L"Design", L"image.scale", image.scale);
			setPrivateProfileReal(fileName, L"Design", L"image.angle", image.angle);
			setPrivateProfileReal(fileName, L"Design", L"image.alpha", image.alpha);

			setPrivateProfileBSTR(fileName, L"Design", L"left.image.fileName", left.image.fileName);
			setPrivateProfileInt(fileName, L"Design", L"left.image.offset.x", left.image.offset.x);
			setPrivateProfileInt(fileName, L"Design", L"left.image.offset.y", left.image.offset.y);
			setPrivateProfileLabel(fileName, L"Design", L"left.image.align.x", left.image.align.x, AlignX::labels);
			setPrivateProfileLabel(fileName, L"Design", L"left.image.align.y", left.image.align.y, AlignY::labels);
			setPrivateProfileLabel(fileName, L"Design", L"left.image.scaleMode", left.image.scaleMode, ScaleMode::labels);
			setPrivateProfileReal(fileName, L"Design", L"left.image.scale", left.image.scale);
			setPrivateProfileReal(fileName, L"Design", L"left.image.angle", left.image.angle);
			setPrivateProfileReal(fileName, L"Design", L"left.image.alpha", left.image.alpha);

			setPrivateProfileBSTR(fileName, L"Design", L"right.image.fileName", right.image.fileName);
			setPrivateProfileInt(fileName, L"Design", L"right.image.offset.x", right.image.offset.x);
			setPrivateProfileInt(fileName, L"Design", L"right.image.offset.y", right.image.offset.y);
			setPrivateProfileLabel(fileName, L"Design", L"right.image.align.x", right.image.align.x, AlignX::labels);
			setPrivateProfileLabel(fileName, L"Design", L"right.image.align.y", right.image.align.y, AlignY::labels);
			setPrivateProfileLabel(fileName, L"Design", L"right.image.scaleMode", right.image.scaleMode, ScaleMode::labels);
			setPrivateProfileReal(fileName, L"Design", L"right.image.scale", right.image.scale);
			setPrivateProfileReal(fileName, L"Design", L"right.image.angle", right.image.angle);
			setPrivateProfileReal(fileName, L"Design", L"right.image.alpha", right.image.alpha);

			setPrivateProfileColor(fileName, L"Design", L"background.fill.start", background.fill.start);
			setPrivateProfileColor(fileName, L"Design", L"background.fill.end", background.fill.end);

			setPrivateProfileLabel(fileName, L"Design", L"scale.stroke.style", scale.stroke.style, LineStyle::labels);
			setPrivateProfileInt(fileName, L"Design", L"scale.stroke.width", scale.stroke.width);
			setPrivateProfileColor(fileName, L"Design", L"scale.stroke.color", scale.stroke.color);
			setPrivateProfileInt(fileName, L"Design", L"scale.text.height", scale.text.height);
			setPrivateProfileReal(fileName, L"Design", L"scale.text.dilate", scale.text.dilate);
			setPrivateProfileReal(fileName, L"Design", L"scale.text.blur", scale.text.blur);
			setPrivateProfileColor(fileName, L"Design", L"scale.text.color", scale.text.color);
			setPrivateProfileReal(fileName, L"Design", L"scale.text.shadow.dilate", scale.text.shadow.dilate);
			setPrivateProfileReal(fileName, L"Design", L"scale.text.shadow.blur", scale.text.shadow.blur);
			setPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.x", scale.text.shadow.offset.x);
			setPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.y", scale.text.shadow.offset.y);
			setPrivateProfileColor(fileName, L"Design", L"scale.text.shadow.color", scale.text.shadow.color);

			setPrivateProfileColor(fileName, L"Design", L"volume.level.fill.start", volume.level.fill.start);
			setPrivateProfileColor(fileName, L"Design", L"volume.level.fill.end", volume.level.fill.end);
			setPrivateProfileLabel(fileName, L"Design", L"volume.peak.stroke.style", volume.peak.stroke.style, LineStyle::labels);
			setPrivateProfileInt(fileName, L"Design", L"volume.peak.stroke.width", volume.peak.stroke.width);
			setPrivateProfileColor(fileName, L"Design", L"volume.peak.stroke.color", volume.peak.stroke.color);
			setPrivateProfileInt(fileName, L"Design", L"volume.zebra.step", volume.zebra.step);
			setPrivateProfileLabel(fileName, L"Design", L"volume.zebra.stroke.style", volume.zebra.stroke.style, LineStyle::labels);
			setPrivateProfileInt(fileName, L"Design", L"volume.zebra.stroke.width", volume.zebra.stroke.width);
			setPrivateProfileColor(fileName, L"Design", L"volume.zebra.stroke.color", volume.zebra.stroke.color);

			return TRUE;
		}
	} design;
}
