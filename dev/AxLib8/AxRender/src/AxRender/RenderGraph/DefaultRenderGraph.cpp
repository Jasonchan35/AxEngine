module AxRender;

import :RenderCommandList;
import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {

DefaultRenderGraph::DefaultRenderGraph() {
	backBufferPass().color0.setClearColor(Color4f(0,0,0.2f));

	if constexpr (false) {
		auto shader = Shader::s_new(AX_NEW, "ImportedAssets/Shaders/core/test.axShader");
		_testMaterial = Material::s_new(AX_NEW);
		_testMaterial->setShader(shader);
	}

	if constexpr (true) {
//		_testTex0 = Texture2D::s_new(AX_ALLOC_REQ, "Assets/Images/test/uvCheckerPalette.png");
//		_testTex0 = Texture2D::s_new(AX_ALLOC_REQ, "Assets/Images/test/uvChecker_BC7.dds");
		_testTex0 = Texture2D::s_new(AX_NEW, "Assets/Images/test/uvChecker.jpg");
		_testTex1 = Texture2D::s_new(AX_NEW, "Assets/Images/test/radial_gradient.png");

		Sampler_CreateDesc samplerDesc;
		_testSampler = Sampler::s_new(AX_NEW, samplerDesc);

		SPtr<Sampler> testSampler2 = Sampler::s_new(AX_NEW, samplerDesc);

	}

	static NameId NameId_tex0 = NameId::s_make("tex0");
	static NameId NameId_tex1 = NameId::s_make("tex1");
	
	if constexpr (true) {
		auto shader = Shader::s_new(AX_NEW, "ImportedAssets/Shaders/core/testMesh.axShader");
		_testMeshMaterial = Material::s_new(AX_NEW);
		_testMeshMaterial->setShader(shader);
		_testMeshMaterial->setParam(NameId_tex0, _testTex0);
		_testMeshMaterial->setParam(NameId_tex1, _testTex1);
		_testMeshMaterial->setParam(NameId_tex0, _testSampler);
		_testMeshMaterial->setParam(NameId_tex1, _testSampler);
	}

	if constexpr (true) {
		auto shader = Shader::s_new(AX_NEW, "ImportedAssets/Shaders/core/testMesh3d.axShader");
		_testMesh3dMaterial = Material::s_new(AX_NEW);
		_testMesh3dMaterial->setShader(shader);
		_testMesh3dMaterial->setParam(NameId_tex0, _testTex0);
		_testMesh3dMaterial->setParam(NameId_tex1, _testTex1);
		_testMesh3dMaterial->setParam(NameId_tex0, _testSampler);
		_testMesh3dMaterial->setParam(NameId_tex1, _testSampler);
	}

	if constexpr (true) {
		using V = Vertex_PosUv;
		Array<V, 4>	vertices;
		{ auto& v = vertices.emplaceBack(); v.pos.set(-0.5f, -0.5f, 0); v.uv[0].set(0, 0); }
		{ auto& v = vertices.emplaceBack(); v.pos.set( 0.5f, -0.5f, 0); v.uv[0].set(1, 0); }
		{ auto& v = vertices.emplaceBack(); v.pos.set(-0.5f,  0.5f, 0); v.uv[0].set(0, 1); }
		{ auto& v = vertices.emplaceBack(); v.pos.set( 0.5f,  0.5f, 0); v.uv[0].set(1, 1); }

		u16 indices[] = {0, 1, 2, 2, 1, 3};
		_testMesh.create(vertices.constSpan(), Span(indices));
	}

	lighting.setInputs(gbuffer.color0, gbuffer.color1);

	_testCube = RenderStockObjects::s_instance()->meshes.cube;
}

void DefaultRenderGraph::onUpdate(RenderRequest* req) {
//	RenderTargetColorBufferDesc colorDesc{ColorType::RGBAf, RenderBufferLoadOp::Clear, Color4f(0, 0.5f, 0, 1)};
}

void DefaultRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
#if 0
	if (_testMaterial) {
		Cmd_DrawCall cmd;
		cmd.setMaterialPass(_testMaterial, 0);
		cmd.vertexCount = 3;
		req->draw(cmd);
	}
#endif

	// if (_testMeshMaterial) {
	// 	req->drawMesh(_testMesh, _testMeshMaterial, 0);
	// }
	
	_camera.setViewport(req->viewport());

	if (_testMesh3dMaterial) {
		auto* req_bk = rttiCastCheck<RenderRequest_Backend>(req);
		auto mvp = _camera.viewProjMatrix();
		req_bk->commonMaterialPass()->setParam(ShaderParamBindSpace::Object, AX_NAMEID("ax_object_mvp"), mvp);
		req->drawMesh(_testCube, _testMesh3dMaterial, 0);
	}

	req->drawUI();
}

void DefaultRenderGraph::onGBufferPass(RenderRequest* req, Span<Input> inputs) {
	// req.drawTexture(inputs[0]);
}

void DefaultRenderGraph::onLightingPass(RenderRequest* req, Span<Input> inputs) {
	// req.drawTexture(inputs[0]);
}

} // namespace

