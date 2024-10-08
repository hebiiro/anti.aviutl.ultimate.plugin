#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// 相対パスを絶対パスに変換して返します。
	//
	std::filesystem::path get_abs_path(const std::filesystem::path& origin, const std::filesystem::path& rel_path)
	{
		// 起点パスと相対パスを結合します。
		auto path = origin / rel_path;

		// パスを正規化して返します。
		return path.lexically_normal();
	}
}
