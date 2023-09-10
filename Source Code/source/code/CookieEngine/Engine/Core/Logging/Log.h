#pragma once

#include "Core/Types/PrimitiveTypes.h"
#include "Core/Types/Pointers.h"
#include "Core/Types/String.h"

#include "Core/Types/Containers.h"

namespace Cookie::Log {

	//-------------------------------------------------------------------------

#define CKE_LOG_VERBOSITY_DEFINITIONS(F)                                                                                                   \
	F(Comment)                                                                                                                             \
	F(Message)                                                                                                                             \
	F(Warning)                                                                                                                             \
	F(Error)                                                                                                                               \
	F(CriticalError)

#define CKE_LOG_CHANNEL_DEFINITIONS(F)                                                                                                     \
	F(Core)                                                                                                                                \
	F(Rendering)                                                                                                                           \
	F(Input)                                                                                                                               \
	F(FileSystem)                                                                                                                          \
	F(Entities)                                                                                                                            \
	F(Resources)                                                                                                                           \
	F(Game)

#define DEFINE_ENUM(def) def,
	enum class Verbosity : u8 { CKE_LOG_VERBOSITY_DEFINITIONS(DEFINE_ENUM) };
	enum class Channel : u8 { CKE_LOG_CHANNEL_DEFINITIONS(DEFINE_ENUM) };
#undef DEFINE_ENUM

	//-------------------------------------------------------------------------

	struct LogEntry {
		String m_TimeStamp;
		String m_Message;
		Channel m_Channels;

		String m_FileName;
		String m_FunctionName;
		u32 m_LineNumber;

		Verbosity m_Verbosity;
	};

	//-------------------------------------------------------------------------

	void Initialize();
	void Shutdown();

	//-------------------------------------------------------------------------

	TVector<LogEntry> const *GetLogEntries();

	void AddEntry(Verbosity verbosity, Channel channel, char const *fileName, u32 lineNumber, char const *format, ...);
	void VAddEntry(Verbosity verbosity, Channel channel, char const *fileName, u32 lineNumber, char const *format, va_list argsList);

	void BasicEntry(char const *format, ...);
	void VBasicEntry(char const *format, va_list argsList);

} // namespace Cookie::Log

// Macros
//-------------------------------------------------------------------------

// clang-format off
// Returns only the file name and not the complete path
#define CKE_LOG_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#ifdef COOKIE_LOGGING
	#define CKE_LOG_COMMENT(CHANNEL, ...)	Cookie::Log::AddEntry(Cookie::Log::Verbosity::Comment, CHANNEL, CKE_LOG_FILENAME, __LINE__, __VA_ARGS__)
	#define CKE_LOG_INFO(CHANNEL, ...)		Cookie::Log::AddEntry(Cookie::Log::Verbosity::Message, CHANNEL, CKE_LOG_FILENAME, __LINE__, __VA_ARGS__)
	#define CKE_LOG_WARN(CHANNEL, ...)	Cookie::Log::AddEntry(Cookie::Log::Verbosity::Warning, CHANNEL, CKE_LOG_FILENAME, __LINE__, __VA_ARGS__)
	#define CKE_LOG_ERROR(CHANNEL, ...)		Cookie::Log::AddEntry(Cookie::Log::Verbosity::Error, CHANNEL, CKE_LOG_FILENAME, __LINE__, __VA_ARGS__)
	#define CKE_LOG_CRITICAL(CHANNEL, ...)	Cookie::Log::AddEntry(Cookie::Log::Verbosity::CriticalError, CHANNEL, CKE_LOG_FILENAME, __LINE__, __VA_ARGS__)
#else
	#define CKE_LOG_COMMENT(CHANNEL, ...)
	#define CKE_LOG_INFO(CHANNEL, ...)
	#define CKE_LOG_WARNING(CHANNEL, ...)
	#define CKE_LOG_ERROR(CHANNEL, ...)
	#define CKE_LOG_CRITICAL(CHANNEL, ...)
#endif

// clang-format on

//-------------------------------------------------------------------------