#include "Windows.h"
#include <iostream>
//#include "Shape.h"

int main()
{
    /*Create named pipe*/
    auto handle = CreateNamedPipeA(
        R"(\\.\pipe\Console)",  //the unique pipe name
        PIPE_ACCESS_OUTBOUND,   //mode: write to the pipe [DUPLEX] for bi-direction, [INBOUND] for reading
        PIPE_TYPE_BYTE|         //[BYTE]: wriiten as a stream by bytes, [MESSAGE]: written as a stream of message, 
        PIPE_READMODE_BYTE|
        PIPE_WAIT,              //operate as blocking mode, [NO_WAIT] as non-blocking
        2,                      //maximum number of instances that can be created for this pipe
        7200,                   //bytes reserve for output buffer
        0,                      //bytes for the inoput buffer
        1,
        nullptr                 //a pointer to a [SECURITY_ATTRIBUTES] struct
    );
    if(handle==INVALID_HANDLE_VALUE)
    {
        std::cerr << GetLastError();
        MessageBoxA(NULL, "CreateNamedPipe failed", "ERROR", MB_OK);
        return -1;
    }

    /*Create console for the named pipe*/
    STARTUPINFOA startUpInfo;
    GetStartupInfoA(&startUpInfo);
    PROCESS_INFORMATION processInfo;
    if(   
        !CreateProcessA(
           nullptr,            //application name
           "ConsoleAssist.exe Console",      //CommandLine
           nullptr,            //process attributes
           nullptr,            //thread attributes
           false,              //inherit handles
           CREATE_NEW_CONSOLE, //creation flags
           nullptr,            //use the environment for the calling process
           nullptr,            //same current directory
           &startUpInfo,
           &processInfo
        )
    )
    {
         MessageBoxA(NULL, "Create Process failed", "ERROR", MB_OK);
         CloseHandle(handle);
         handle = INVALID_HANDLE_VALUE;
         return EXIT_FAILURE;
    }

    /*Connect named pipe -> process */
    if (!ConnectNamedPipe(handle, nullptr))
    {
        MessageBoxA(NULL, "ConnectNamedPipe failed", "ConsoleLogger failed", MB_OK);

        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
        return EXIT_FAILURE;
    }

    /*Test write to the console */
    char buffer[] = "Test writing";
    std::cout << "Sending \"" << buffer << "\" -> console 2.\n";
    DWORD bytes_written{};
    if(!WriteFile(handle, buffer, strlen(buffer), &bytes_written, nullptr))
    {
        puts("Test writting failed");
        CloseHandle(handle);
    }


    std::cout << "Finished\n";
    std::cin.get();
}