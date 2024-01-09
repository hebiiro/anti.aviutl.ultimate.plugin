#include "framework.h"
#include "Fate/Grand Order"
#include "Share/Filer.h"
#include "Dark/dark.h"

#include <shobjidl.h>
_COM_SMARTPTR_TYPEDEF(IExplorerBrowser, __uuidof(IExplorerBrowser));
_COM_SMARTPTR_TYPEDEF(IExplorerPaneVisibility, __uuidof(IExplorerPaneVisibility));
_COM_SMARTPTR_TYPEDEF(IShellItem, __uuidof(IShellItem));
_COM_SMARTPTR_TYPEDEF(IFolderFilterSite, __uuidof(IFolderFilterSite));

#pragma comment(linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
