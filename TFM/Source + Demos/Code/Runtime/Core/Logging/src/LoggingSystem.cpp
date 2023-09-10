#include "LoggingSystem.h"

#include <cstdarg>
#include <format>

namespace CKE {

	void LoggingSystem::Initialize() {
		m_LogEntries.reserve(1000);
	}

	void LoggingSystem::Shutdown() {
		m_LogEntries.clear();
	}

	void LoggingSystem::AddLogEntryVA(LogLevel level, LogChannel channel, char const* format, va_list vaList) {
		constexpr u32 MAX_MSG_SIZE = 2048;
		static char   s_LogBuffer[MAX_MSG_SIZE];
		FormatVA(s_LogBuffer, MAX_MSG_SIZE, format, vaList);

		printf("%s", s_LogBuffer);

		LogEntry entry{};
		entry.m_Channel = channel;
		entry.m_Level = level;
		entry.m_Message = String{s_LogBuffer};
		m_LogEntries.emplace_back(entry);
	}

	void LoggingSystem::Format(char* pBuffer, u32 bufferSize, char const* format, ...) {
		va_list vaList;
		va_start(vaList, format);
		FormatVA(pBuffer, bufferSize, format, vaList);
		va_end(vaList);
	}

	void LoggingSystem::FormatVA(char* pBuffer, u32 bufferSize, char const* format, va_list vaList) {
		vsnprintf(pBuffer, bufferSize, format, vaList);
	}

	void LoggingSystem::AddLogEntry(LogLevel level, LogChannel channel, char const* format, ...) {
		va_list vaList;
		va_start(vaList, format);
		AddLogEntryVA(level, channel, format, vaList);
		va_end(vaList);
	}
}
