#include "Console.h"
#include "Command.h"

Console::Console()
{
    is_std = count++ ==0? true:false;

    if(is_std)
    {
        hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
        if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) == 0)
            throw ConsoleException{"GetConsoleScreenBuffer error!"};
        width = screen_buffer_info.srWindow.Right - screen_buffer_info.srWindow.Left + 1;
        height = screen_buffer_info.srWindow.Bottom - screen_buffer_info.srWindow.Top + 1;
        default_attrib = screen_buffer_info.wAttributes;
        current_background_color = default_attrib & static_cast<WORD>(BackgroundColor::WHITE);
        current_foreground_color = default_attrib & static_cast<WORD>(Color::WHITE);
    }
    else /*if index != 0, create new console window*/
    {
        /*Create named pipe*/
        hTerminal = CreateNamedPipeA(
            R"(\\.\pipe\Console)", //the unique pipe name
            PIPE_ACCESS_OUTBOUND,  //mode: write to the pipe [DUPLEX] for bi-direction, [INBOUND] for reading
            PIPE_TYPE_BYTE |       //[BYTE]: wriiten as a stream by bytes, [MESSAGE]: written as a stream of message,
                PIPE_READMODE_BYTE |
                PIPE_WAIT, //operate as blocking mode, [NO_WAIT] as non-blocking
            2,             //maximum number of instances that can be created for this pipe
            7200,          //bytes reserve for output buffer
            0,             //bytes for the inoput buffer
            1,
            nullptr //a pointer to a [SECURITY_ATTRIBUTES] struct
        );
        if(hTerminal==INVALID_HANDLE_VALUE)
            throw ConsoleException{"Create named pipe error! Code: " + GetLastError()};

        /*Create console for the named pipe*/
        STARTUPINFOA startUpInfo;
        GetStartupInfoA(&startUpInfo);
        PROCESS_INFORMATION processInfo;
        if (
            !CreateProcessA(
                nullptr,                     //application name
                "ConsoleAssist.exe Console", //CommandLine
                nullptr,                     //process attributes
                nullptr,                     //thread attributes
                false,                       //inherit handles
                CREATE_NEW_CONSOLE,          //creation flags
                nullptr,                     //use the environment for the calling process
                nullptr,                     //same current directory
                &startUpInfo,
                &processInfo))
        {
            MessageBoxA(NULL, "Create Process failed", "ERROR", MB_OK);
            CloseHandle(hTerminal);
            throw ConsoleException{"Create Process error! Code: " + GetLastError()};
        }

        /*Connect named pipe -> process */
        if (!ConnectNamedPipe(hTerminal, nullptr))
        {
            MessageBoxA(NULL, "ConnectNamedPipe failed", "ConsoleLogger failed", MB_OK);
            CloseHandle(hTerminal);
            throw ConsoleException{"Connect named pipe error! Code: " + GetLastError()};

        }

        /*Test write to the console */
        char buffer[] = "Test writing";
        std::cout << "Sending \"" << buffer << "\" -> console 2.\n";
        DWORD bytes_written{};
        if (!WriteFile(hTerminal, buffer, strlen(buffer), &bytes_written, nullptr))
        {
            puts("Test writting failed");
            CloseHandle(hTerminal);
        }
    }
}

void Console::printConsoleInfo() const
{
    if(is_std)  
    { 
        CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
        if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
            std::cout
                << "Buffer Size: " << screen_buffer_info.dwSize.X << ", " << screen_buffer_info.dwSize.Y << '\n'
                << "Cursor Pos: " << screen_buffer_info.dwCursorPosition.X << ", " << screen_buffer_info.dwCursorPosition.Y << '\n'
                << "Window: (" << screen_buffer_info.srWindow.Top << ',' << screen_buffer_info.srWindow.Left << "), (" << screen_buffer_info.srWindow.Bottom << ',' << screen_buffer_info.srWindow.Right << ")\n"
                << "Window Size: " << screen_buffer_info.dwMaximumWindowSize.X << ", " << screen_buffer_info.dwMaximumWindowSize.Y << '\n';
        else
            throw ConsoleException{"GetConsoleScreenBuffer error!"};
    }
    else
        sendPipe(PRINT_CONSOLE_INFO);
}

Console& Console::set(Color color, bool intensify)
{
    if (color == Color::NOCHANGE)
        return *this;
    
    current_foreground_color = static_cast<WORD>(color);
    if (intensify)
        current_foreground_color |= FOREGROUND_INTENSITY;

    setColor(current_background_color | current_foreground_color);
    return *this;
}

