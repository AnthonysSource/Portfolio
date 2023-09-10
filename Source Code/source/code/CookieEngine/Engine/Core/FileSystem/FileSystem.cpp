#include "FileSystem.h"

#include "Core/Types/PrimitiveTypes.h"
#include "Core/Logging/Log.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace Cookie {
	namespace FileSystem {

		// write to a binary file
		void FileSystem::WriteToFile(const char *path, const char *data, const size_t size) {
			std::ofstream stream(path, std::ios::out | std::ios::binary);
			stream.write(data, size);
			stream.close();
		};

		// Read a binary file and store its content in the output buffer
		void FileSystem::ReadFile(const char *path, char *outputBuffer, const size_t size) {
			std::ifstream stream(path, std::ios::in | std::ios::binary);
			stream.read(outputBuffer, size);
			stream.close();
		}

		String FileSystem::ReadTextFile(const char *path) {
			String fileText;
			std::ifstream fileStream;
			fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				fileStream.open(path);
				std::stringstream textStream;
				textStream << fileStream.rdbuf();
				fileStream.close();
				fileText = textStream.str();
			} catch (const std::ifstream::failure e) {
				CKE_LOG_ERROR(Log::Channel::FileSystem, "Could not load text file at [%s]", path);
			}
			return fileText;
		}

		// void FileSystem::Write(char *data) { wf.write((char *)&data[i], sizeof(Dt));
		// }

	} // namespace FileSystem
} // namespace Cookie