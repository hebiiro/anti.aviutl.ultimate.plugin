#pragma once
#include "Hive.h"

namespace fgo::dark::skin
{
	inline struct Config
	{
		struct ShadowMode {
			static const int Off = 0;
			static const int On = 1;
			inline static const Hive::Label label[] = {
				{ L"Off", Off },
				{ L"On", On },
			};
		};

		struct RoundMode {
			static const int Off = 0;
			static const int On = 1;
			inline static const Hive::Label label[] = {
				{ L"Off", Off },
				{ L"On", On },
			};
		};

		struct StaticEdgeMode {
			static const int Off = 0;
			static const int On = 1;
			inline static const Hive::Label label[] = {
				{ L"Off", Off },
				{ L"On", On },
			};
		};

		struct OmitFileDialog {
			static const int Off = 0;
			static const int On = 1;
			inline static const Hive::Label label[] = {
				{ L"Off", Off },
				{ L"On", On },
			};
		};

		struct ExEdit {
			struct Group {
				COLORREF color = CLR_NONE;
				float alpha = 0.0f;
			} group;
			struct XorPen {
				int style = -1;
				int width = -1;
				COLORREF color = CLR_NONE;
			} xorPen;
		} exedit;

		_bstr_t skinFileName = L"";
		int shadowMode = ShadowMode::On;
		int roundMode = RoundMode::On;
		int staticEdgeMode = StaticEdgeMode::Off;
		BOOL useLayerColor = TRUE;
		BOOL useLayerColorEx = FALSE;
		int omitFileDialog = OmitFileDialog::On;

		_bstr_t getSkinFileName() const { return skinFileName; }
		void setSkinFileName(_bstr_t newSkinFileName) { skinFileName = newSkinFileName; }

		int getShadowMode() const { return shadowMode; }
		void setShadowMode(int newShadowMode) { shadowMode = newShadowMode; }

		int getRoundMode() const { return roundMode; }
		void setRoundMode(int newRoundMode) { roundMode = newRoundMode; }

		int getStaticEdgeMode() const { return staticEdgeMode; }
		void setStaticEdgeMode(int newStaticEdgeMode) { staticEdgeMode = newStaticEdgeMode; }

		BOOL getUseLayerColor() const { return useLayerColor; }
		void setUseLayerColor(BOOL newUseLayerColor) { useLayerColor = newUseLayerColor; }

		BOOL getUseLayerColorEx() const { return useLayerColorEx; }
		void setUseLayerColorEx(BOOL newUseLayerColorEx) { useLayerColorEx = newUseLayerColorEx; }

		int getOmitFileDialog() const { return omitFileDialog; }
		void setOmitFileDialog(int newOmitFileDialog) { omitFileDialog = newOmitFileDialog; }

		inline static void getConfigFolderName(LPWSTR buffer, size_t bufferSize)
		{
			WCHAR fileName[MAX_PATH] = {};
			::GetModuleFileNameW(fgo::dark::hive.instance, fileName, std::size(fileName));
			::PathRemoveFileSpecW(fileName);
			::PathAppendW(fileName, L"../UltimateConfig");
			::PathCanonicalizeW(buffer, fileName);
		}

		inline static void getConfigFileName(LPWSTR buffer, size_t bufferSize)
		{
			getConfigFolderName(buffer, bufferSize);
			::PathAppendW(buffer, L"Dark.xml");
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			{
				// <ExEditGroup>を読み込みます。

				MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"ExEditGroup");
				int c = nodeList->length;
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

					getPrivateProfileNamedColor(element, L"color", exedit.group.color, ColorSet::Type::fillColor);
					getPrivateProfileReal(element, L"alpha", exedit.group.alpha);
				}
			}

			{
				// <XorPen>を読み込みます。

				MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"XorPen");
				int c = nodeList->length;
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

					getPrivateProfileInt(element, L"style", exedit.xorPen.style);
					getPrivateProfileInt(element, L"width", exedit.xorPen.width);
					getPrivateProfileNamedColor(element, L"color", exedit.xorPen.color, ColorSet::Type::edgeColor);
				}
			}

			return S_OK;
		}
	} config;
}
