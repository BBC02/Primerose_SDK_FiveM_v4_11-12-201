#include "pch.h"
#include "Memory.hpp"

wchar_t* CMem::GetFileNameFromPath(wchar_t* Path)
{
	wchar_t* LastSlash = NULL;
	for (DWORD i = 0; Path[i] != 0; i++)
	{
		if (Path[i] == '\\')
			LastSlash = &Path[i + 1];
	}
	return LastSlash;
}

wchar_t* CMem::RemoveFileExtension(wchar_t* FullFileName, wchar_t* OutputBuffer, DWORD OutputBufferSize)
{
	wchar_t* LastDot = NULL;
	for (DWORD i = 0; FullFileName[i] != NULL; i++)
		if (FullFileName[i] == '.')
			LastDot = &FullFileName[i];

	for (DWORD j = 0; j < OutputBufferSize; j++)
	{
		OutputBuffer[j] = FullFileName[j];
		if (&FullFileName[j] == LastDot)
		{
			OutputBuffer[j] = NULL;
			break;
		}
	}
	OutputBuffer[OutputBufferSize - 1] = NULL;
	return OutputBuffer;
}

HMODULE WINAPI CMem::GetModuleW(_In_opt_ LPCWSTR lpModuleName)
{
	struct CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	};

	struct TEB
	{
		NT_TIB NtTib;
		PVOID EnvironmentPointer;
		CLIENT_ID ClientId;
		PVOID ActiveRpcHandle;
		PVOID ThreadLocalStoragePointer;
		struct PEB* ProcessEnvironmentBlock;
		//...
	};

	struct PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		HANDLE SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
	};

	struct PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN IsProtectedProcessLight : 1;
				BOOLEAN SpareBits : 1;
			};
		};
		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PEB_LDR_DATA* Ldr;
	};

	struct UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWCH Buffer;
	};

	struct LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		union
		{
			LIST_ENTRY InInitializationOrderLinks;
			LIST_ENTRY InProgressLinks;
		};
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
	};

	PEB* ProcessEnvironmentBlock = ((PEB*)((TEB*)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock));
	if (lpModuleName == nullptr)
		return (HMODULE)(ProcessEnvironmentBlock->ImageBaseAddress);

	PEB_LDR_DATA* Ldr = ProcessEnvironmentBlock->Ldr;

	LIST_ENTRY* ModuleLists[3] = { 0,0,0 };
	ModuleLists[0] = &Ldr->InLoadOrderModuleList;
	ModuleLists[1] = &Ldr->InMemoryOrderModuleList;
	ModuleLists[2] = &Ldr->InInitializationOrderModuleList;

	for (int j = 0; j < 3; j++)
	{
		for (LIST_ENTRY* pListEntry = ModuleLists[j]->Flink; pListEntry != ModuleLists[j]; pListEntry = pListEntry->Flink)
		{
			LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)((BYTE*)pListEntry - sizeof(LIST_ENTRY) * j);

			if (_wcsicmp(pEntry->BaseDllName.Buffer, lpModuleName) == 0)
				return (HMODULE)pEntry->DllBase;

			wchar_t* FileName = this->GetFileNameFromPath(pEntry->FullDllName.Buffer);
			if (!FileName)
				continue;

			if (_wcsicmp(FileName, lpModuleName) == 0)
				return (HMODULE)pEntry->DllBase;

			wchar_t FileNameWithoutExtension[256];
			this->RemoveFileExtension(FileName, FileNameWithoutExtension, 256);

			if (_wcsicmp(FileNameWithoutExtension, lpModuleName) == 0)
				return (HMODULE)pEntry->DllBase;
		}
	}

	return 0;
}

