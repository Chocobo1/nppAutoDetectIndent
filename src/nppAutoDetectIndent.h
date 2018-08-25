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

#ifndef NPPAUTODETECTINDENT_H
#define NPPAUTODETECTINDENT_H

#include <array>
#include <initializer_list>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace nppAutoDetectIndent
{
	struct IndentInfo
	{
		enum class IndentType
		{
			Space,
			Tab,
			Invalid
		};

		IndentType type = IndentType::Invalid;
		int num = 0;  // number of indention per line
	};
	IndentInfo detectIndentInfo();
	void applyIndentInfo(const IndentInfo &info);

	struct NppSettings
	{
		bool tabIndents = false;
		bool useTabs = false;
		int indents = 0;
	};
	NppSettings detectNppSettings();
	void applyNppSettings(const NppSettings &settings);

	using IndentCache = std::unordered_map<std::wstring, IndentInfo>;  // <full file path, IndentInfo>
}

class MyPlugin
{
	class CallFunctor
	{
		public:
			constexpr CallFunctor(const SciFnDirect func, const sptr_t hnd)
				: m_functor(func)
				, m_hnd(hnd)
			{
			}

			template <typename T>
			constexpr T call(const UINT Msg, const WPARAM wParam = 0, const LPARAM lParam = 0) const
			{
				return static_cast<T>(m_functor(m_hnd, Msg, wParam, lParam));
			}

		private:
			const SciFnDirect m_functor;
			const sptr_t m_hnd;
	};

	struct MessageParams
	{
		UINT msg;
		WPARAM wParam;
		LPARAM lParam;
	};

	class Message
	{
		public:
			Message(const NppData &data);

			template <typename T = void>
			constexpr T sendNppMessage(const UINT msg, const WPARAM wParam, const LPARAM lParam) const
			{
				return static_cast<T>(::SendMessage(m_nppData._nppHandle, msg, wParam, lParam));
			}
			void postSciMessages(std::initializer_list<MessageParams> params) const;

			CallFunctor getSciCallFunctor() const;

		private:
			HWND getCurrentSciHwnd() const;

			const NppData m_nppData;
	};

	public:
		static void initInstance();
		static void freeInstance();
		static MyPlugin* instance();

		void setupNppData(const NppData &data);

		Message* message() const;

		const std::array<FuncItem, 4> m_funcItems {};
		nppAutoDetectIndent::IndentCache indentCache;
		std::unordered_map<uptr_t, std::wstring> fileRenameMap;  // <buffer id, old file path>
		nppAutoDetectIndent::NppSettings nppOriginalSettings;

		static constexpr TCHAR *PLUGIN_NAME = TEXT("Auto Detect Indention");

	private:
		MyPlugin();
		~MyPlugin() = default;

		std::unique_ptr<Message> m_message;

		static MyPlugin *m_instance;
};

#endif
