#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <WtsApi32.h>
#include <vector>
#include <random>
#pragma comment( lib, "WtsApi32.lib" )

using namespace std;

struct Memory {
    unsigned char* Address;
    SIZE_T RegionSize;
};

void EnableDebugPerms()
{
    // Credits to ChatGPT (surprisingly)
    // This function grants the program debug permissions, which is required under certain conditions
    HANDLE token;
    LUID id;
    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token ) )
    {
        cerr << "Couldn't open process token. Aborting..." << endl;
        system( "pause" );
        exit( 1 );
    }
    if ( !LookupPrivilegeValue( nullptr, SE_DEBUG_NAME, &id ) )
    {
        cerr << "Couldn't lookup privilege value. Aborting..." << endl;
        system( "pause" );
        exit( 1 );
    }

    TOKEN_PRIVILEGES priv;
    priv.PrivilegeCount = 1;
    priv.Privileges[0].Luid = id;
    priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if ( !AdjustTokenPrivileges( token, FALSE, &priv, sizeof( TOKEN_PRIVILEGES ), nullptr, nullptr ) )
    {
        cerr << "Couldn't adjust privileges. Aborting..." << endl;
        system( "pause" );
        exit( 1 );
    }
    CloseHandle( token );
}

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
    vector<Memory> pages;
    MEMORY_BASIC_INFORMATION page;
    unsigned char* addr;
    for ( addr = NULL; VirtualQueryEx( proc, addr, &page, sizeof( page ) ) == sizeof( page ); addr += page.RegionSize )
    {
        if ( page.State == MEM_COMMIT && page.Type == MEM_PRIVATE && page.Protect == PAGE_READWRITE )
        {
            Memory mem;
            mem.Address = addr;
            mem.RegionSize = page.RegionSize;
            pages.emplace_back( mem );
        }
    }
    if ( pages.size() == 0 )
    {
        cerr << "Couldn't find any valid memory addresses for selected process. The target process may have terminated. Aborting..." << endl;
        system( "pause" );
        exit( 1 );
    }

    // Get a truly random address by picking one between the start and end of the page
    random_device rd;
    mt19937 gen( rd() );
    Memory randomMem = pages[rand() % pages.size()];
    unsigned char* endAddr = randomMem.Address + randomMem.RegionSize;
    uniform_int_distribution<uint64_t> dist( ( uint64_t ) randomMem.Address, ( uint64_t ) endAddr );
    uintptr_t randomAddr = static_cast<uintptr_t>( dist( gen ) );
    return ( unsigned char* ) randomAddr;
}

void FlipRandomBit( BYTE &data )
{
    BYTE random = rand() % 8; // 8 bits in a byte
    data ^= ( 1 << random ); // Invert random bit using bitwise XOR
}

void RunMemoryManipulation( DWORD procId )
{
    // Generate a list of currently running processes and pick a random one
    bool repeat = false;
Start:
    if ( procId <= 0 || repeat )
    {
        procId = GetRandomProcess();
    }
    cout << "Opening process " << procId << endl;
    HANDLE proc = OpenProcess( PROCESS_ALL_ACCESS, false, procId );
    if ( !proc )
    {
        cerr << "Couldn't open process " << procId << ". It's either protected by Windows or the architectures don't match. Retrying..." << endl;
        repeat = true;
        goto Start;
    }

    // Get a list of memory addresses from the selected process that actually have data, then read a random one from that list
    BYTE buffer;
    PVOID addr = GetRandomAddress( proc );
    if ( !ReadProcessMemory( proc, addr, &buffer, sizeof( buffer ), NULL ) )
    {
        cerr << "Couldn't read memory address 0x" << addr << ". Retrying..." << endl;
        CloseHandle( proc );
        repeat = true;
        goto Start;
    }

    // Flip random bit and write it to memory
    cout << "Old data at address 0x" << addr << ": 0x" << hex << uppercase << int( buffer ) << endl;
    FlipRandomBit( buffer );
    if ( !WriteProcessMemory( proc, addr, &buffer, sizeof( buffer ), NULL ) )
    {
        cerr << "Couldn't write to memory address 0x" << addr << ". Retrying..." << endl;
        CloseHandle( proc );
        repeat = true;
        goto Start;
    }
    cout << "New data at address 0x" << addr << ": 0x" << hex << uppercase << int( buffer ) << dec << endl;
    CloseHandle( proc );
}

int main()
{
    int amount = 0;
    DWORD id = 0;
    DWORD delay = 0;
    cout << "Single-event Upset Simulator (SUS) | Copyright (c) 2023 LambdaGaming" << endl;
    cout << "WARNING!!! THIS PROGRAM EDITS RANDOM BITS OF MEMORY FROM RANDOM PROCESSES, WHICH CAN SERIOUSLY SCREW UP YOUR SYSTEM! PROCEED AT YOUR OWN RISK!" << endl << endl;
    cout << "Enter the amount of random bits you want to flip: ";
    cin >> amount;
    cout << "Enter the ID of the process you want to target, or enter 0 to select a random process: ";
    cin >> id;
    cout << "Enter the amount of milliseconds you want to delay each flip, or enter 0 for no delay: ";
    cin >> delay;
    srand( time( NULL ) );

    // Make sure we have the right permissions to be doing all this
    EnableDebugPerms();

    for ( int i = 0; i < amount; i++ )
    {
        RunMemoryManipulation( id );
        Sleep( delay );
    }

    cout << "Process completed." << endl;
    system( "pause" );
}
