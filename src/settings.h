/*
*  Chocobo1/nppAutoDetectIndent
*
*   Copyright 2018 by Mike Tzou (Chocobo1)
*     https://github.com/Chocobo1/nppAutoDetectIndent
*
*   Licensed under GNU General Public License 3 or later.
*
*  @license GPL3 <https://www.gnu.org/licenses/gpl-3.0-standalone.html>
*/

#ifndef SETTINGS_H
#define SETTINGS_H

class Settings
{
	public:
		static void initInstance();
		static void freeInstance();
		static Settings* instance();

		// accessors
		using BoolGetter = bool (Settings::*)() const;
		using BoolSetter = void (Settings::*)(bool);

		bool getDisablePlugin() const;
		void setDisablePlugin(bool value);

	private:
		constexpr Settings() = default;
		~Settings() = default;

		static Settings *m_instance;

		// data
		bool m_disablePlugin = false;
};

#endif
