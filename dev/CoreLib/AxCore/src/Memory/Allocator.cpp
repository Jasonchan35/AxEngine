module AxCore.Allocator;

namespace ax {

Allocator* ax_default_allocator() {
	static Allocator s;
	return &s;
}

} // namespace
