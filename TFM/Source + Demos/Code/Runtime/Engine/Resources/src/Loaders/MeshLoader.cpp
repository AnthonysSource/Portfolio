#include "Loaders/MeshLoader.h"

#include "CookieKat/Core/Math/Math.h"
#include "CookieKat/Core/Memory/Memory.h"

#include "CookieKat/Engine/Resources/Resources/MeshResource.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <rapidjson/document.h>

namespace CKE {
	void MeshLoader::Initialize(RenderDevice* pRenderDevice) {
		m_pDevice = pRenderDevice;
	}

	LoadResult MeshLoader::Load(LoaderContext& ctx, Blob& binarySource) const {
		// Assimp load mesh
		//-----------------------------------------------------------------------------
		Assimp::Importer importer;

		aiScene const* aiScene = importer.ReadFileFromMemory(
			binarySource.data(), binarySource.size(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		auto const aiMesh = aiScene->mMeshes[0];

		// Convert into engine mesh
		//-----------------------------------------------------------------------------
		auto meshResource = New<MeshResource>();
		meshResource->m_Vertices.reserve(aiMesh->mNumVertices);

		for (u64 i = 0; i < aiMesh->mNumVertices; ++i) {
			aiVector3D const& pos = aiMesh->mVertices[i];
			aiVector3D const& normal = aiMesh->mNormals[i];
			aiVector3D const& tangent = aiMesh->mTangents[i];
			aiVector3D const& texCoord = aiMesh->mTextureCoords[0][i];

			Vertex_3P3N3T2Tc vert{
				Vec3(pos.x, pos.y, pos.z),
				Vec3(normal.x, normal.y, normal.z),
				Vec3(tangent.x, tangent.y, tangent.z),
				Vec2(texCoord.x, texCoord.y)
			};
			meshResource->m_Vertices.push_back(vert);
		}

		u32 const numIndices = aiMesh->mNumFaces * 3;
		meshResource->m_Indices.reserve(numIndices);

		for (u64 i = 0; i < aiMesh->mNumFaces; ++i) {
			for (u64 j = 0; j < aiMesh->mFaces[i].mNumIndices; ++j) {
				meshResource->m_Indices.emplace_back(aiMesh->mFaces[i].mIndices[j]);
			}
		}

		// Set texture dependencies
		//-----------------------------------------------------------------------------

		ctx.SetResource(meshResource);
		return LoadResult::Successful;
	}

	LoadResult MeshLoader::Install(LoaderContext& ctx, InstallDependencies& dependencies) {
		auto meshResource = ctx.GetResource<MeshResource>();

		BufferDesc vertexBufferDesc;
		vertexBufferDesc.m_Usage = BufferUsage::Vertex | BufferUsage::TransferDst;
		vertexBufferDesc.m_MemoryAccess = MemoryAccess::GPU;
		vertexBufferDesc.m_SizeInBytes = meshResource->m_Vertices.size() * sizeof(meshResource->
			m_Vertices[0]);
		vertexBufferDesc.m_StrideInBytes = sizeof(meshResource->m_Vertices[0]);
		meshResource->m_VertexBufferHandle = m_pDevice->CreateBuffer_DEPR(vertexBufferDesc,
			meshResource->m_Vertices.data(),
			vertexBufferDesc.m_SizeInBytes);

		BufferDesc indexBufferDesc;
		indexBufferDesc.m_Usage = BufferUsage::Index | BufferUsage::TransferDst;
		indexBufferDesc.m_MemoryAccess = MemoryAccess::GPU;
		indexBufferDesc.m_SizeInBytes = meshResource->m_Indices.size() * sizeof(u32);
		indexBufferDesc.m_StrideInBytes = sizeof(u32);
		meshResource->m_IndexBufferHandle = m_pDevice->CreateBuffer_DEPR(indexBufferDesc,
			meshResource->m_Indices.data(),
			indexBufferDesc.m_SizeInBytes);

		return LoadResult::Successful;
	}
}
