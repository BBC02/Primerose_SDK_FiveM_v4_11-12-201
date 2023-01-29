#pragma once

typedef std::string ModuleName64;
typedef uintptr_t ModuleImageBase64;

struct Process 
{
	std::string Name;

	uintptr_t ImageBase;

	std::unordered_map<ModuleName64 , ModuleImageBase64> Modules;
};

class Client
{
	Process m_Process;
public:
	static Client* Get ( )
	{
		static auto Instance = new Client;
		return Instance;
	}
private:
	std::string GetProcessName ( );

	std::unordered_map<ModuleName64 , ModuleImageBase64> GetModules ( );
	bool SetupProcess ( );

public:
	inline Process GetProcess ( ) const { return m_Process; }

	Client ( ) {}
	~Client ( ) {}

	bool Initalize ( );
	bool Shutdown ( HMODULE hInstance );
};

