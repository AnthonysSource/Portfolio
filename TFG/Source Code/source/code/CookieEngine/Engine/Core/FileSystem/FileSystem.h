#pragma once

#include "Core/Types/String.h"

#include <iostream>
#include <fstream>

namespace Cookie {
	namespace FileSystem {

		using InputFileStream = std::ifstream;
		using OutputFileStream = std::ofstream;
		using IOFileStream = std::fstream;

		// bool IsFile(const char *path);
		// bool IsDirectory(const char *path);

		// inline bool IsFile(const String *path) { return IsFile(path->c_str()); };
		// inline bool IsDirectory(const String *path) { return IsDirectory(path->c_str()); };

		void WriteToFile(const char *path, const char *data, const size_t size);
		void ReadFile(const char *path, char *outputBuffer, const size_t size);

		String ReadTextFile(const char *path);

	} // namespace FileSystem
} // namespace Cookie