#include "stdafx.h"
#include "nppAutoDetectIndent.h"


#ifdef NPPAUTODETECTINDENT_EXPORTS
#define NPPAUTODETECTINDENT_API __declspec(dllexport)
#else
#define NPPAUTODETECTINDENT_API __declspec(dllimport)
#endif


MyPlugin plugin;
static nppAutoDetectIndent::IndentCache indentCache;


BOOL WINAPI DllMain(HANDLE /*hinstDLL*/, DWORD  fdwReason, LPVOID /*lpvReserved*/)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		default:
			break;
	}

	return TRUE;
}

extern "C" NPPAUTODETECTINDENT_API const TCHAR * getName()
{
	return plugin.PLUGIN_NAME;
}

extern "C" NPPAUTODETECTINDENT_API void setInfo(NppData data)
{
	plugin.setupNppData(data);
}

extern "C" NPPAUTODETECTINDENT_API FuncItem * getFuncsArray(int *funcCount)
{
	*funcCount = int(plugin.functionsCount());
	return plugin.getFunctionsArray();
}

extern "C" NPPAUTODETECTINDENT_API void beNotified(SCNotification *notifyCode)
{
	switch (notifyCode->nmhdr.code)
	{
		case NPPN_FILECLOSED:
		{
			const uptr_t id = notifyCode->nmhdr.idFrom;

			indentCache.erase(id);
			break;
		}

		case NPPN_BUFFERACTIVATED:
		{
			const uptr_t id = notifyCode->nmhdr.idFrom;

			const auto iter = indentCache.find(id);
			const nppAutoDetectIndent::IndentInfo info = (iter != indentCache.end()) ? iter->second : nppAutoDetectIndent::getIndentInfo();

			indentCache[id] = info;
			applyIndentInfo(info);
			break;
		}

		default:
		{
			break;
		}
	}
}

extern "C" NPPAUTODETECTINDENT_API LRESULT messageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return TRUE;
}

#ifdef UNICODE
extern "C" NPPAUTODETECTINDENT_API BOOL isUnicode()
{
	return TRUE;
}
#endif // UNICODE
