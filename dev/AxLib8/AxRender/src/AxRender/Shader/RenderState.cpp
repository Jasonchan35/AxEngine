module AxRender;

namespace ax /*::AxRender*/ {

RenderState::Blend::Blend() {
	rgb.set(  RenderState_BlendOp::Disable, RenderState_BlendFactor::One, RenderState_BlendFactor::OneMinusSrcAlpha);
	alpha.set(RenderState_BlendOp::Disable, RenderState_BlendFactor::One, RenderState_BlendFactor::OneMinusSrcAlpha);
	constColor = ColorRGBAf::kWhite();
}

} // namespace