uintptr_t CMem::Module(LPCSTR moduleName)
{
	if (!moduleName)
		return 0;

	DWORD ModuleNameLength = (DWORD)strlen(moduleName) + 1;
	DWORD NewBufferSize = sizeof(wchar_t) * ModuleNameLength;
	wchar_t* W_ModuleName = (wchar_t*)_alloca(NewBufferSize);
	for (DWORD i = 0; i < ModuleNameLength; i++)
		W_ModuleName[i] = moduleName[i];

	HMODULE hReturnModule = GetModuleW(W_ModuleName);

	RtlSecureZeroMemory(W_ModuleName, NewBufferSize);
	return (uintptr_t)hReturnModule;
}

HMODULE CMem::ModuleEx(LPCSTR moduleName)
{
	if (!moduleName)
		return 0;

	DWORD ModuleNameLength = (DWORD)strlen(moduleName) + 1;
	DWORD NewBufferSize = sizeof(wchar_t) * ModuleNameLength;
	wchar_t* W_ModuleName = (wchar_t*)_alloca(NewBufferSize);

	for (DWORD i = 0; i < ModuleNameLength; i++)
		W_ModuleName[i] = moduleName[i];

	HMODULE hReturnModule = GetModuleW(W_ModuleName);

	RtlSecureZeroMemory(W_ModuleName, NewBufferSize);
	return hReturnModule;
}

std::string CMem::Hwid()
{
	HANDLE hToken = NULL;
	std::string usersid;

	unsigned long long tokenQuery = TOKEN_QUERY;

	tokenQuery++;
	tokenQuery--;

	if (tokenQuery == TOKEN_QUERY)
		memset((void*)&tokenQuery, 0, sizeof(tokenQuery));

	tokenQuery = (TOKEN_QUERY * 2);

	if (!OpenProcessToken(SAFE_CALL(GetCurrentProcess)(), (tokenQuery / 2), &hToken))
	{
		return nullptr;
	}

	DWORD dwBufferSize = 0;
	if (!SAFE_CALL(GetTokenInformation)(hToken, TokenUser, NULL, 0, &dwBufferSize) && (SAFE_CALL(GetLastError)() != ERROR_INSUFFICIENT_BUFFER))
	{
		SAFE_CALL(CloseHandle)(hToken);
		hToken = NULL;
		return nullptr;
	}

	std::vector<BYTE> buffer;
	buffer.resize(dwBufferSize);

	PTOKEN_USER pTokenUser = reinterpret_cast<PTOKEN_USER>(&buffer[0]);

	if (!SAFE_CALL(GetTokenInformation)(
		hToken,
		TokenUser,
		pTokenUser,
		dwBufferSize,
		&dwBufferSize))
	{
		SAFE_CALL(CloseHandle)(hToken);
		hToken = NULL;

		return nullptr;
	}


	if (!SAFE_CALL(IsValidSid)(pTokenUser->User.Sid))
	{
		SAFE_CALL(CloseHandle)(hToken);
		hToken = NULL;

		return nullptr;
	}

	LPSTR string;
	SAFE_CALL(ConvertSidToStringSidA)(pTokenUser->User.Sid, &string);

	return sha256(string);
}

std::string CMem::ReadRegistry(LPCSTR subkey, LPCSTR name, DWORD type)
{
	HKEY key;
	CHAR value[1024];
	DWORD valueLength = 1024;

	Call<LSTATUS>(_("Advapi32.dll"), _("RegOpenKeyA")).Run( HKEY_LOCAL_MACHINE , subkey , &key );
	Call<LSTATUS> ( _ ( "Advapi32.dll" ) , _ ( "RegQueryValueExA" ) ).Run ( key , name , 0 , &type , ( PBYTE ) &value , &valueLength );
	Call<LSTATUS> ( _ ( "Advapi32.dll" ) , _ ( "RegCloseKey" ) ).Run ( key );

	return value;
}

void CMem::WriteRegistry(LPCSTR subkey, LPCSTR name, DWORD type, LPCSTR data)
{
	auto key = HKEY( );

	Call<LSTATUS> ( _ ( "Advapi32.dll")  , _ ( "RegOpenKeyA" )).Run( HKEY_LOCAL_MACHINE, subkey, &key );
	Call<LSTATUS> ( _ ( "Advapi32.dll")  , _ ( "RegSetValueExA" )).Run(key, name, 0, type, (PBYTE)data, strlen(data) * sizeof(char));
	Call<LSTATUS> ( _ ( "Advapi32.dll" ) , _ ( "RegCloseKey" ) ).Run ( key );
}

