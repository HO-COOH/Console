#include "Console.h"
#include "Command.h"
#include <thread>	//for getKey

void Console::constructSecondaryConsole()
{
#ifdef WINDOWS
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
    if (hTerminal == INVALID_HANDLE_VALUE)
        throw ConsoleException{ "Create named pipe error! Code: " + GetLastError() };

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
        throw ConsoleException{ "Create Process error! Code: " + GetLastError() };
    }

    /*Connect named pipe -> process */
    if (!ConnectNamedPipe(hTerminal, nullptr))
    {
        MessageBoxA(NULL, "ConnectNamedPipe failed", "ConsoleLogger failed", MB_OK);
        CloseHandle(hTerminal);
        throw ConsoleException{ "Connect named pipe error! Code: " + GetLastError() };

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
#else//LINUX

#endif
}

void Console::constructStdConsole()
{
#ifdef WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) == 0)
        throw ConsoleException{ "GetConsoleScreenBuffer error!" };
    width = screen_buffer_info.srWindow.Right - screen_buffer_info.srWindow.Left + 1;
    height = screen_buffer_info.srWindow.Bottom - screen_buffer_info.srWindow.Top + 1;
    default_attrib = screen_buffer_info.wAttributes;
    current_background_color = default_attrib & static_cast<WORD>(BackgroundColor::WHITE);
    current_foreground_color = default_attrib & static_cast<WORD>(Color::WHITE);
#else
    getmaxyx(hTerminal, height, width);
#endif
}

Console::Console()
{
    is_std = (count++ == 0);
    if (is_std)
    {
#ifdef WINDOWS
        hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
#else
        initscr();
        hTerminal = stdscr;
        start_color();
#endif
        constructStdConsole();
    }
    else /*if index != 0, create new console window*/
        constructSecondaryConsole();
}

Console& Console::resetConsole(short width, short height, short fontWidth, short fontHeight)
{
    if (is_std)
    {
#ifdef WINDOWS
        SMALL_RECT windowSize{ 0,0,width - 1, height - 1 };
        //If the function fails return value is 0
        if (!SetConsoleWindowInfo(hTerminal, true, &windowSize))
            throw ConsoleException{ "Construct Console Window failed" };

        /*set font size*/
        CONSOLE_FONT_INFOEX fontInfo;
        fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);  //Must be set before calling GetCurrentConsoleFontEX() or it will fail
        if(!GetCurrentConsoleFontEx(hTerminal, 
            false,     //If this parameter is TRUE, font information is retrieved for the maximum window size. If this parameter is FALSE, font information is retrieved for the current window size.
           &fontInfo))
            throw ConsoleException{ "Get Current Console font failed" };
        fontInfo.dwFontSize = { fontWidth, fontHeight };
        if (!SetCurrentConsoleFontEx(hTerminal, false, &fontInfo))
            throw ConsoleException{ "Set Current Console font failed" };
#else
        /*
            Ncurses doesn't support changing font size :-(
            There are 2 ambiguous functions:
                int resizeterm(int lines, int cols);
                int resize_term(int lines, int cols);
            According to: https://linux.die.net/man/3/resizeterm
                resize_term() basically does more book-keeping than resizeterm() to handle a signal.
        */
        if(resizeterm(height, width)!=OK)
            LOG("Error in resize term");
#endif
        constructStdConsole();
    }
    else
        constructSecondaryConsole();    //TODO: enable same functionality to the secondary console window
}

void Console::printConsoleInfo() const
{
    if(is_std)  
    {
#ifdef WINDOWS 
        CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
        if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
            std::cout
                << "Buffer Size: " << screen_buffer_info.dwSize.X << ", " << screen_buffer_info.dwSize.Y << '\n'
                << "Cursor Pos: " << screen_buffer_info.dwCursorPosition.X << ", " << screen_buffer_info.dwCursorPosition.Y << '\n'
                << "Window: (" << screen_buffer_info.srWindow.Top << ',' << screen_buffer_info.srWindow.Left << "), (" << screen_buffer_info.srWindow.Bottom << ',' << screen_buffer_info.srWindow.Right << ")\n"
                << "Window Size: " << screen_buffer_info.dwMaximumWindowSize.X << ", " << screen_buffer_info.dwMaximumWindowSize.Y << '\n';
        else
            throw ConsoleException{"GetConsoleScreenBuffer error!"};
#else
        COORD cursorPos;
        getyx(hTerminal, cursorPos.y, cursorPos.x);
        std::cout
            << "Cursor Pos: " << cursorPos.x << ", " << cursorPos.y << '\n'
            << "Window Size: " << width << ", " << height << '\n';
#endif
    }
    else
        sendPipe(PRINT_CONSOLE_INFO);
}

Console& Console::set(Color color, bool intensify)
{
    if (color == Color::NOCHANGE)
        return *this;
#ifdef WINDOWS    
    current_foreground_color = static_cast<WORD>(color);
    if (intensify)
        current_foreground_color |= FOREGROUND_INTENSITY;

    setColor(current_background_color | current_foreground_color);
#else
    init_pair(1, static_cast<short>(color), static_cast<short>(current_background_color));
    attron(COLOR_PAIR(1));
    current_foreground_color = static_cast<short>(color);
#endif
    return *this;
}

