#pragma once

class CMem
{
public:
	static CMem* Get()
	{
		static auto Instance = new CMem;
		return Instance;
	}
private:
	wchar_t* GetFileNameFromPath(wchar_t* Path);
	wchar_t* RemoveFileExtension(wchar_t* FullFileName, wchar_t* OutputBuffer, DWORD OutputBufferSize);
	HMODULE WINAPI GetModuleW(_In_opt_ LPCWSTR lpModuleName);
public:
	uintptr_t   Module(LPCSTR moduleName);
	HMODULE     ModuleEx(LPCSTR moduleName);
	std::string Hwid();
	std::string ReadRegistry(LPCSTR subkey, LPCSTR name, DWORD type);
	void		WriteRegistry(LPCSTR subkey, LPCSTR name, DWORD type, LPCSTR data);
	uintptr_t   Pattern(const char* moduleName, const char* pattern);
	void        Box(const char* message);
	uintptr_t   GetExpAddress(LPCTSTR DllName, LPCSTR FunctionName);
	std::string GetProcessName ( );
};

template<typename T>
class Call
{
private:
	void* Address;
public:
	Call ( const char* module , const char* function ) 
	{
		Address = (void*)CMem::Get ( )->GetExpAddress ( module , function );
	}

	template<typename ... Args>
	inline T Run(Args ... args) { if (Address) { return reinterpret_cast<T(*)(Args ...)>(Address)(args...); } }
};

#define READ_REGISTRY(Subkey, Name, Type) CMem::Get()->ReadRegistry(Subkey, Name, Type)
#define WRITE_REGISTRY(Subkey, Name, Type, Data) CMem::Get()->WriteRegistry(Subkey, Name, Type, Data)

#define EXPORT(Module, Function) CMem::Get()->GetExpAddress(Module, Function)
#define CALL_ONCE(Function) { static bool _Flag = false; if (!_Flag) { Function(); _Flag = true; } }