#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <string_view>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;


#include "winmm.hpp"
#include "Util.hpp"

#pragma pack(push, 1)
struct jmp
{
	uint8_t opcode;
	size_t address;
};
#pragma pack(pop)

size_t addrCreateFontIndirectW = 0;
size_t addrGetStockObject = 0;

// overrideflags
#define	_NONE 0
#define	_HEIGHT 1 << 1
#define	_WIDTH 1 << 2
#define	_WEIGHT 1 << 3
#define	_ITALIC 1 << 4
#define	_UNDERLINE 1 << 5
#define	_STRIKEOUT 1 << 6
#define	_CHARSET 1 << 7
#define	_OUTPRECISION 1 << 8
#define	_CLIPPRECISION 1 << 9
#define	_QUALITY 1 << 10
#define	_PITCHANDFAMILY 1 << 11

struct font
{
	std::wstring replace;
	uint32_t overrideFlags;
	long height;
	long width;
	long weight;
	bool italic;
	bool underLine;
	bool strikeOut;
	BYTE charSet;
	BYTE outPrecision;
	BYTE clipPrecision;
	BYTE quality;
	BYTE pitchAndFamily;
};

std::unordered_map<std::wstring, font> fontsMap;

__declspec(naked) HFONT WINAPI CallOrigCreateFontIndirectW(const LOGFONTW* lplf)
{
	_asm
	{
		mov edi, edi
		push ebp
		mov ebp, esp
		jmp addrCreateFontIndirectW
	}
}

__declspec(naked) HGDIOBJ WINAPI CallOrigGetStockObject(int i)
{
	_asm
	{
		mov edi, edi
		push ebp
		mov ebp, esp
		jmp addrGetStockObject
	}
}

HFONT WINAPI MyCreateFontIndirectW(LOGFONTW* lplf)
{
	auto it = fontsMap.find(lplf->lfFaceName);
	if (it != fontsMap.end())
	{
		size_t len = it->second.replace._Copy_s(lplf->lfFaceName, LF_FACESIZE, LF_FACESIZE);
		lplf->lfFaceName[len] = L'\0';

		if ((it->second.overrideFlags & _HEIGHT) == _HEIGHT)
			lplf->lfHeight = it->second.height;
		if ((it->second.overrideFlags & _WIDTH) == _WIDTH)
			lplf->lfWidth = it->second.width;
		if ((it->second.overrideFlags & _WEIGHT) == _WEIGHT)
			lplf->lfWeight = it->second.weight;
		if ((it->second.overrideFlags & _ITALIC) == _ITALIC)
			lplf->lfItalic = it->second.italic;
		if ((it->second.overrideFlags & _UNDERLINE) == _UNDERLINE)
			lplf->lfUnderline = it->second.underLine;
		if ((it->second.overrideFlags & _STRIKEOUT) == _STRIKEOUT)
			lplf->lfStrikeOut = it->second.strikeOut;
		if ((it->second.overrideFlags & _CHARSET) == _CHARSET)
			lplf->lfCharSet = it->second.charSet;
		if ((it->second.overrideFlags & _OUTPRECISION) == _OUTPRECISION)
			lplf->lfOutPrecision = it->second.outPrecision;
		if ((it->second.overrideFlags & _CLIPPRECISION) == _CLIPPRECISION)
			lplf->lfClipPrecision = it->second.clipPrecision;
		if ((it->second.overrideFlags & _QUALITY) == _QUALITY)
			lplf->lfQuality = it->second.quality;
		if ((it->second.overrideFlags & _PITCHANDFAMILY) == _PITCHANDFAMILY)
			lplf->lfPitchAndFamily = it->second.pitchAndFamily;
	}
	return CallOrigCreateFontIndirectW(lplf);
}

void LoadUserFonts(const fs::path& path)
{
	try
	{
		auto fontsPath = path / L"fonts";
		if (fs::is_directory(fontsPath))
		{
			for (auto& f : fs::directory_iterator(fontsPath))
			{
				if (f.is_directory()) continue;
				int ret = AddFontResourceExW(f.path().c_str(), FR_PRIVATE, 0);
			}
		}
	}
	catch (const std::exception)
	{
		return;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

#if _DEBUG
		MessageBoxW(0, L"DLL_PROCESS_ATTACH", L"", 0);
#endif

		if (!LoadDLL())
			return FALSE;

		auto path = GetModuleFsPath(hModule);

		LoadUserFonts(path);
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
#if _DEBUG
		MessageBoxW(0, L"CLOSING", L"", 0);
#endif
	}
	return TRUE;
}
