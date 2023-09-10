#pragma once

#include "Core/Types/Containers.h"
#include "Core/Types/PrimitiveTypes.h"

#include "Core/Math/Math.h"

namespace Cookie::RenderingAPI {

	//-------------------------------------------------------------------------

	enum DataType { UINT, INT, FLOAT };

	struct VertexBuffer {
		u32 m_DeviceID;
		u32 m_Size;
		// Source data ref
	};

	struct IndexBuffer {
		u32 m_DeviceID;
		u32 m_Size;
		DataType m_DataType;
		u32 m_IndexCount; // Amount of indices in the buffer
	};

	struct LayoutAttribute {
		// These parameters are provided
		// on creation
		u32 m_Pos;
		DataType m_Type;
		u32 m_Count;
		bool m_Normalized;

		// These are derivated from the
		// previous ones
		u32 m_Size;
		u32 m_Offset;

		LayoutAttribute(u32 pos, DataType type, u32 count, bool normalized);
	};

	struct Layout {
		TVector<LayoutAttribute> m_Attributes;
		u32 m_Stride;

		void AddAttribute(LayoutAttribute attr);
	};

	//-------------------------------------------------------------------------

	struct Program {
		u32 m_DeviceID;
		// Frag Shader
		// Vert Shader
		void SetUniformMat4(const char *name, const float *data);
	};

	struct FragmentShader {
		u32 m_DeviceID;
		// Source Frag Shader
	};

	struct VertexShader {
		u32 m_DeviceID;
		// Source Vert Shader
	};

	//-------------------------------------------------------------------------

	struct Texture {
		u32 m_ID;
		u32 m_Width;
		u32 m_Height;
	};

	//-------------------------------------------------------------------------

	namespace Device {
		VertexBuffer CreateVertexBuffer(char *data, u32 size);
		VertexBuffer CreateDynamicVertexBuffer(u32 size);
		void BufferDataIntoVertexBuffer(VertexBuffer *vb, const char *data, u32 size);
		IndexBuffer CreateIndexBuffer(char *data, u32 size, DataType type);
		Texture CreateTexture(unsigned char *data, u32 width, u32 height);
		Program CreateProgram(const char *vertShaderPath, const char *fragShaderPath);

	} // namespace Device

	//-------------------------------------------------------------------------

	namespace Context {
		void Init();

		void BindVertexBuffer(VertexBuffer *vb);
		void BindIndexBuffer(IndexBuffer *ib);
		void BindProgram(Program *p);
		void BindTexture(Texture *t);
		void BindLayout(Layout *l);

		void ClearBuffer(float r, float g, float b, float a);
		void SwapBuffers();
		void Submit(u32 spritesCount);
	} // namespace Context

	//-------------------------------------------------------------------------

	struct Vertex {
		Float3 m_Pos;
		Float2 m_TexCoord;
		// Float3 m_Color;

		Vertex() : m_Pos(Float3(0.0f)), m_TexCoord(Float3(0.0f)) {}
		Vertex(Float3 pos, Float2 texCoord) : m_Pos(pos), m_TexCoord(texCoord) {}
	};

	struct Quad {
		Vertex m_Vertices[4];
	};

} // namespace Cookie::RenderingAPI