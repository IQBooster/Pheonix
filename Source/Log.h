#pragma once

class Logger
{
public:
	static void Init();
	static void Log(const char* type, const char* format, va_list args);
	static void Custom(const char* prefix, const char* format, ...);
	static void Info(const char* format, ...);
	static void Error(const char* format, ...);
	static void Fatal(const char* format, ...);
};