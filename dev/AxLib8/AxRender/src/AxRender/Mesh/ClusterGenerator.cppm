module;

#include "meshoptimizer.h"
#include "AxClusterLod.h"

export module AxRender:ClusterGenerator;
import :MeshObject;
import :EditableMesh;

export namespace ax {

class ClusterGenerator {
public:
	using Box = BBox3f;

	using Vertex = AxGpuMeshletVert;

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	union Triangle
	{
		Vertex v[3];
		char data[sizeof(Vertex) * 3];
	};
	
	static float boundsError(const clodBounds& bounds, const Vec3f& cameraPos, float camera_proj, float camera_znear)
	{
		Vec3f c(bounds.center[0], bounds.center[1], bounds.center[2]);
		auto delta = c - cameraPos;
		float d = delta.length() - bounds.radius;
		return bounds.error / (d > camera_znear ? d : camera_znear) * (camera_proj * 0.5f);
	}
	
	static int follow(int* parents, int index)
	{
		while (index != parents[index])
		{
			int parent = parents[index];
			parents[index] = parents[parent];
			index = parent;
		}

		return index;
	}
	
	// What follows is code that is helpful for collecting metrics, visualizing cuts, etc.
	// This code is not used in the actual clustering implementation and can be ignored.
	static int follow(std::vector<int>& parents, int index)
	{
		while (index != parents[index])
		{
			int parent = parents[index];
			parents[index] = parents[parent];
			index = parent;
		}

		return index;
	}	

	static int measureComponents(std::vector<int>& parents, const std::vector<unsigned int>& indices, const std::vector<unsigned int>& remap);
	static size_t measureBoundary(std::vector<int>& used, const std::vector<std::vector<unsigned int> >& clusters, const std::vector<unsigned int>& remap);

	static inline float surface(const Box& box)
	{
		auto s = box.size();
		return s.x * s.y 
			 + s.y * s.z
			 + s.z * s.x; 
	}

	static float sahCost(const Box* boxes, unsigned int* order, unsigned int* temp, size_t count);

	static float sahCost(const Box* boxes, size_t count)
	{
		if (count == 0)
			return 0.f;

		std::vector<unsigned int> order(count);
		for (size_t i = 0; i < count; ++i)
			order[i] = unsigned(i);

		std::vector<unsigned int> temp(count);
		return sahCost(boxes, &order[0], &temp[0], count);
	}

	static float sahOverhead(const std::vector<std::vector<unsigned int> >& clusters, Span<Vertex> vertices);

	void dumpWaveFrontObjFile(Span<Vertex> vertices, Span<u32> indices, bool recomputeNormals = false);
	void dumpWaveFrontObjFile(const char* section, const std::vector<unsigned int>& indices);

	void nanite(MeshObject& outMesh, Span<Vertex> vertices, Span<u32> indices);
};

} // namespace