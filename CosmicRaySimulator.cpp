#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <WtsApi32.h>
#pragma comment( lib, "WtsApi32.lib" )

using namespace std;

DWORD GetRandomProcess()
{
    WTS_PROCESS_INFO *processes = NULL;
    DWORD count = 0;
    srand( time( NULL ) );
    if ( WTSEnumerateProcesses( WTS_CURRENT_SERVER_HANDLE, NULL, 1, &processes, &count ) )
    {
        int random = rand() % count + 1;
        return processes[random].ProcessId;
    }
    return 0;
}

int main()
{
    DWORD procId = GetRandomProcess();
    cout << "Modified process: " << procId << endl;
}
