#include <cstdint>
#include <iostream>
#include <random>
#include <sys/ptrace.h>
#include <dirent.h>
#include <vector>
#include <chrono>
#include <thread>
#include "pmparser.h"

using namespace std;

// Get random process from /proc directory
int GetRandomProcess()
{
    vector<int> procs;
    struct dirent *de;
    DIR *dir = opendir( "/proc" );
    while ( dir != NULL && ( de = readdir( dir ) ) != NULL )
    {
        const char* name = de->d_name;
        bool good = true;
        for ( ; *name; name++ )
        {
            // Make sure the name is a number
            if ( *name < '0' || *name > '9' )
            {
                good = false;
            }
        }
        if ( good )
        {
            procs.push_back( atoi( de->d_name ) );
        }
    }
    int random = rand() % procs.size();
    return procs[random];
}

int* GetRandomAddress( int pid )
{
    procmaps_iterator maps = { 0 };
    procmaps_error_t err = PROCMAPS_SUCCESS;
    err = pmparser_parse( pid, &maps );
    vector<procmaps_struct*> mapList;

    if ( err )
    {
        cout << "Couldn't parse memory map for process " << pid << endl;
        return ( int* ) -1;
    }

    procmaps_struct *region = NULL;
    while ( ( region = pmparser_next( &maps ) ) != NULL )
    {
        if ( region->is_w )
        {
            mapList.push_back( region );
        }
    }
    pmparser_free( &maps );

    if ( mapList.size() == 0 )
    {
        cout << "Couldn't find any valid memory addresses for process " << pid << endl;
        return ( int* ) -1;
    }

    random_device rd;
    mt19937 gen( rd() );
    procmaps_struct *randomMem = mapList[rand() % mapList.size()];
    uniform_int_distribution<uint64_t> dist( ( uint64_t ) randomMem->addr_start, ( uint64_t ) randomMem->addr_end );
    intptr_t randomAddr = static_cast<intptr_t>( dist( gen ) );
    return ( int* ) randomAddr;
}

void FlipRandomBit( int &data )
{
    int random = rand() % 32; // 32 bits in this case (I think?)
    data ^= ( 1 << random ); // Invert random bit using bitwise XOR
}

void RunMemoryManipulation( int id )
{
    bool repeat = false;
Start:
    if ( id <= 0 || repeat )
    {
        id = GetRandomProcess();
    }

    cout << "Opening process " << id << endl;
    int *addr = GetRandomAddress( id );
    /*if ( *addr <= 0 ) // Causes segfault for some reason?
    {
        repeat = true;
        goto Start;
    }*/

    ptrace( PTRACE_ATTACH, id, NULL, NULL );
    int data = ptrace( PTRACE_PEEKDATA, id, addr );
    cout << "Old data at address " << hex << uppercase << addr << ": " << data << endl;
    FlipRandomBit( data );
    ptrace( PTRACE_POKEDATA, id, addr, data );
    cout << "New data at address " << hex  << uppercase << addr << ": " << data << endl;
    ptrace( PTRACE_DETACH, id, NULL, NULL );
}

int main()
{
    int amount = 0;
    int id = 0;
    int delay = 0;
    cout << "Single-event Upset Simulator (Linux Edition) | Copyright (c) 2024 OPGman" << endl;
    cout << "WARNING!!! THIS PROGRAM EDITS RANDOM BITS OF MEMORY FROM RANDOM PROCESSES, WHICH CAN SERIOUSLY SCREW UP YOUR SYSTEM! PROCEED AT YOUR OWN RISK!" << endl << endl;
    cout << "Enter the amount of random bits you want to flip: ";
    cin >> amount;
    cout << "Enter the ID of the process you want to target, or enter 0 to select a random process: ";
    cin >> id;
    cout << "Enter the amount of milliseconds you want to delay each flip, or enter 0 for no delay: ";
    cin >> delay;
    srand( time( NULL ) );

    for ( int i = 0; i < amount; i++ )
    {
        RunMemoryManipulation( id );
        this_thread::sleep_for( chrono::milliseconds( delay ) );
    }

    cout << "Process completed." << endl;
    cin.get();
    return 0;
}
