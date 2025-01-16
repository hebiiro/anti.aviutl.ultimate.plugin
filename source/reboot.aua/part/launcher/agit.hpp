#pragma once

namespace apn::reboot::launcher
{
	//
	// このクラスはランチャーのアジトです。
	//
	inline struct Agit
	{
		//
		// このクラスはデューティです。
		//
		struct Duty {
			std::wstring name;
			std::wstring verb;
			std::wstring path;
			std::wstring args;
		} duties[6];
	} agit;
}
