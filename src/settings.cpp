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

#include "settings.h"

Settings *Settings::m_instance = nullptr;

void Settings::initInstance()
{
	if (!m_instance)
		m_instance = new Settings;
}

void Settings::freeInstance()
{
	delete m_instance;
	m_instance = nullptr;
}

Settings* Settings::instance()
{
	return m_instance;
}

bool Settings::getDisablePlugin() const
{
	return m_disablePlugin;
}

void Settings::setDisablePlugin(const bool value)
{
	m_disablePlugin = value;
}
