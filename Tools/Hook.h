#pragma once
#include "Tools.h"
namespace Tools {
	template<class T> LONG attach(T& hooker) {
		return DetourAttach((PVOID*)&hooker.orig, hooker.hook);
	}
	template<class T> LONG detach(T& hooker) {
		return DetourDetach((PVOID*)&hooker.orig, hooker.hook);
	}
	template<class T, class A> LONG attach(T& hooker, A address) {
		hooker.orig = reinterpret_cast<decltype(hooker.orig)>(address);
		return DetourAttach((PVOID*)&hooker.orig, hooker.hook);
	}
	template<class T, class A> void attach_call(T& hooker, A address) {
		hooker.orig = set_call(address, hooker.hook);
	}
	template<class T, class A> void attach_abs_call(T& hooker, A address) {
		hooker.orig = set_abs_call(address, hooker.hook);
	}
	template<class T, class A> void attach_abs_addr(T& hooker, A address) {
		hooker.orig = set_abs_addr(address, hooker.hook);
	}
	template<class T> void attach_import(T& hooker, HMODULE module, LPCSTR funcName) {
		hooker.orig = hookImportFunc(module, funcName, hooker.hook);
	}
	template<class T> void replace(T& hooker, decltype(hooker.orig)& address) {
		hooker.orig = address;
		address = hooker.hook;
	}
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
		template<class Addr> void attach_to_call(Addr address) { orig = set_call(address, T::hook); }
		template<class Addr> void attach_to_abs_call(Addr address) { orig = set_abs_call(address, T::hook); }
		template<class Addr> void attach_to_abs_addr(Addr address) { orig = set_abs_addr(address, T::hook); }

		void replace(decltype(orig)& address) {
			orig = address;
			address = T::hook;
		}
	};
}
