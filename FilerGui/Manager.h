#pragma once
#include "resource.h"

class Manager : public CDialogEx
{
public:

	BOOL loadConfig()
	{
		MY_TRACE(_T("Manager::loadConfig()\n"));
#if 0
		try
		{
			MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

			if (document->load(hive->configFileName) == VARIANT_FALSE)
			{
				MY_TRACE(_T("%s の読み込みに失敗しました\n"), hive->configFileName);

				return FALSE;
			}

			MSXML2::IXMLDOMElementPtr element = document->documentElement;

			getPrivateProfileBool(element, L"isVoiceEnabled", hive->isVoiceEnabled);

			{
				talents.clear();

				MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"filer");
				int c = nodeList->length;
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

					_bstr_t path;
					getPrivateProfileString(element, L"path", path);
					MY_TRACE_WSTR((BSTR)path);
					if ((BSTR)path)
					{
						talents.emplace_back(std::make_shared<Talent>(name));
					}
				}
			}

			{
				hive->favorites.clear();

				MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"favorite");
				int c = nodeList->length;
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

					_bstr_t path;
					getPrivateProfileString(element, L"path", path);
					MY_TRACE_WSTR((BSTR)path);
					if ((BSTR)path)
					{
						hive->favorites.emplace_back(path);
					}
				}
			}
		}
		catch (_com_error& e)
		{
			MY_TRACE(_T("%s\n"), e.ErrorMessage());

			return FALSE;
		}
#endif
		return TRUE;
	}

	void saveConfig()
	{
	}

protected:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGER };
#endif
	DECLARE_MESSAGE_MAP()
};
