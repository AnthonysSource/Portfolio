#include "RenderAPI.h"

#include "Core/Application.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Logging/Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <basetsd.h>

namespace Cookie::RenderingAPI {

	//-------------------------------------------------------------------------

	// returns the OpenGL representation of the
	// provided DataType
	int GetOpenGLDataType(DataType type) {
		switch (type) {
		case UINT:
			return GL_UNSIGNED_INT;
		case INT:
			return GL_INT;
		case FLOAT:
			return GL_FLOAT;
		default:
			return 0;
		}
	};

	// Returns the size of the provided data type
	int GetDataTypeSize(DataType type) {
		switch (type) {
		case UINT:
			return sizeof(u32);
		case INT:
			return sizeof(i32);
		case FLOAT:
			return sizeof(f32);
		default:
			return 0;
		}
	};

	//-------------------------------------------------------------------------

	struct VertexArray {
		u32 m_DeviceID;
	};

	//-------------------------------------------------------------------------

	LayoutAttribute::LayoutAttribute(u32 pos, DataType type, u32 count, bool normalized)
		: m_Pos(pos), m_Type(type), m_Count(count), m_Normalized(normalized), m_Size(GetDataTypeSize(type) * count), m_Offset(-1){};

	void Layout::AddAttribute(LayoutAttribute attr) {
		m_Attributes.push_back(attr);
		attr.m_Size = GetDataTypeSize(attr.m_Type) * attr.m_Count;
		m_Stride = 0;
		u32 offset = 0;
		for (u32 i = 0; i < m_Attributes.size(); i++) {
			m_Attributes[i].m_Offset = offset;
			offset += m_Attributes[i].m_Size;
			m_Stride += m_Attributes[i].m_Size;
		};
	};

	void Program::SetUniformMat4(const char *name, const float *data) {
		glUseProgram(m_DeviceID);
		glUniformMatrix4fv(glGetUniformLocation(m_DeviceID, name), 1, GL_FALSE, data);
	};

	//-------------------------------------------------------------------------

	namespace Device {

		VertexArray CreateVertexArray() {
			VertexArray va;
			glGenVertexArrays(1, &va.m_DeviceID);
			glBindVertexArray(va.m_DeviceID);
			return va;
		};

		VertexBuffer CreateVertexBuffer(char *data, u32 size) {
			VertexBuffer vb;
			vb.m_Size = size;

			glGenBuffers(1, &vb.m_DeviceID);
			glBindBuffer(GL_ARRAY_BUFFER, vb.m_DeviceID);
			// TODO: Add options to change gl_static_draw
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
			return vb;
		};

		VertexBuffer CreateDynamicVertexBuffer(u32 maxQuads) {
			VertexBuffer vb;
			vb.m_Size = maxQuads * sizeof(Quad);
			glGenBuffers(1, &vb.m_DeviceID);
			glBindBuffer(GL_ARRAY_BUFFER, vb.m_DeviceID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Quad) * maxQuads, nullptr, GL_DYNAMIC_DRAW);
			return vb;
		};

		void BufferDataIntoVertexBuffer(VertexBuffer *vb, const char *data, u32 size) {
			glBindBuffer(GL_ARRAY_BUFFER, vb->m_DeviceID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}

		IndexBuffer CreateIndexBuffer(char *data, u32 size, DataType type) {
			IndexBuffer ib;
			ib.m_Size = size;
			ib.m_DataType = type;
			ib.m_IndexCount = size / GetDataTypeSize(type);

			glGenBuffers(1, &ib.m_DeviceID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.m_DeviceID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
			return ib;
		};

		Texture CreateTexture(unsigned char *data, u32 width, u32 height) {
			Texture t;
			t.m_Width = width;
			t.m_Height = height;

			glGenTextures(1, &t.m_ID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, t.m_ID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			return t;
		};

		Program CreateProgram(const char *vertShaderPath, const char *fragShaderPath) {
			Program p;
			std::string vertexString = FileSystem::ReadTextFile(vertShaderPath);
			std::string fragString = FileSystem::ReadTextFile(fragShaderPath);
			const char *vertexSource = vertexString.c_str();
			const char *fragSource = fragString.c_str();

			u32 vertShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertShader, 1, &vertexSource, NULL);
			glCompileShader(vertShader);
			int success;
			char infoLog[512];
			glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
				CKE_LOG_INFO(Log::Channel::Rendering, "Vertex Shader Compilation Error -> {}", infoLog);
			}

			u32 fragShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragShader, 1, &fragSource, NULL);
			glCompileShader(fragShader);
			glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
				CKE_LOG_INFO(Log::Channel::Rendering, "Fragment Shader Compilation Error -> {}", infoLog);
			}

