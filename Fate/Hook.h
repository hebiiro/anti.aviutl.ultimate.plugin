#pragma once
#include "Tools.h"
namespace Tools {
	template<class T>
	struct Hook : T {
		decltype(T::hook)* orig = 0;
		template<class Addr>
		LONG attach(Addr address) {
			orig = reinterpret_cast<decltype(orig)>(address);
			return DetourAttach((PVOID*)&orig, T::hook);
		}
		LONG detach() {
			return DetourDetach((PVOID*)&orig, T::hook);
		}
		template<class Addr> void attach_to_call(Addr address) { orig = hook_call(address, T::hook); }
		template<class Addr> void attach_to_abs_call(Addr address) { orig = hook_abs_call(address, T::hook); }
		void replace(decltype(orig)& address) {
			orig = address;
			address = T::hook;
		}
	};
}
