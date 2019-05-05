#pragma once

// https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/author-coclasses#add-helper-types-and-functions
auto GetModuleFsPath(HMODULE hModule)
{
	std::wstring path(MAX_PATH, L'\0');
	DWORD pathSize;
	DWORD actualSize;

	do
	{
		pathSize = static_cast<DWORD>(path.size());
		actualSize = GetModuleFileNameW(nullptr, path.data(), pathSize);

		if (actualSize + 1 > pathSize)
		{
			path.resize(pathSize * 2);
		}
	} while (actualSize + 1 > pathSize);

	path.resize(actualSize);
	return fs::path(path).remove_filename();
}

void SetThreadDpiAware()
{
	HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
	if (hUser32)
	{
		using SetThreadDpiAwarenessContextPtr = DPI_AWARENESS_CONTEXT(WINAPI *)(DPI_AWARENESS_CONTEXT);
		auto funcPtr = reinterpret_cast<SetThreadDpiAwarenessContextPtr>(GetProcAddress(hUser32, "SetThreadDpiAwarenessContext"));
		if (funcPtr)
			funcPtr(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
	}
}
