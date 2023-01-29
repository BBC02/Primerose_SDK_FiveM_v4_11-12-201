#include "pch.h"
#include "Client.hpp"
#include "Memory.hpp"
#include "Auth.hpp"

bool Client::SetupProcess ( )
{
	m_Process.Name = GetProcessName ( );
	m_Process.ImageBase = CMem::Get ( )->Module ( m_Process.Name.c_str() );
	m_Process.Modules = GetModules ( );

	return true;
}

bool Client::Initalize ( )
{
	//if ( !SetupProcess ( ) )
	//	return false;

	if ( !CAuth::Get ( )->Authenticate ( ) )
		return false;

	if ( !CAuth::Get ( )->UserAuthenticated ( ) )
		return false;

	return true;
}

bool Client::Shutdown ( HMODULE hInstance )
{
	_Exit ( 0 );

	return true;
}