uintptr_t CMem::Pattern(const char* moduleName, const char* pattern)
{
	uintptr_t moduleAdress = Module(moduleName);

	static auto patternToByte = [](const char* pattern)
	{
		auto       bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else { bytes.push_back(strtoul(current, &current, 16)); }
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)moduleAdress;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleAdress + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto       patternBytes = patternToByte(pattern);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(moduleAdress);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found) { return reinterpret_cast<uintptr_t>(&scanBytes[i]); }
	}
	return 0;
}

void CMem::Box(const char* message)
{
	MSGBOXPARAMSA msgbox;

	msgbox.cbSize = sizeof(msgbox);
	msgbox.hwndOwner = 0;
	msgbox.hInstance = 0;
	msgbox.lpszText = message;
	msgbox.lpszCaption = _("ching chong");
	msgbox.dwStyle = 0;
	msgbox.lpszIcon = NULL;
	msgbox.dwContextHelpId = 0;
	msgbox.lpfnMsgBoxCallback = NULL;
	msgbox.dwLanguageId = LANG_NEUTRAL;

	reinterpret_cast<int(__stdcall*)(const MSGBOXPARAMSA*)>(GetExpAddress(_("user32.dll"), _("MessageBoxIndirectA")))(&msgbox);
}

uintptr_t CMem::GetExpAddress(LPCTSTR DllName, LPCSTR FunctionName)
{
	PIMAGE_DOS_HEADER pIDH;
	PIMAGE_NT_HEADERS pINH;
	PIMAGE_EXPORT_DIRECTORY pIED;

	HMODULE hModule;
	PDWORD Address, Name;
	PWORD Ordinal;

	DWORD i;

	hModule = ModuleEx(DllName);

	if (!hModule)
		return 0;

	pIDH = (PIMAGE_DOS_HEADER)hModule;

	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	pINH = (PIMAGE_NT_HEADERS)((LPBYTE)hModule + pIDH->e_lfanew);

	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	if (pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0)
		return 0;

	pIED = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)hModule + pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	Address = (PDWORD)((LPBYTE)hModule + pIED->AddressOfFunctions);
	Name = (PDWORD)((LPBYTE)hModule + pIED->AddressOfNames);

	Ordinal = (PWORD)((LPBYTE)hModule + pIED->AddressOfNameOrdinals);

	for (i = 0; i < pIED->AddressOfFunctions; i++)
	{
		if (!strcmp(FunctionName, (char*)hModule + Name[i]))
		{
			return (uintptr_t)(PVOID)((LPBYTE)hModule + Address[Ordinal[i]]);
		}
	}

	return 0;
}

std::string CMem::GetProcessName ( )
{
	static auto Name = std::string ( _("" ) );

	if ( Name.length ( ) > 1 )
		return Name;

	auto Current = Call<DWORD> ( _ ( "kernel32.dll" ) , _ ( "GetCurrentProcessId" ) ).Run ( );

	if ( !Current )
		return Name;

	HANDLE Snapshot = 0;
	Snapshot = Call<HANDLE> ( _ ( "kernel32.dll" ) , _ ( "CreateToolhelp32Snapshot" ) ).Run ( TH32CS_SNAPPROCESS , 0 );

	if ( !Snapshot )
		return Name;

	PROCESSENTRY32 Entry = { sizeof ( PROCESSENTRY32 ) };

	if ( !Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "Process32First" ) ).Run ( Snapshot , &Entry ) )
		return Name;

	while ( Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "Process32Next" ) ).Run ( Snapshot , &Entry ) )
	{
		if ( Entry.th32ProcessID == Current )
		{
			Name = std::string ( Entry.szExeFile );
			break;
		}
	}

	Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "CloseHandle" ) ).Run ( Snapshot );

	return Name;
}