			u32 program = glCreateProgram();
			glAttachShader(program, vertShader);
			glAttachShader(program, fragShader);
			glLinkProgram(program);

			// Check if there are errors in program linking
			i32 isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE) {
				char logMsg[1024];
				glGetProgramInfoLog(program, 1024, NULL, logMsg);

				glDeleteProgram(program);

				CKE_LOG_ERROR(Log::Channel::Rendering, logMsg);
			}

			glDetachShader(program, vertShader);
			glDetachShader(program, fragShader);

			glDeleteShader(vertShader);
			glDeleteShader(fragShader);

			p.m_DeviceID = program;
			return p;
		};

		// void DeleteProgram();
		// void ClearColorBuffer(float r, float g, float b, float a);

	} // namespace Device

	//-------------------------------------------------------------------------

	namespace Context {

		struct ContextState {
			VertexArray m_VAO;
			VertexBuffer *m_CurrentVB;
			IndexBuffer *m_CurrentIB;
			Program *m_CurrentProgram;
		};

		ContextState g_Context;

		//-------------------------------------------------------------------------

		void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
										const void *userParam) {
			CKE_LOG_ERROR(Log::Channel::Rendering, "[OpenGL] %s", message);
		}

		// Lifetime
		//-------------------------------------------------------------------------

		void Init() {
			// Set current OpenGL context to the window
			glfwMakeContextCurrent((GLFWwindow *)g_AppData.m_Window.m_Handle);

			// Setup OpenGL function loading
			gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

			// Viewport settings and resize
			int w, h;
			glfwGetFramebufferSize((GLFWwindow *)g_AppData.m_Window.m_Handle, &w, &h);
			glViewport(0, 0, w, h);
			glfwSetFramebufferSizeCallback((GLFWwindow *)g_AppData.m_Window.m_Handle,
										   [](GLFWwindow *window, i32 width, i32 height) { glViewport(0, 0, width, height); });

			// Enable Debug Logging Callback
			// glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(MessageCallback, 0);

			glEnable(GL_DEPTH_TEST);

			// Generate Global VAO
			g_Context.m_VAO = Device::CreateVertexArray();
		};

		//-------------------------------------------------------------------------

		void BindVertexBuffer(VertexBuffer *vb) {
			glBindBuffer(GL_ARRAY_BUFFER, vb->m_DeviceID);
			g_Context.m_CurrentVB = vb;
		}

		void BindIndexBuffer(IndexBuffer *ib) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->m_DeviceID);
			g_Context.m_CurrentIB = ib;
		}

		void BindTexture(Texture *t) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, t->m_ID);
		}

		void BindProgram(Program *p) {
			g_Context.m_CurrentProgram = p;
			glUseProgram(p->m_DeviceID);
		}

		void BindLayout(Layout *l) {
			size_t count = l->m_Attributes.size();
			for (size_t i = 0; i < count; i++) {
				LayoutAttribute *attr = &l->m_Attributes[i];
				glVertexAttribPointer(attr->m_Pos, attr->m_Count, GetOpenGLDataType(attr->m_Type), attr->m_Normalized, l->m_Stride,
									  (void *)(UINT_PTR)attr->m_Offset);
				glEnableVertexAttribArray(attr->m_Pos);
			}
		}

		void Submit(u32 spritesCount) { glDrawElements(GL_TRIANGLES, spritesCount * 6, GL_UNSIGNED_INT, nullptr); }

		void Context::ClearBuffer(float r, float g, float b, float a) {
			glClearColor(r, g, b, a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void Context::SwapBuffers() { glfwSwapBuffers((GLFWwindow *)g_AppData.m_Window.m_Handle); }

	} // namespace Context

} // namespace Cookie::RenderingAPI