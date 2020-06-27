#include "Console.h"
#include "Command.h"
#include <thread>	//for getKey

Console::Console()
{
    is_std = (count++ == 0);

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

Console& Console::set(BackgroundColor bgColor, bool intensify)
{
    if (bgColor == BackgroundColor::NOCHANGE)
        return *this;
    current_background_color = static_cast<WORD>(bgColor);
    if (intensify)
        current_background_color |= BACKGROUND_INTENSITY;

    setColor(current_foreground_color | static_cast<WORD>(current_background_color));
    return *this;
}

void Console::changeWindowColor(BackgroundColor bgColor)
{
    if (bgColor == BackgroundColor::NOCHANGE)
        return;
    auto length = width * height;
    DWORD actual_written = 0;
    FillConsoleOutputCharacterA(hTerminal, static_cast<CHAR>(bgColor), length, { 0,0 }, &actual_written);
}

void Console::backspace(short count)
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
    {
        if (SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X - count, screen_buffer_info.dwCursorPosition.Y }) == 0)
            LOG("Set Console Cursor Position failed!\n");
        while (count-- > 0)
            std::cout << ' ';
    }
    else
        LOG("Get console screen buffer info failed!\n");
}

void Console::moveCursor(short count, Direction direction)
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
    {
        bool success = true;
        switch (direction)
        {
        case Direction::UP:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X, screen_buffer_info.dwCursorPosition.Y > count ? screen_buffer_info.dwCursorPosition.Y - count : 0 });
            break;
        case Direction::DOWN:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X, screen_buffer_info.dwCursorPosition.Y + count });
            break;
        case Direction::LEFT:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X > count ? screen_buffer_info.dwCursorPosition.X - count : 0, screen_buffer_info.dwCursorPosition.Y });
            break;
        case Direction::RIGHT:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X + count, screen_buffer_info.dwCursorPosition.Y });
            break;
        }
        if (!success)
            LOG("Move cursor error!\n");
    }
    else
        LOG("Get console screen buffer info failed!\n");
}

void Console::moveCursorTo(MIDDLE)
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info);
    SetConsoleCursorPosition(hTerminal, { width / 2, screen_buffer_info.dwCursorPosition.Y });
}

COORD Console::getCursorPos() const
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hTerminal, &info);
    return { info.dwCursorPosition.X, info.dwCursorPosition.Y };
}

void Console::eraseLine()
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    DWORD count;	//If the pointer to the last parameter is set to nullptr, there will be an exception
    if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
    {
        if (screen_buffer_info.dwCursorPosition.X == 0)
        {
            FillConsoleOutputCharacter(hTerminal, ' ', screen_buffer_info.srWindow.Right, { 0, screen_buffer_info.dwCursorPosition.Y - 1 }, &count);//erase the last line
            SetConsoleCursorPosition(hTerminal, { 0, screen_buffer_info.dwCursorPosition.Y - 1 });
        }
        else
        {
            FillConsoleOutputCharacter(hTerminal, ' ', screen_buffer_info.dwCursorPosition.Y + 1, { 0, screen_buffer_info.dwCursorPosition.Y }, &count);//erase current line
            SetConsoleCursorPosition(hTerminal, { 0, screen_buffer_info.dwCursorPosition.Y });
        }
    }
    else
        LOG("Error in erase line!\n");
}

void Console::clear()
{
    DWORD actual_written = 0;
    FillConsoleOutputCharacter(hTerminal, ' ', width * height, { 0,0 }, &actual_written);
    SetConsoleCursorPosition(hTerminal, { 0,0 });
}

FunctionKey Console::getKey()
{
    while (true)
    {
        SHORT keys[static_cast<int>(FunctionKey::COUNT)]{};
        for (auto i = 0; i < static_cast<int>(FunctionKey::COUNT); ++i)
        {
            keys[i] = GetAsyncKeyState(KeyCode[static_cast<FunctionKey>(i)]);
            if ((keys[i] & 1) == 1)
                return static_cast<FunctionKey>(i);
        }
        std::this_thread::sleep_for(std::chrono::microseconds{ 10 });
    }
    return FunctionKey::COUNT;
}

void Console::hideCursor()
{
    CONSOLE_CURSOR_INFO prev;
    GetConsoleCursorInfo(hTerminal, &prev);
    prev.bVisible = false;
    if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
        std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
}

void Console::showCursor()
{
    CONSOLE_CURSOR_INFO prev;
    GetConsoleCursorInfo(hTerminal, &prev);
    prev.bVisible = true;
    if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
        std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
}

Console::~Console()
{
    SetConsoleTextAttribute(hTerminal, default_attrib);
    CloseHandle(hTerminal);
}

#include <new>
static char ConsoleStorage[sizeof(Console)];
class Console& console = reinterpret_cast<class Console&>(ConsoleStorage);
unsigned ConsoleInitializer::count;

void ConsoleInitializer::init()
{
    new(ConsoleStorage) Console{};
}

void ConsoleInitializer::cleanUp()
{
    console.~Console();
}

ConsoleInitializer::ConsoleInitializer()
{
    if (count++ == 0)
        init();
}

ConsoleInitializer::~ConsoleInitializer()
{
    if (--count == 0)
        cleanUp();
}