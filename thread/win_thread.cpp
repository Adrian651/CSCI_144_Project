#include <iostream>
#include <windows.h>

using namespace std;

class ThreadInfo
{
    public:
        int a, b, c;
};

//Thread() receives a pointer to threadinfo
long WINAPI Thread(ThreadInfo *threadinfo)                                    
{
    threadinfo -> a = 100;
    threadinfo -> b = 200;

    //we change around with the values in threadinfo
    threadinfo -> c = threadinfo -> a * threadinfo -> b;
    
    cout << "I am running..." << endl;
}

 

int main()
{
    ThreadInfo threadinfo;

    //Create the thread
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, &threadinfo, 0, 0);

    //Wait for a moment to let the thread finish executing
    Sleep(100);

    //print threadinfo.c on the screen
    cout << threadinfo.c << endl;

    //Pause for 3 seconds                                           
    Sleep(3000);
}
