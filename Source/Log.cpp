//Log.cpp
#include "stdafx.h"
#include "Features.h"
#include <time.h>
#include "Log.h"
#include <cstdio>
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

std::ofstream m_File;
std::ofstream m_Console;
std::ifstream m_Input;

void Logger::Init() 
{
	filesystem::path m_FilePath;

	try
	{
		m_FilePath.append(DocumentsPath());
		if (!filesystem::exists(m_FilePath))
			filesystem::create_directory(m_FilePath);
		m_FilePath.append(MENU_NAME ".log");
	}
	catch (filesystem::filesystem_error const&)
	{

	}

	if (!AttachConsole(GetCurrentProcessId()))
		AllocConsole();
	SetConsoleTitleA("Anim");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	HANDLE hInput;
	DWORD prev_mode;
	hInput = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hInput, &prev_mode);
	SetConsoleMode(hInput, prev_mode & ENABLE_EXTENDED_FLAGS);

	m_Console.open("CONOUT$");
	m_Input.open("CONIN$");
	m_File.open(m_FilePath, std::ios_base::out | std::ios_base::app);

	//Print logo
	m_Console << u8R"kek(
██████╗ ██╗  ██╗ ██████╗ ███████╗███╗   ██╗██╗██╗  ██╗
██╔══██╗██║  ██║██╔═══██╗██╔════╝████╗  ██║██║╚██╗██╔╝
██████╔╝███████║██║   ██║█████╗  ██╔██╗ ██║██║ ╚███╔╝ 
██╔═══╝ ██╔══██║██║   ██║██╔══╝  ██║╚██╗██║██║ ██╔██╗ 
██║     ██║  ██║╚██████╔╝███████╗██║ ╚████║██║██╔╝ ██╗
╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝

)kek";

}


void Logger::Log(const char* type, const char* format, va_list args)
{
	auto time = std::time(nullptr);
	auto tm = std::localtime(&time);

	char prefix[64] = {};
	std::snprintf(prefix, sizeof(prefix) - 1, "[%02d:%02d:%02d] [%s] ", tm->tm_hour, tm->tm_min, tm->tm_sec, type);

	std::size_t messageLength = std::vsnprintf(nullptr, 0, format, args) + 1;
	auto messageBuffer = std::make_unique<char[]>(messageLength);

	std::uninitialized_fill_n(messageBuffer.get(), messageLength, '\0');
	std::vsnprintf(messageBuffer.get(), messageLength, format, args);

	m_Console << prefix << messageBuffer.get() << std::endl;
	m_File << prefix << messageBuffer.get() << std::endl;
}

void Logger::Custom(const char* prefix, const char* format, ...)
{
	va_list args{};
	va_start(args, format);
	Log(prefix, format, args);
	va_end(args);
}

void Logger::Info(const char* format, ...)
{
	va_list args{};
	va_start(args, format);
	Log("Info", format, args);
	va_end(args);
}

void Logger::Error(const char* format, ...)
{
	va_list args{};
	va_start(args, format);
	Log("Error", format, args);
	va_end(args);
}

void Logger::Fatal(const char* format, ...)
{
	va_list args{};
	va_start(args, format);
	Log("Fatal Error", format, args);
	va_end(args);

	Sleep(5000);

	exit(0);
}