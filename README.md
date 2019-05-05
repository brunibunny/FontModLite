# FontModLite
This is a fork of ysc3839's [FontMod](https://github.com/ysc3839/FontMod)

Simple hook tool to _add_ fonts to a Win32 program.
Works almost like the original FontMod, but there's no log, no font replacement and no configuration file. It is basically just an easy way to use [AddFontResourceExW](https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/nf-wingdi-addfontresourceexw) on any(?) executable.