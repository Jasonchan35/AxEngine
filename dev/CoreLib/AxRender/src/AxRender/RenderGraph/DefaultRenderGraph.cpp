module AxRender.DefaultRenderGraph;

import AxRender.CommandBuffer;
import AxRender.RenderRequest;

namespace ax::AxRender {

DefaultRenderGraph::DefaultRenderGraph() {
	backBufferPass().color0.setClearColor(Color4f::kDarkBlue());

	if (0) {
		auto shader = Shader::s_new(AX_ALLOC_REQ, "ImportedAssets/Shaders/core/test.axShader");
		_testMaterial = Material::s_new(AX_ALLOC_REQ);
		_testMaterial->setShader(shader);
	}

	if (1) {
//		_testTex0  = Texture2D::s_newFromFile(AX_ALLOC_REQ, "images/test/uvCheckerPalette.png");
		// _testTex0 = Texture2D::s_newFromFile(AX_ALLOC_REQ, "images/test/uvChecker_BC7.dds");
		_testTex0 = Texture2D::s_new(AX_ALLOC_REQ, "Assets/Images/test/uvChecker.jpg");
		_testTex1 = Texture2D::s_new(AX_ALLOC_REQ, "Assets/Images/test/radial_gradient.png");

		Sampler_CreateDesc samplerDesc;
		_testSampler = Sampler::s_new(AX_ALLOC_REQ, samplerDesc);

		SPtr<Sampler> testSampler2 = Sampler::s_new(AX_ALLOC_REQ, samplerDesc);

	}

	if (1) {
		auto shader = Shader::s_new(AX_ALLOC_REQ, "ImportedAssets/Shaders/core/testMesh.axShader");
		_testMeshMaterial = Material::s_new(AX_ALLOC_REQ);
		_testMeshMaterial->setShader(shader);
		
//		_testMeshMaterial->setParam(NameId("color"), Color4f::kRed());

		static NameId tex0("tex0");
		static NameId tex1("tex1");

		_testMeshMaterial->setParam(tex0, _testTex0);
		_testMeshMaterial->setParam(tex1, _testTex1);

		_testMeshMaterial->setParam(tex0, _testSampler);
		_testMeshMaterial->setParam(tex1, _testSampler);
	}


	if (1) {
		using V = Vertex_PosUv;
		Array<V, 4>	vertices;
		{ auto& v = vertices.emplaceBack(); v.pos.set(-0.5f, -0.5f, 0); v.uv[0].set(0, 0); }
		{ auto& v = vertices.emplaceBack(); v.pos.set( 0.5f, -0.5f, 0); v.uv[0].set(1, 0); }
		{ auto& v = vertices.emplaceBack(); v.pos.set(-0.5f,  0.5f, 0); v.uv[0].set(0, 1); }
		{ auto& v = vertices.emplaceBack(); v.pos.set( 0.5f,  0.5f, 0); v.uv[0].set(1, 1); }

		u16 indices[] = {0, 1, 2, 2, 1, 3};
		_testMesh.create(vertices.constSpan(), Span_ref(indices));
	}

	lighting.setInputs(gbuffer.color0, gbuffer.color1);
}

void DefaultRenderGraph::onUpdate(RenderRequest* req) {
	ColorBufferDesc colorDesc{ColorType::RGBAf, BufferLoadOp::Clear, Color4f(0, 0.5f, 0, 1)};
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

	if (_testMeshMaterial) {
		req->drawMesh(_testMesh, _testMeshMaterial, 0);
	}

	{
		UI::Window	win("Testing");

		float f = 10;
		UI::DragFloat("float", &f);
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