Console& Console::set(BackgroundColor bgColor, bool intensify)
{
    if (bgColor == BackgroundColor::NOCHANGE)
        return *this;
#ifdef WINDOWS
    current_background_color = static_cast<WORD>(bgColor);
    if (intensify)
        current_background_color |= BACKGROUND_INTENSITY;

    setColor(current_foreground_color | static_cast<WORD>(current_background_color));
#else
    init_pair(1, static_cast<short>(current_foreground_color), static_cast<short>(bgColor));
    attron(COLOR_PAIR(1));
    current_background_color = static_cast<short>(bgColor);
#endif
    return *this;
}

#ifdef LINUX
/**
 * @brief A helper function to fill a rectangular region with color for linux
*/
static void fill(COORD from, COORD to)
{
    for (auto row = from.y; row < to.y; ++row)
    {
        for (auto col = from.x; col < to.x; ++col)
        {
            auto const text = mvinch(row, col);
            mvaddch(row, col, static_cast<char>(text & A_CHARTEXT));
        }
    }
    refresh();
}
#endif

void Console::changeWindowColor(BackgroundColor bgColor)
{
    if (bgColor == BackgroundColor::NOCHANGE)
        return;
#ifdef WINDOWS
    auto length = width * height;
    DWORD actual_written = 0;
    FillConsoleOutputCharacterA(hTerminal, static_cast<CHAR>(bgColor), length, { 0,0 }, &actual_written);
#else
    set(bgColor);
    moveCursorTo({ 0,0 });
    fill({ 0,0 }, { width, height });
    refresh();
#endif
}

void Console::backspace(short count)
{
    //TODO: refactor this to support multiple lines
#ifdef WINDOWS
    auto pos = getCursorPos();
    DWORD actual_written = 0;
    FillConsoleOutputCharacterA(hTerminal, ' ', pos.X + 1, { 0,pos.Y }, &actual_written);
#else
    auto pos = getCursorPos();
    moveCursorTo({ 0, pos.y });
    for (auto i = 0; i < pos.x; ++i)
        addch(' ');
    moveCursorTo({ 0, pos.y });
    refresh();
#endif
}

void Console::moveCursor(short count, Direction direction)
{
#ifdef WINDOWS
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
#else
    int x, y;
    getyx(hTerminal, y, x);
    int success{};
    switch (direction)
    {
    case Direction::UP:
        success = move(std::min(0, y - count), x);
        break;
    case Direction::DOWN:
        success = move(std::min(static_cast<int>(height), y + count), x);
        break;
    case Direction::LEFT:
        success = move(y, std::min(0, x - count));
        break;
    case Direction::RIGHT:
        success = move(y, std::min(static_cast<int>(width), x + count));
        break;
    }
    if(success!=OK)
        LOG("Move Cursor Error");
    refresh();
#endif
}

void Console::moveCursorTo(MIDDLE)
{
#ifdef WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info);
    SetConsoleCursorPosition(hTerminal, { width / 2, screen_buffer_info.dwCursorPosition.Y });
#else
    int x, y;
    getmaxyx(hTerminal, y, x);
    if(move(y, x/2)!=OK)
        LOG("Move Cursor Error");
    refresh();
#endif
}

COORD Console::getCursorPos() const
{
#ifdef WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hTerminal, &info);
    return { info.dwCursorPosition.X, info.dwCursorPosition.Y };
#else
    COORD pos;
    getyx(hTerminal, pos.y, pos.x);
    return pos;
#endif
}

void Console::eraseLine()
{
#ifdef WINDOWS
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
#else
    const auto pos = getCursorPos();
    if (pos.x == 0)
    {
        /*move to the upper line*/
        int length = 0;
        while (!isspace(static_cast<char>(mvinch(pos.y - 1, length) & A_CHARTEXT)))
            ++length;
        backspace(length);
        moveCursorTo({ 0, pos.y - 1 });
    }
    else
        backspace(pos.y);
    refresh();
#endif
}

void Console::deleteTo(COORD position)
{
    //TODO
}

void Console::clear()
{
#ifdef WINDOWS
    DWORD actual_written = 0;
    FillConsoleOutputCharacter(hTerminal, ' ', width * height, { 0,0 }, &actual_written);
    SetConsoleCursorPosition(hTerminal, { 0,0 });
#else

#endif
}

FunctionKey Console::getKey()
{
#ifdef WINDOWS
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
#else
#endif
}

void Console::hideCursor()
{
#ifdef WINDOWS
    CONSOLE_CURSOR_INFO prev;
    GetConsoleCursorInfo(hTerminal, &prev);
    prev.bVisible = false;
    if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
        std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
#else
    //The curs_set routine sets the cursor state is set to invisible, normal, or very visible for visibility equal to 0, 1, or 2 respectively
    curs_set(0);
#endif
}

void Console::showCursor()
{
#ifdef WINDOWS
    CONSOLE_CURSOR_INFO prev;
    GetConsoleCursorInfo(hTerminal, &prev);
    prev.bVisible = true;
    if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
        std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
#else
    curs_set(1);
#endif
}

Console::~Console()
{
#ifdef WINDOWS
    SetConsoleTextAttribute(hTerminal, default_attrib);
    CloseHandle(hTerminal);
#else
    endwin();
#endif
}



/*
    The following code is the nifty-counter idiom applied for constructing a global console object. 
*/
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