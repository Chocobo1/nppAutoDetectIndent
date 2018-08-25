/*
*  Chocobo1/nppAutoDetectIndent
*
*   Copyright 2017-2018 by Mike Tzou (Chocobo1)
*     https://github.com/Chocobo1/nppAutoDetectIndent
*
*   Licensed under GNU General Public License 3 or later.
*
*  @license GPL3 <https://www.gnu.org/licenses/gpl-3.0-standalone.html>
*/

#include "stdafx.h"
#include "nppAutoDetectIndent.h"
#include "settings.h"

#ifdef NPPAUTODETECTINDENT_EXPORTS
#define NPPAUTODETECTINDENT_API __declspec(dllexport)
#else
#define NPPAUTODETECTINDENT_API __declspec(dllimport)
#endif

BOOL WINAPI DllMain(HANDLE /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			MyPlugin::initInstance();
			Settings::initInstance();
			break;
		}

		case DLL_PROCESS_DETACH:
		{
			MyPlugin::freeInstance();
			Settings::freeInstance();
			break;
		}

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		default:
			break;
	}

	return TRUE;
}

extern "C" NPPAUTODETECTINDENT_API const TCHAR * getName()
{
	return MyPlugin::instance()->PLUGIN_NAME;
}

extern "C" NPPAUTODETECTINDENT_API void setInfo(const NppData data)
{
	MyPlugin::instance()->setupNppData(data);
}

extern "C" NPPAUTODETECTINDENT_API FuncItem * getFuncsArray(int * const funcCount)
{
	const MyPlugin *plugin = MyPlugin::instance();
	*funcCount = static_cast<int>(plugin->m_funcItems.size());
	return const_cast<FuncItem *>(plugin->m_funcItems.data());
}

extern "C" NPPAUTODETECTINDENT_API void beNotified(SCNotification *notifyCode)
{
	MyPlugin * const myPlugin = MyPlugin::instance();
	nppAutoDetectIndent::IndentCache &indentCache = myPlugin->indentCache;
	switch (notifyCode->nmhdr.code)
	{
		case NPPN_READY:
		{
			myPlugin->nppOriginalSettings = nppAutoDetectIndent::detectNppSettings();
			break;
		}

		case NPPN_FILEBEFORECLOSE:
		{
			TCHAR path[MAX_PATH + 1] {};
			myPlugin->message()->sendNppMessage<>(NPPM_GETFULLCURRENTPATH, MAX_PATH, reinterpret_cast<LPARAM>(path));
			indentCache.erase(path);
			break;
		}

		case NPPN_FILEBEFORESAVE:
		{
			const uptr_t bufferId = notifyCode->nmhdr.idFrom;
			TCHAR oldPath[MAX_PATH + 1] {};
			myPlugin->message()->sendNppMessage<>(NPPM_GETFULLCURRENTPATH, MAX_PATH, reinterpret_cast<LPARAM>(oldPath));

			myPlugin->fileRenameMap[bufferId] = oldPath;
			break;
		}

		case NPPN_FILESAVED:
		{
			const uptr_t bufferId = notifyCode->nmhdr.idFrom;
			const std::wstring oldPath = myPlugin->fileRenameMap[bufferId];
			myPlugin->fileRenameMap.erase(bufferId);

			TCHAR newPath[MAX_PATH + 1] {};
			myPlugin->message()->sendNppMessage<>(NPPM_GETFULLCURRENTPATH, MAX_PATH, reinterpret_cast<LPARAM>(newPath));
			indentCache[newPath] = indentCache[oldPath];
			indentCache.erase(oldPath);
			break;
		}

		case NPPN_BUFFERACTIVATED:
		{
			if (Settings::instance()->getDisablePlugin())
				break;

			TCHAR path[MAX_PATH + 1] {};
			myPlugin->message()->sendNppMessage<>(NPPM_GETFULLCURRENTPATH, MAX_PATH, reinterpret_cast<LPARAM>(path));

			const auto iter = indentCache.find(path);
			const nppAutoDetectIndent::IndentInfo info = (iter != indentCache.end()) ? iter->second : nppAutoDetectIndent::detectIndentInfo();
			indentCache[path] = info;
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
