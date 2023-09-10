#include "Log.h"

#include "Core/FileSystem/FileSystem.h"

#include <stdio.h>
#include <cstdarg>
#include <iostream>

namespace Cookie::Log {

	//-------------------------------------------------------------------------

	namespace {

#define DEFINE_CHARS(def) #def,
		static char const *const g_VerbosityLabels[] = {CKE_LOG_VERBOSITY_DEFINITIONS(DEFINE_CHARS)};
		static char const *const g_ChannelLabels[] = {CKE_LOG_CHANNEL_DEFINITIONS(DEFINE_CHARS)};
#undef DEFINE_CHARS

		struct LogData {
			TVector<LogEntry> m_LogEntries;
		};

		struct LogConfig {
			Verbosity m_MinVerbosity = Verbosity::Message;

			// Include in the log message the code file+line where the log is called
			bool m_LogOriginCodeFile = false;
		};

		LogData g_LogData;
		LogConfig g_Config;

		FileSystem::OutputFileStream g_GlobalLogFile;

	} // namespace

	//-------------------------------------------------------------------------

	void Initialize() {
		// Open/Create the global logs file
		g_GlobalLogFile.open("EngineLog.txt");
	}

	void Shutdown() { g_GlobalLogFile.close(); }

	//-------------------------------------------------------------------------

	i32 VFormat(char *buffer, u32 bufferSize, char const *format, va_list argsList) {
		return vsnprintf(buffer, bufferSize, format, argsList);
	}

	i32 Format(char *buffer, u32 bufferSize, char const *format, ...) {
		va_list argsList;
		va_start(argsList, format);
		i32 charsCount = VFormat(buffer, bufferSize, format, argsList);
		va_end(argsList);

		return charsCount;
	}

	//-------------------------------------------------------------------------

	TVector<LogEntry> const *GetLogEntries() { return &g_LogData.m_LogEntries; }

	void AddEntry(Verbosity verbosity, Channel channel, char const *fileName, u32 lineNumber, char const *format, ...) {

		if (verbosity >= g_Config.m_MinVerbosity) {
			va_list argsList;
			va_start(argsList, format);

			VAddEntry(verbosity, channel, fileName, lineNumber, format, argsList);

			va_end(argsList);
		}
	}

	void VAddEntry(Verbosity verbosity, Channel channel, char const *fileName, u32 lineNumber, char const *format, va_list argsList) {
		// Setup static buffer for the message
		const u32 MAX_CHARS = 1024;
		static char s_LogBuffer[MAX_CHARS];

		// Format the base message with the arguments provided
		VFormat(s_LogBuffer, MAX_CHARS, format, argsList);

		// Copy of the formatted message to reuse
		// the buffer
		String msg = s_LogBuffer;

		// Set the final log format to create the final message
		if (g_Config.m_LogOriginCodeFile) {
			Format(s_LogBuffer, MAX_CHARS, "[%s][%s : %u][%s] %s", g_ChannelLabels[(u32)channel], fileName, lineNumber,
				   g_VerbosityLabels[(u32)verbosity], msg.c_str());
		} else {
			Format(s_LogBuffer, MAX_CHARS, "[%s][%s] %s", g_ChannelLabels[(u32)channel], g_VerbosityLabels[(u32)verbosity], msg.c_str());
		}

		// Print it to the console using the standard output
		printf("%s\n", s_LogBuffer);

		// Create and save an in-memory log entry of the message
		LogEntry entry;
		entry.m_Verbosity = verbosity;
		entry.m_Channels = channel;
		entry.m_FileName = fileName;
		entry.m_LineNumber = lineNumber;
		entry.m_Message = s_LogBuffer;

		g_LogData.m_LogEntries.push_back(entry);

		// Save the log to the global log file
		g_GlobalLogFile << s_LogBuffer << std::endl;
	}

	void BasicEntry(char const *format, ...) {
		va_list argsList;
		va_start(argsList, format);
		VBasicEntry(format, argsList);
		va_end(argsList);
	}

	void VBasicEntry(char const *format, va_list argsList) {
		const u32 MAX_CHARS = 1024;
		static char s_LogBuffer[MAX_CHARS];

		vsnprintf(s_LogBuffer, MAX_CHARS, format, argsList);

		printf("%s\n", s_LogBuffer);

		// Log to file
		g_GlobalLogFile << s_LogBuffer << std::endl;
	}

	//-------------------------------------------------------------------------

} // namespace Cookie::Log