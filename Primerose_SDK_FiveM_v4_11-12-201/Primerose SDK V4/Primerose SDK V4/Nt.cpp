#include "pch.h"
#include "Nt.hpp"
#include "Memory.hpp"

typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID , * PCLIENT_ID;

void Nt::Thread ( void* Routine)
{
    if ( !Routine )
        return;

    static auto RtlCreateUserThread = Call<LONG> ( _ ( "ntdll.dll" ) , _ ( "RtlCreateUserThread" ) );
    RtlCreateUserThread.Run< HANDLE , PSECURITY_DESCRIPTOR , BOOLEAN , ULONG , SIZE_T , SIZE_T , PTHREAD_START_ROUTINE , PVOID , PHANDLE , PCLIENT_ID> ( ( ( HANDLE ) ( LONG_PTR ) -1 ) , 0 , 0 , 0 , 0 , 0 , ( PTHREAD_START_ROUTINE ) Routine , 0 , 0 , 0 );
}

void Nt::SpoofThread ( void* Routine )
{
    if ( !Routine )
        return;

    uintptr_t SpoofedAddress = 0;
    int ThreadSize = 1000;

    srand ( ( unsigned ) time ( 0 ) );

    for ( int i = 0; i < 4; i++ )
    {
        SpoofedAddress |= ( rand ( ) & 0xFF ) << i * 8;
        SpoofedAddress |= ( rand ( ) & 0xFF ) << i * 8;
        SpoofedAddress |= ( rand ( ) & 0xFF ) << i * 8;
    }

    while ( SpoofedAddress > 0x7FFFFFFF )
        SpoofedAddress -= 0x1000;

    Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "VirtualProtect" ) ).Run ( ( void* ) SpoofedAddress , ThreadSize , PAGE_EXECUTE_READWRITE , 0 );

    CONTEXT ThreadContext = { };
    HANDLE ThreadHandle = nullptr;

    static auto RtlCreateUserThread = Call<LONG> ( _ ( "ntdll.dll" ) , _ ( "RtlCreateUserThread" ) );
    RtlCreateUserThread.Run< HANDLE , PSECURITY_DESCRIPTOR , BOOLEAN , ULONG , SIZE_T , SIZE_T , PTHREAD_START_ROUTINE , PVOID , PHANDLE , PCLIENT_ID> ( ( ( HANDLE ) ( LONG_PTR ) -1 ) , 0 , 0 , 0 , 0 , 0 , ( PTHREAD_START_ROUTINE ) Routine , 0 , &ThreadHandle , 0 );

    ThreadContext.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
    
    Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "GetThreadContext" ) ).Run ( ThreadHandle , &ThreadContext );

    ThreadContext.Rcx = reinterpret_cast< DWORD64 >( Routine );
    ThreadContext.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;

    Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "SetThreadContext" ) ).Run ( ThreadHandle, &ThreadContext );
    Call<BOOL> ( _ ( "kernel32.dll" ) , _ ( "ResumeThread" ) ).Run ( ThreadHandle );
}