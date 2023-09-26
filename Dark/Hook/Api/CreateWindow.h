#pragma once
#include "ActCtx.h"

namespace fgo::dark::hook
{
	inline LPCSTR safe_string(LPCSTR name) {
		return ((ULONG_PTR)name > 0x0000FFFF) ? name : "無効";
	}

	inline LPCWSTR safe_string(LPCWSTR name) {
		return ((ULONG_PTR)name > 0x0000FFFF) ? name : L"無効";
	}

	//
	// このクラスは::CreateWindowExA()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(DWORD exStyle, LPCSTR className, LPCSTR windowName, DWORD style,
			int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
		{
			MY_TRACE_FUNC("%hs, %hs", safe_string(className), safe_string(windowName));

			ActCtx::Activator activator;

			return orig(
				exStyle, className, windowName, style,
				x, y, w, h, parent, menu, instance, param);
		}
		inline static decltype(&hook) orig = ::CreateWindowExA;
	} CreateWindowExA;

	//
	// このクラスは::CreateWindowExW()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
			int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
		{
			MY_TRACE_FUNC("%ws, %ws", safe_string(className), safe_string(windowName));

			ActCtx::Activator activator;

			return orig(
				exStyle, className, windowName, style,
				x, y, w, h, parent, menu, instance, param);
		}
		inline static decltype(&hook) orig = ::CreateWindowExW;
	} CreateWindowExW;

	//
	// このクラスは::CreateDialogParamA()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCSTR lpTemplateName,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::CreateDialogParamA;
	} CreateDialogParamA;

	//
	// このクラスは::CreateDialogParamW()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCWSTR lpTemplateName,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::CreateDialogParamW;
	} CreateDialogParamW;

	//
	// このクラスは::CreateDialogIndirectParamA()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCDLGTEMPLATEA lpTemplate,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::CreateDialogIndirectParamA;
	} CreateDialogIndirectParamA;

	//
	// このクラスは::CreateDialogIndirectParamW()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCDLGTEMPLATEW lpTemplate,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::CreateDialogIndirectParamW;
	} CreateDialogIndirectParamW;

	//
	// このクラスは::DialogBoxParamA()をフックします。
	//
	inline struct
	{
		inline static INT_PTR WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCSTR lpTemplateName,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::DialogBoxParamA;
	} DialogBoxParamA;

	//
	// このクラスは::DialogBoxParamW()をフックします。
	//
	inline struct
	{
		inline static INT_PTR WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCWSTR lpTemplateName,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::DialogBoxParamW;
	} DialogBoxParamW;

	//
	// このクラスは::DialogBoxIndirectParamA()をフックします。
	//
	inline struct
	{
		inline static INT_PTR WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCDLGTEMPLATEA lpTemplate,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::DialogBoxIndirectParamA;
	} DialogBoxIndirectParamA;

	//
	// このクラスは::DialogBoxIndirectParamW()をフックします。
	//
	inline struct
	{
		inline static INT_PTR WINAPI hook(
			_In_opt_ HINSTANCE hInstance,
			_In_ LPCDLGTEMPLATEW lpTemplate,
			_In_opt_ HWND hWndParent,
			_In_opt_ DLGPROC lpDialogFunc,
			_In_ LPARAM dwInitParam)
		{
			MY_TRACE_FUNC("");

			ActCtx::Activator activator;

			return orig(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
		}
		inline static decltype(&hook) orig = ::DialogBoxIndirectParamW;
	} DialogBoxIndirectParamW;
}
