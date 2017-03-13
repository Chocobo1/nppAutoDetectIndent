#pragma once

#include <string>
#include <unordered_map>
#include <vector>


#define ARRAY_LENGTH(a) (std::extent<decltype(a)>::value)


namespace nppAutoDetectIndent
{
	enum class IndentType
	{
		Spaces,
		Tabs,
		Invalid
	};

	IndentType detectIndentType();
	int detectIndentSpaces();

	struct IndentInfo
	{
		IndentType type;
		int num;  // number of indents
	};

	IndentInfo getIndentInfo();
	void applyIndentInfo(const IndentInfo &info);

	typedef std::unordered_map<uptr_t, IndentInfo> IndentCache;  // <file name, IndentInfo>
}


class MyPlugin
{
	public:
	class DirectCallFunctor
	{
		public:
		explicit DirectCallFunctor(const SciFnDirect func, const sptr_t hnd)
			: m_functor(func)
			, m_hnd(hnd)
		{
		}

		sptr_t operator()(const UINT Msg, const WPARAM wParam = 0, const LPARAM lParam = 0) const
		{
			return m_functor(m_hnd, Msg, wParam, lParam);
		}

		private:
		const SciFnDirect m_functor;
		const sptr_t m_hnd;
	};

	const TCHAR *PLUGIN_NAME = TEXT("Auto Detect Indention");

	MyPlugin();

	void setupNppData(const NppData &data);

	HWND getNppHwnd() const;
	HWND getCurrentScintillaHwnd() const;
	DirectCallFunctor getScintillaDirectCall(HWND scintillaHwnd = NULL) const;

	FuncItem * getFunctionsArray() const;
	size_t functionsCount() const;


	private:
	static FuncItem funcItemCreate(const TCHAR *cmdName, const PFUNCPLUGINCMD pFunc, const bool check0nInit, ShortcutKey *sk);

	NppData m_nppData = {0};
	std::vector<FuncItem> m_funcItems;
};
