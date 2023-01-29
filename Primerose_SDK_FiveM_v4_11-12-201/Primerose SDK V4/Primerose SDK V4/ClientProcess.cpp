#include "pch.h"
#include "Client.hpp"
#include "Memory.hpp"

std::string Client::GetProcessName ( )
{
	static auto name = std::string ( );
	static auto id = Call<DWORD> ( _ ( "kernel32.dll" ) , _ ( "GetCurrentProcessId" ) ).Run ( );

	if ( !id )
		return std::string ( );

	HANDLE handle = 0;
	handle = Call<HANDLE> ( _ ( "kernel32.dll" ) , _ ( "CreateToolhelp32Snapshot" ) ).Run ( TH32CS_SNAPPROCESS , 0 );

	if ( !handle )
		return std::string ( );

	auto pe32 = PROCESSENTRY32 { sizeof ( PROCESSENTRY32 ) };

	if ( !Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "Process32First" ) ).Run ( handle , &pe32 ) )
		return std::string ( );

	while ( Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "Process32Next" ) ).Run ( handle , &pe32 ) )
	{
		if ( pe32.th32ProcessID == id )
		{
			name = std::string ( pe32.szExeFile );
			break;
		}
	}

	Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "CloseHandle" ) ).Run ( handle );

	return name;
}

std::unordered_map<ModuleName64 , ModuleImageBase64> Client::GetModules ( )
{
	std::unordered_map<ModuleName64 , ModuleImageBase64> results = { };

	static auto id = Call<DWORD> ( _ ( "kernel32.dll" ) , _ ( "GetCurrentProcessId" ) ).Run ( );

	if ( !id )
		return results;

	HANDLE handle = 0;
	handle = Call<HANDLE> ( _ ( "kernel32.dll" ) , _ ( "CreateToolhelp32Snapshot" ) ).Run ( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 , id );

	if ( !handle )
		return results;

	auto me32 = MODULEENTRY32{ sizeof ( MODULEENTRY32 ) };

	if ( !Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "Module32First" ) ).Run ( handle , &me32 ) )
		return results;

	while ( Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "Module32Next" ) ).Run ( handle , &me32 ) )
	{
		results.insert ( { me32.szModule, ( uintptr_t ) me32.modBaseAddr } );
	}

	Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "CloseHandle" ) ).Run ( handle );

	return results;
}