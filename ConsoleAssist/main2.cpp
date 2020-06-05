#include <Windows.h>
#include <cstdio>

int main(int argc, char *argv[])
{
    // if (argc == 1 || !argv[1] || !argv[1][0])
    // {
    //     MessageBoxA(NULL, "\nFailed to start logger\n", "FAILED", MB_OK);
    //     return -1;
    // }

    char szPipename[64];
    sprintf(szPipename, "\\\\.\\pipe\\%s", argv[1]);

    HANDLE pipe_handle;
    while(true)
    {
        pipe_handle = CreateFileA(
            szPipename,                                         // pipe name
            GENERIC_READ /*|GENERIC_WRITE*/ /* GENERIC_WRITE*/, // read and write access
            0,                                                  // no sharing
            NULL,                                               // no security attributes
            OPEN_EXISTING,                                      // opens existing pipe
            0,                                                  // default attributes
            NULL);                                              // no template file

        if (pipe_handle != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            MessageBoxA(NULL, "Could not open pipe(1)", "FAILED", MB_OK);
            getchar();
            return 0;
        }

        // All pipe instances are busy, so wait for 20 seconds.
        printf("Wait for pipe...");
        if (!WaitNamedPipeA(szPipename, 20000))
        {
            MessageBoxA(NULL, "Could not open pipe(2)", "FAILED", MB_OK);
            getchar();
            return 0;
        }
        Sleep(500);
    }

    char c[256];
    DWORD bytes_read;
    if(!ReadFile(
        pipe_handle, // pipe handle
        c,      // buffer to receive reply
        256,       // size of buffer
        &bytes_read, // number of bytes read
        NULL)   // not overlapped
    )
    {
        MessageBoxA(NULL, "ReadFile failed", "FAILED", MB_OK);
        return -1; // stop only on failure
    }

    c[bytes_read] = '\0';
    puts(c);
    CloseHandle(pipe_handle);
    getchar();
}