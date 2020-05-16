#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <unordered_map>
#include <thread>

#define DEBUG

#ifdef DEBUG
#define LOG(X) {std::cerr<<(X);}
#endif

enum class Color{RED, GREEN, BLUE, YELLOW, WHITE, CYAN, MAGENTA, NOCHANGE};
enum class BackgroundColor { RED, GREEN, BLUE, YELLOW, WHITE, CYAN, MAGENTA, DEFAULT ,NOCHANGE };
enum class Direction{UP, DOWN, LEFT, RIGHT};
enum MIDDLE {};
enum class FunctionKey { UP , DOWN , LEFT , RIGHT , ENTER , SPACE , BACKSPACE , COUNT };
inline std::unordered_map<FunctionKey, int> KeyCode{ {FunctionKey::UP, VK_UP}, {FunctionKey::DOWN, VK_DOWN}, {FunctionKey::LEFT, VK_LEFT}
	,{FunctionKey::RIGHT, VK_RIGHT}, {FunctionKey::ENTER, VK_RETURN}, {FunctionKey::SPACE, VK_SPACE}, {FunctionKey::BACKSPACE, VK_BACK} };


struct ConsoleWindow
{
	COORD buffer_size;
	SMALL_RECT window;
	WORD attrib;
};

//static WORD default_attrib;

class Console: public std::ostream, public std::streambuf
{
public:
	static HANDLE hTerminal;
	static ConsoleWindow console_window;
	static void print_console_info();
	static void init_info();
public:
	Console() :std::ostream(std::cout.rdbuf()) { init_info(); };
	std::pair<short, short> get_window_size() { return { console_window.window.Right - console_window.window.Left+1, console_window.window.Bottom - console_window.window.Top+1 }; }
	static void set_foreground_color(Color color);
	static void set_text_intensify();
	static void set_background_color(BackgroundColor bgColor);
	static void change_window_color(BackgroundColor bgColor);
	static void restore();
	Console& operator<<(Color color);
	Console& operator<<(MIDDLE);
	Console& operator<<(BackgroundColor bgColor);
	static void backspace(short count=1);
	static void move_cursor(short count, Direction direction);
	static void move_cursor_to(COORD position);
	static COORD get_cursor_pos();
	static void erase_line();
	static void clear();
	static void close();

	template<typename T>
	Console& writeln(const T& arg, Color color=Color::NOCHANGE);

	Console& putchar(char c, size_t count = 1);

	static void hide_cursor();
	static void show_cursor();

	static void setTitle(std::string const& title);
	static FunctionKey getKey();
};



inline void Console::print_console_info()
{
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
		std::cout
		<< "Buffer Size: " << screen_buffer_info.dwSize.X << ", " << screen_buffer_info.dwSize.Y << '\n'
		<< "Cursor Pos: " << screen_buffer_info.dwCursorPosition.X << ", " << screen_buffer_info.dwCursorPosition.Y << '\n'
		<< "Window: (" << screen_buffer_info.srWindow.Top << ',' << screen_buffer_info.srWindow.Left << "), (" << screen_buffer_info.srWindow.Bottom << ',' << screen_buffer_info.srWindow.Right << ")\n"
		<< "Window Size: " << screen_buffer_info.dwMaximumWindowSize.X << ", " << screen_buffer_info.dwMaximumWindowSize.Y << '\n';
	else
		std::cerr << "Get console screen buffer info failed!\n";
}

inline void Console::init_info()
{
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
	{
		console_window.buffer_size = screen_buffer_info.dwSize;
		console_window.window = screen_buffer_info.srWindow;
		console_window.attrib = screen_buffer_info.wAttributes;
		//efault_attrib = screen_buffer_info.wAttributes;
	}
	else
		LOG("Get console screen buffer info failed!\n");
}

