#pragma once

namespace apn::timeline_map::view
{
	//
	// このクラスはビュー層の共通リスナーです。
	//
	struct listner_t {
		virtual void on_from_ui(BOOL recreate_resources) = 0;
	} *listener = {};
}
