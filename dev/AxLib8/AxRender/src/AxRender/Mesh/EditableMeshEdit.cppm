module;

export module AxRender:EditableMeshEdit;
export import :EditableMesh;

export namespace ax {

class EditableMeshEdit {
public:
	using Mesh = EditableMesh;
	EditableMeshEdit(Mesh& mesh) : _mesh(mesh) {}

	void createSphere	(double radius, Int rows, Int columns);
	void createCylinder	(double height, double radius, Int rows, Int columns, bool topCap, bool bottomCap);
	void createCone		(double height, double radius, Int rows, Int columns, bool bottomCap);

private:
	Mesh& _mesh;
};

} // namespace
