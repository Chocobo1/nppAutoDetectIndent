#include "stdafx.h"
#include "nppAutoDetectIndent.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

#include <Shellapi.h>
#include <Strsafe.h>


#define PLUGIN_VERSION "1.0"

static const int MAX_LINES = 5000;
extern MyPlugin plugin;


namespace MenuAction
{
	void doNothing();
	void gotoWebsite();
}


nppAutoDetectIndent::IndentInfo nppAutoDetectIndent::getIndentInfo()
{
	const nppAutoDetectIndent::IndentType type = nppAutoDetectIndent::detectIndentType();
	switch (type)
	{
		case nppAutoDetectIndent::IndentType::Spaces:
			return IndentInfo {type, nppAutoDetectIndent::detectIndentSpaces()};
		case nppAutoDetectIndent::IndentType::Tabs:
			return IndentInfo {type, 0};
		default:
			return IndentInfo {nppAutoDetectIndent::IndentType::Invalid, 0};
	};
}

void nppAutoDetectIndent::applyIndentInfo(const nppAutoDetectIndent::IndentInfo &info)
{
	const HWND sciHwnd = plugin.getCurrentScintillaHwnd();
	switch (info.type)
	{
		case nppAutoDetectIndent::IndentType::Spaces:
		{
			::PostMessage(sciHwnd, SCI_SETTABINDENTS, true, 0);

			::PostMessage(sciHwnd, SCI_SETINDENT, info.num, 0);
			::PostMessage(sciHwnd, SCI_SETUSETABS, false, 0);
			break;
		}

		case nppAutoDetectIndent::IndentType::Tabs:
		{
			::PostMessage(sciHwnd, SCI_SETTABINDENTS, false, 0);

			// no need of SCI_SETINDENT
			::PostMessage(sciHwnd, SCI_SETUSETABS, true, 0);
			break;
		}

		case nppAutoDetectIndent::IndentType::Invalid:
		{
			break;  // do nothing
		}

		default:
			assert(false);
	};
}

int nppAutoDetectIndent::detectIndentSpaces()
{
	// return: number of spaces for indention

	const auto sciCall = plugin.getScintillaDirectCall();

	int spaces[10] = {0};  // [2, 12]

	const int Lines = (std::max)(int(sciCall(SCI_GETLINECOUNT)), MAX_LINES);
	for (int i = 0; i < Lines; ++i)
	{
		const int indentWidth = int(sciCall(SCI_GETLINEINDENTATION, i));
		if (indentWidth < 2)
			continue;

		const int linePos = int(sciCall(SCI_POSITIONFROMLINE, i));
		const char lineHeadChar = char(sciCall(SCI_GETCHARAT, linePos));

		if (lineHeadChar == '\t')
			continue;

		// TODO: get length of continuous spaces
		// but now lets just be lazy...

		for (int k = 0; k < ARRAY_LENGTH(spaces); ++k)
		{
			if (indentWidth % (k + 2) == 0)
				++spaces[k];
		}
	}

	int which = 0;
	int weight = 0;
	for (int i = (ARRAY_LENGTH(spaces) - 1); i >= 0; --i)  // give larger indents higher chance
	{
		if (spaces[i] > (weight * 3 / 2))
		{
			weight = spaces[i];
			which = i;
		}
	}

	return (which + 2);
}

nppAutoDetectIndent::IndentType nppAutoDetectIndent::detectIndentType()
{
	// return -1: cannot decide
	// return  0: indent type is spaces
	// return  1: indent type is tabs

	const auto sciCall = plugin.getScintillaDirectCall();

	size_t tabs = 0;
	size_t spaces = 0;

	const int Lines = (std::max)(int(sciCall(SCI_GETLINECOUNT)), MAX_LINES);
	for (int i = 0; i < Lines; ++i)
	{
		const int indentWidth = int(sciCall(SCI_GETLINEINDENTATION, i));  // tabs width depends on SCI_SETTABWIDTH
		if (indentWidth < 2)
			continue;

		const int linePos = int(sciCall(SCI_POSITIONFROMLINE, i));
		const char lineHeadChar = char(sciCall(SCI_GETCHARAT, linePos));
		if (lineHeadChar == '\t')
			++tabs;
		else if (lineHeadChar == ' ')
		{
			if (indentWidth >(80 / 3))  // 1/3 of 80-width screen, this line must be alignment, skip
				;
			else
				++spaces;
		}
	}

	if ((tabs == 0) && (spaces == 0))
		return nppAutoDetectIndent::IndentType::Invalid;

	if (spaces > (tabs * 4))
		return nppAutoDetectIndent::IndentType::Spaces;
	else if (tabs > (spaces * 4))
		return nppAutoDetectIndent::IndentType::Tabs;
	else
	{
		bool useTab = (bool) sciCall(SCI_GETUSETABS);
		return useTab ? nppAutoDetectIndent::IndentType::Tabs : nppAutoDetectIndent::IndentType::Spaces;
	}
}


MyPlugin::MyPlugin()
{
	const FuncItem items[] = {
		funcItemCreate(TEXT("Version: " PLUGIN_VERSION), MenuAction::doNothing, NULL, false),
		funcItemCreate(TEXT("Goto website..."), MenuAction::gotoWebsite, NULL, false)
	};

	for (const auto &i : items)
		m_funcItems.emplace_back(i);
}

void MyPlugin::setupNppData(const NppData &data)
{
	m_nppData = data;
}

HWND MyPlugin::getNppHwnd() const
{
	return m_nppData._nppHandle;
}

HWND MyPlugin::getCurrentScintillaHwnd() const
{
	int view = -1;
	::SendMessage(m_nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM) &view);
	return (view == 0) ? m_nppData._scintillaMainHandle : m_nppData._scintillaSecondHandle;
}

MyPlugin::DirectCallFunctor MyPlugin::getScintillaDirectCall(HWND scintillaHwnd) const
{
	if (scintillaHwnd == NULL)
		scintillaHwnd = getCurrentScintillaHwnd();

	const SciFnDirect func = (SciFnDirect) ::SendMessage(scintillaHwnd, SCI_GETDIRECTFUNCTION, 0, 0);
	const sptr_t hnd = (sptr_t) ::SendMessage(scintillaHwnd, SCI_GETDIRECTPOINTER, 0, 0);

	return DirectCallFunctor(func, hnd);
}

FuncItem * MyPlugin::getFunctionsArray() const
{
	return (FuncItem *) m_funcItems.data();
}

size_t MyPlugin::functionsCount() const
{
	return m_funcItems.size();
}

FuncItem MyPlugin::funcItemCreate(const TCHAR *cmdName, const PFUNCPLUGINCMD pFunc, const bool check0nInit, ShortcutKey *sk)
{
	FuncItem item = {0};

	StringCchCopy(item._itemName, ARRAY_LENGTH(item._itemName), cmdName);
	item._pFunc = pFunc;
	item._init2Check = check0nInit;
	item._pShKey = sk;

	return item;
}


void MenuAction::doNothing()
{
	/*
	typedef std::chrono::high_resolution_clock Clock;
	auto t1 = Clock::now();
	// do something
	auto t2 = Clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	*/
}

void MenuAction::gotoWebsite()
{
	ShellExecute(NULL, TEXT("open"), TEXT("https://github.com/Chocobo1/nppAutoDetectIndent"), NULL, NULL, SW_SHOWDEFAULT);
}
