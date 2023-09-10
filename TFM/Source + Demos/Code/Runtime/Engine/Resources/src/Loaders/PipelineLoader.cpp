#include "Loaders/PipelineLoader.h"

#include "CookieKat/Core/FileSystem/FileSystem.h"
#include "CookieKat/Core/Memory/Memory.h"
#include "CookieKat/Engine/Resources/Resources/PipelineResource.h"
#include "CookieKat/Systems/RenderUtils/ShaderReflection.h"

namespace CKE {
	void PipelineLoader::Initialize(RenderDevice* pRenderDevice) {
		m_pRenderDevice = pRenderDevice;
	}

	LoadResult PipelineLoader::LoadCompiledResource(LoaderContext& ctx, BinaryInputArchive& ar) const {
		auto pPipeline = New<PipelineResource>();
		ar << *pPipeline;
		ctx.SetResource(pPipeline);
		return LoadResult::Successful;
	}

	LoadResult PipelineLoader::Install(LoaderContext& ctx, InstallDependencies& dependencies) {
		PipelineResource* pPipeline = ctx.GetResource<PipelineResource>();

		PipelineLayoutDesc layoutDesc = ShaderReflectionUtils::ReflectLayout(
			pPipeline->GetVertSource(),
			pPipeline->GetFragSource()
		);
		pPipeline->m_PipelineLayout = m_pRenderDevice->CreatePipelineLayout(layoutDesc);
		pPipeline->m_PipelineLayoutDesc = layoutDesc;
		pPipeline->m_VertexInputLayoutDesc = ShaderReflectionUtils::ReflectVertexInput(pPipeline->GetVertSource());

		return LoadResult::Successful;
	}

	LoadResult PipelineLoader::Uninstall(LoaderContext& ctx) {
		PipelineResource* pPipeline = ctx.GetResource<PipelineResource>();
		m_pRenderDevice->DestroyPipelineLayout(pPipeline->m_PipelineLayout);
		return LoadResult::Successful;
	}
}
