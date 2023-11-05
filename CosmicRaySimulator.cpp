#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <WtsApi32.h>
#include <vector>
#pragma comment( lib, "WtsApi32.lib" )
#define PAGESIZE 4096

using namespace std;

DWORD GetRandomProcess()
{
    WTS_PROCESS_INFO *processes = NULL;
    DWORD count = 0;
    srand( time( NULL ) );
    if ( WTSEnumerateProcesses( WTS_CURRENT_SERVER_HANDLE, NULL, 1, &processes, &count ) )
    {
        int random = rand() % count;
        return processes[random].ProcessId;
    }
    if ( processes )
    {
        WTSFreeMemory( processes );
    }
    return 0;
}

unsigned char* GetRandomAddress( HANDLE proc )
{
    vector<unsigned char*> pages;
    MEMORY_BASIC_INFORMATION page;
    unsigned char* addr;
    for ( addr = NULL; VirtualQueryEx( proc, addr, &page, sizeof( page ) ) == sizeof( page ); addr += page.RegionSize )
    {
        if ( page.State == MEM_COMMIT && page.Type == MEM_PRIVATE && page.Protect == PAGE_READWRITE )
        {
            pages.emplace_back( addr );
        }
    }
    int random = rand() % pages.size();
    return pages[random];
}

int main()
{
    DWORD procId = GetRandomProcess();
    cout << "Opening process " << procId << endl;
    HANDLE proc = OpenProcess( PROCESS_ALL_ACCESS, false, procId );
    if ( !proc )
    {
        cerr << "Couldn't open process " << procId << ". Try restarting the program as admin." << endl;
        exit( 1 );
    }

    char buffer[PAGESIZE];
    int data = int( buffer );
    PVOID addr = GetRandomAddress( proc );
    if ( !ReadProcessMemory( proc, addr, buffer, PAGESIZE, NULL ) )
    {
        cerr << "Couldn't read memory address 0x" << addr << ". Try restarting the program as admin." << endl;
        exit( 1 );
    }

    cout << "Old data at address 0x" << addr << ": 0x" << hex << uppercase << int( buffer ) << endl;
    data = ~data;
    cout << "New data at address 0x" << addr << ": 0x" << hex << uppercase << int( ( char* ) data ) << endl; 
    if ( !WriteProcessMemory( proc, addr, ( char* ) data, PAGESIZE, NULL) )
    {
        cerr << "Couldn't write memory address 0x" << addr << ". Try restarting the program as admin." << endl;
    }

    CloseHandle( proc );
}
