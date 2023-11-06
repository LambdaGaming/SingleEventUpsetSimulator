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
    if ( pages.size() == 0 )
    {
        cerr << "Couldn't find any valid memory addresses for selected process. Possible mismatching architectures?" << endl;
        system( "pause" );
        exit( 1 );
    }
    int random = rand() % pages.size();
    return pages[random];
}

void FlipRandomBit( int &data )
{
    int random = rand() % sizeof( int ) * 8; // 4 bytes * 8 bits = 32 bits
    data ^= ( 1 << random ); // Invert random bit using bitwise XOR
}

int main()
{
    // Generate a list of currently running processes and pick a random one
    srand( time( NULL ) );
    DWORD procId = GetRandomProcess();
    cout << "Opening process " << procId << endl;
    HANDLE proc = OpenProcess( PROCESS_ALL_ACCESS, false, procId );
    if ( !proc )
    {
        cerr << "Couldn't open process " << procId << ". Either the program isn't running as admin or the selected process is protected." << endl;
        system( "pause" );
        exit( 1 );
    }

    // Get a list of memory addresses from the selected process that actually have data, then read a random one from that list
    char buffer[PAGESIZE];
    PVOID addr = GetRandomAddress( proc );
    if ( !ReadProcessMemory( proc, addr, buffer, PAGESIZE, NULL ) )
    {
        cerr << "Couldn't read memory address 0x" << addr << "." << endl;
        system( "pause" );
        exit( 1 );
    }

    // Flip random bit and write it to memory
    int data = int( buffer );
    cout << "Old data at address 0x" << addr << ": 0x" << hex << uppercase << int( buffer ) << endl;
    FlipRandomBit( data );
    cout << hex << data << endl;
    sprintf_s( buffer, "0%X", data );
    /*if ( !WriteProcessMemory( proc, addr, buffer, PAGESIZE, NULL ) )
    {
        cerr << "Couldn't write to memory address 0x" << addr << "." << endl;
        system( "pause" );
        exit( 1 );
    }*/
    cout << "New data at address 0x" << addr << ": 0x" << hex << uppercase << int( buffer ) << endl;
    CloseHandle( proc );
    system( "pause" );
}
