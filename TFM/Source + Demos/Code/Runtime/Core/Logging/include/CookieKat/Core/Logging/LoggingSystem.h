#pragma once

#include "CookieKat/Core/Containers/Containers.h"
#include "CookieKat/Core/Containers/String.h"
#include <format>
#include <iostream>
#include <chrono>
#include <Windows.h>

namespace CKE {
	//-----------------------------------------------------------------------------

#define CKE_LOGGING_DEF_LEVELS(DEF) \
	DEF(Info) \
	DEF(Warning) \
	DEF(Error) \
	DEF(Debug)

	// All of the available log channels
	// Can define more if needed
#define CKE_LOGGING_DEF_CHANNEL(DEF) \
	DEF(Assets) \
	DEF(Resources) \
	DEF(Rendering) \
	DEF(ECS) \
	DEF(Core) \
	DEF(Game)
#define CKE_DEFINE_LOG_ENUM(d) d,

	enum class LogLevel : u8 { CKE_LOGGING_DEF_LEVELS(CKE_DEFINE_LOG_ENUM) };

	enum class LogChannel : u8 { CKE_LOGGING_DEF_CHANNEL(CKE_DEFINE_LOG_ENUM) };

#undef CKE_DEFINE_LOG_ENUM

	//-----------------------------------------------------------------------------

	struct LogEntry
	{
		LogLevel   m_Level;
		LogChannel m_Channel;
		String     m_Message;
	};

	// General purpose logging system of the engine
	class LoggingSystem
	{
	public:
		void Initialize();
		void Shutdown();

		// Templated API
		//-----------------------------------------------------------------------------

		// Logs a message directly without appending any extra information and without any tracking
		template <typename... Args>
		void Simple(char const* format, Args&&... args);

		// Logs a message using the standard format alongside the provided one
		// Utilizes std::vformat to format the supplied message
		// Example:
		//   Log(LogLevel::Info, LogChannel::Assets, "Number {}", 42);
		//   // Outputs: 24:60:60.000 | Info | Assets | Number 42 
		template <typename... Args>
		void Log(LogLevel level, LogChannel channel, char const* format, Args&&... args);

		// C Based API
		//-----------------------------------------------------------------------------

		void AddLogEntry(LogLevel level, LogChannel channel, char const* format, ...);
		void AddLogEntryVA(LogLevel level, LogChannel channel, char const* format, va_list vaList);

		// Format a message using vsnprintf
		void Format(char* pBuffer, u32 bufferSize, char const* format, ...);
		void FormatVA(char* pBuffer, u32 bufferSize, char const* format, va_list vaList);

	private:
		Vector<LogEntry> m_LogEntries{}; // All log messages in the current execution

#define CKE_LOG_DEFINE_STR(x) #x,
		constexpr static char const* const s_LogLevelLabels[] = {
			CKE_LOGGING_DEF_LEVELS(CKE_LOG_DEFINE_STR)
		};
		constexpr static char const* const s_LogChannelsLabels[] = {
			CKE_LOGGING_DEF_CHANNEL(CKE_LOG_DEFINE_STR)
		};
#undef CKE_LOG_DEFINE_STR
	};

	// Global logging system of the engine
	inline LoggingSystem g_LoggingSystem{};
}

// Template Implementation
//-----------------------------------------------------------------------------

namespace CKE {
	template <typename... Args>
	void LoggingSystem::Simple(char const* format, Args&&... args) {
		std::cout << std::vformat(format, std::make_format_args(args...));
	}

	template <typename... Args>
	void LoggingSystem::Log(LogLevel  level, LogChannel channel, char const* format,
	                        Args&&... args) {
		LogEntry entry{};
		entry.m_Channel = channel;
		entry.m_Level = level;
		entry.m_Message = std::vformat(format, std::make_format_args(args...));
		m_LogEntries.emplace_back(entry);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		// Get data about current time
		auto timePoint = std::chrono::floor<std::chrono::milliseconds>(
			std::chrono::system_clock::now());
		SetConsoleTextAttribute(hConsole, 8);
		std::cout << timePoint;
		SetConsoleTextAttribute(hConsole, 7);
		std::cout << " | ";
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << s_LogLevelLabels[static_cast<i32>(level)];
		SetConsoleTextAttribute(hConsole, 7);
		std::cout << " | ";
		SetConsoleTextAttribute(hConsole, 3);
		std::cout << s_LogChannelsLabels[static_cast<i32>(channel)];
		SetConsoleTextAttribute(hConsole, 7);
		std::cout << " | ";
		std::cout << entry.m_Message;
	}
}