inline void Console::set_foreground_color(Color color)
{
	if (color == Color::NOCHANGE)
		return;
	static auto set_color = [&](auto color_attrib)
	{
		if (!SetConsoleTextAttribute(hTerminal, color_attrib))
			MessageBox(NULL, TEXT("SetConsoleTextAttribute"), TEXT("Error"), MB_OK);
		console_window.attrib = color_attrib;
	};
	switch (color)
	{
	case Color::RED:
		set_color(FOREGROUND_RED);
		break;
	case Color::GREEN:
		set_color(FOREGROUND_GREEN);
		break;
	case Color::BLUE:
		set_color(FOREGROUND_BLUE);
		break;
	case Color::YELLOW:
		set_color(FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case Color::CYAN:
		set_color(FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case Color::MAGENTA:
		set_color(FOREGROUND_RED | FOREGROUND_BLUE);
		break;
	case Color::WHITE:
		set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	default:
		break;
	}
}

inline void Console::set_text_intensify()
{
	SetConsoleTextAttribute(hTerminal, FOREGROUND_INTENSITY);
}

inline void Console::set_background_color(BackgroundColor bgColor)
{
	if (bgColor == BackgroundColor::NOCHANGE)
		return;
	static auto set_color = [&](auto color_attrib)
	{
		if (!SetConsoleTextAttribute(hTerminal, color_attrib))
			MessageBox(NULL, TEXT("SetConsoleTextAttribute"), TEXT("Error"), MB_OK);
	};
	auto text_color = console_window.attrib;
	switch (bgColor)
	{
	case BackgroundColor::RED:
		set_color(BACKGROUND_RED | text_color);
		break;
	case BackgroundColor::GREEN:
		set_color(BACKGROUND_GREEN | text_color);
		break;
	case BackgroundColor::BLUE:
		set_color(BACKGROUND_BLUE | text_color);
		break;
	case BackgroundColor::YELLOW:
		set_color(BACKGROUND_RED | BACKGROUND_GREEN | text_color);
		break;
	case BackgroundColor::CYAN:
		set_color(BACKGROUND_GREEN | BACKGROUND_BLUE | text_color);
		break;
	case BackgroundColor::MAGENTA:
		set_color(BACKGROUND_RED | BACKGROUND_BLUE | text_color);
		break;
	case BackgroundColor::WHITE:
		set_color(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
		break;
	case BackgroundColor::DEFAULT:
		set_color(0);
		set_foreground_color(Color::WHITE);
		break;
	default:
		break;
	}
}

inline void Console::change_window_color(BackgroundColor bgColor)
{
	if (bgColor == BackgroundColor::NOCHANGE)
		return;
	auto length = (console_window.window.Right - console_window.window.Left) * (console_window.window.Bottom - console_window.window.Top);
	DWORD actual_written = 0;
	switch (bgColor)
	{
	case BackgroundColor::RED:
		FillConsoleOutputAttribute(hTerminal, BACKGROUND_RED, length, { 0,0 }, &actual_written);
		break;
	case BackgroundColor::GREEN:
		FillConsoleOutputAttribute(hTerminal, BACKGROUND_GREEN, length, { 0,0 }, &actual_written);
		break;
	case BackgroundColor::BLUE:
		FillConsoleOutputAttribute(hTerminal, BACKGROUND_BLUE, length, { 0,0 }, &actual_written);
		break;
	case BackgroundColor::YELLOW:
		FillConsoleOutputAttribute(hTerminal, BACKGROUND_RED|BACKGROUND_GREEN, length, { 0,0 }, &actual_written);
		break;
	case BackgroundColor::WHITE:
		FillConsoleOutputAttribute(hTerminal, BACKGROUND_RED|BACKGROUND_BLUE|BACKGROUND_GREEN, length, { 0,0 }, &actual_written);
		break;
	case BackgroundColor::CYAN:
		FillConsoleOutputAttribute(hTerminal, BACKGROUND_BLUE|BACKGROUND_GREEN, length, { 0,0 }, &actual_written);
		break;
	case BackgroundColor::MAGENTA:
		FillConsoleOutputAttribute(hTerminal, BACKGROUND_RED|BACKGROUND_BLUE, length, { 0,0 }, &actual_written);
		break;
	case BackgroundColor::DEFAULT:
		FillConsoleOutputAttribute(hTerminal, 0, length, { 0,0 }, &actual_written);
		set_foreground_color(Color::WHITE);
		break;
	default:
		break;
	}
}

inline void Console::restore()
{
}

inline Console& Console::operator<<(Color color)
{
	set_foreground_color(color);
	return *this;
}

inline Console& Console::operator<<(MIDDLE)
{
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info);
	SetConsoleCursorPosition(hTerminal, { console_window.buffer_size.X / 2, screen_buffer_info.dwCursorPosition.Y });
	return *this;
}

inline Console& Console::operator<<(BackgroundColor bgColor)
{
	set_background_color(bgColor);
	return *this;
}

inline void Console::backspace(short count)
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

inline void Console::move_cursor(short count, Direction direction)
{
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	if(GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info)!=0)
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

inline void Console::move_cursor_to(COORD position)
{
	if (!SetConsoleCursorPosition(hTerminal, position))
		LOG("Move cursor to error!\n");
}

inline COORD Console::get_cursor_pos()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hTerminal, &info);
	return {info.dwCursorPosition.X, info.dwCursorPosition.Y};
}

inline void Console::erase_line()
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

inline void Console::clear()
{
	DWORD actual_written = 0;
	FillConsoleOutputCharacter(hTerminal, ' ', (console_window.window.Right - console_window.window.Left) * (console_window.window.Bottom - console_window.window.Top), { 0,0 }, &actual_written);
	SetConsoleCursorPosition(hTerminal, { 0,0 });
}

inline void Console::close()
{
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
}

inline Console& Console::putchar(char c, size_t count)
{
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	//DWORD count;	//If the pointer to the last parameter is set to nullptr, there will be an exception
	return *this;
}

inline void Console::hide_cursor()
{
	CONSOLE_CURSOR_INFO prev;
	GetConsoleCursorInfo(hTerminal, &prev);
	prev.bVisible = false;
	if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
		std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
}

inline void Console::show_cursor()
{
	CONSOLE_CURSOR_INFO prev;
	GetConsoleCursorInfo(hTerminal, &prev);
	prev.bVisible = true;
	if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
		std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
}

inline void Console::setTitle(std::string const& title)
{
	//if succeeds, the return value is NON 0
	SetConsoleTitleA(title.c_str());
}

inline FunctionKey Console::getKey()
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

template<typename T>
Console& Console::writeln(const T& arg, Color color)
{
	if (color == Color::NOCHANGE)
		std::cout << arg << '\n';
	else
	{
		if (CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info; GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
		{
			WORD old_color = screen_buffer_info.wAttributes;
			set_foreground_color(color);
			std::cout << arg << '\n';
			if (SetConsoleTextAttribute(hTerminal, old_color) == 0)
				std::cerr << "Set console text attribute error!\n";
		}
		else
			std::cerr << "Get console screen buffer info failed!\n";
	}
	return *this;
}
