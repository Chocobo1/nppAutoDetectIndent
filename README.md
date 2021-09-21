# Auto Detect Indention Plugin for [Notepad++](https://notepad-plus-plus.org/) [![AppVeyor build status](https://ci.appveyor.com/api/projects/status/github/Chocobo1/nppAutoDetectIndent?branch=master&svg=true)](https://ci.appveyor.com/project/Chocobo1/nppAutoDetectIndent)

Detects indention (tab or spaces) and auto adjust <kbd>Tab</kbd> key on-the-fly

## What It Does

| Indention used in file | <kbd>Tab</kbd> key            |
| ---------------------- | ----------------------------- |
| Tabs                   | 1 tab                         |
| For example: 3 Spaces  | 3 spaces                      |
| Cannot determine       | Depends on Notepad++ settings |

## Installation
1. If you are running Notepad++ 7.6 or later:
   * You are encouraged to use [Plugins Admin](https://npp-user-manual.org/docs/plugins/#install-using-plugins-admin) for installation.
   * For manual installation instructions, read: [Install plugin manually](https://npp-user-manual.org/docs/plugins/#install-plugin-manually)
2. If you are running Notepad++ version prior to 7.6: \
   Download and extract the archive from below and move `nppAutoDetectIndent.dll` into Notepad++ `plugin` folder.
   * For Notepad++ x86: `C:\Program Files (x86)\Notepad++\plugins` \
   * For Notepad++ x64: `C:\Program Files\Notepad++\plugins`

## Download
[Releases page](../../releases)

## Why
Many projects have differnet coding style requirements, some use tabs other prefer spaces. \
And there is this holy war of [tabs or spaces](https://wiki.c2.com/?TabsVersusSpaces).

I first discovered this function in [Geany editor](https://www.geany.org/): [detecting-indent-type](https://geany.org/manual/dev/#detecting-indent-type) \
I thought it's cool because you don't have to worry about having the correct settings in your editor nor need to care about setting up [editorconfig](https://editorconfig.org/), it just works!

## Compiler
Visual Studio 2019 Community

## License
See [LICENSE](./LICENSE) file
