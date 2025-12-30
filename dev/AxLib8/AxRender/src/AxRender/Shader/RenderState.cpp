module AxRender;

namespace ax /*::AxRender*/ {

RenderState::Blend::Blend() {
	rgb.set(  RenderBlendOp::Disable, RenderBlendFactor::One, RenderBlendFactor::OneMinusSrcAlpha);
	alpha.set(RenderBlendOp::Disable, RenderBlendFactor::One, RenderBlendFactor::OneMinusSrcAlpha);
	constColor = ColorRGBAf::kWhite();
}

} // namespace