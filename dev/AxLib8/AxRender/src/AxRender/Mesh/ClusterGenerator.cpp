module;

#include "meshoptimizer.h"

#define CLUSTERLOD_IMPLEMENTATION
#include "AxClusterLod.h"

module AxRender;
import :ClusterGenerator;
import :RenderMath;

namespace ax {

int ClusterGenerator::measureComponents(
	std::vector<int>& parents,
	const std::vector<unsigned int>& indices,
	const std::vector<unsigned int>& remap
) {
	assert(parents.size() == remap.size());

	for (size_t i = 0; i < indices.size(); ++i)
	{
		unsigned int v = remap[indices[i]];
		parents[v]     = v;
	}

	for (size_t i = 0; i < indices.size(); ++i)
	{
		int v0 = remap[indices[i]];
		int v1 = remap[indices[i + (i % 3 == 2 ? -2 : 1)]];

		v0 = follow(parents, v0);
		v1 = follow(parents, v1);

		parents[v0] = v1;
	}

	for (size_t i = 0; i < indices.size(); ++i)
	{
		unsigned int v = remap[indices[i]];
		parents[v]     = follow(parents, v);
	}

	int roots = 0;
	for (size_t i = 0; i < indices.size(); ++i)
	{
		unsigned int v = remap[indices[i]];
		roots          += parents[v] == int(v);
		parents[v]     = -1; // make sure we only count each root once
	}

	return roots;
}

size_t ClusterGenerator::measureBoundary(std::vector<int>& used,
	const std::vector<std::vector<unsigned int>>& clusters,
	const std::vector<unsigned int>& remap) {
	for (size_t i = 0; i < used.size(); ++i)
		used[i] = -1;

	// mark vertices that are used by multiple groups with -2
	int group = 0;

	for (size_t i = 0; i < clusters.size(); ++i)
	{
		group += clusters[i].empty();

		for (size_t j = 0; j < clusters[i].size(); ++j)
		{
			unsigned int v = remap[clusters[i][j]];

			used[v] = (used[v] == -1 || used[v] == group) ? group : -2;
		}
	}

	size_t result = 0;

	for (size_t i = 0; i < clusters.size(); ++i)
	{
		// count vertices that are used by multiple groups and change marks to -1
		for (size_t j = 0; j < clusters[i].size(); ++j)
		{
			unsigned int v = remap[clusters[i][j]];

			result  += (used[v] == -2);
			used[v] = (used[v] == -2) ? -1 : used[v];
		}

		// change marks back from -1 to -2 for the next pass
		for (size_t j = 0; j < clusters[i].size(); ++j)
		{
			unsigned int v = remap[clusters[i][j]];

			used[v] = (used[v] == -1) ? -2 : used[v];
		}
	}

	return int(result);
}

float ClusterGenerator::sahCost(const Box* boxes, unsigned int* order, unsigned int* temp, size_t count) {
	Box total = boxes[order[0]];
	for (size_t i = 1; i < count; ++i)
		total.merge(boxes[order[i]]);

	int   best_axis = -1;
	int   best_bin  = -1;
	float best_cost = FLT_MAX;

	const int kBins = 15;

	for (int axis = 0; axis < 3; ++axis)
	{
		Box          bins[kBins];
		unsigned int counts[kBins] = {};

		float extent = total.max.e[axis] - total.min.e[axis];
		if (extent <= 0.f)
			continue;

		for (int i = 0; i < kBins; ++i)
			bins[i] = Box::s_empty();

		for (size_t i = 0; i < count; ++i)
		{
			unsigned int index = order[i];
			float        p     = (boxes[index].min.e[axis] + boxes[index].max.e[axis]) * 0.5f;
			int          bin   = int((p - total.min.e[axis]) / extent * (kBins - 1) + 0.5f);
			assert(bin >= 0 && bin < kBins);

			bins[bin].merge(boxes[index]);
			counts[bin]++;
		}

		Box    laccum       = Box::s_empty();
		Box    raccum       = Box::s_empty();
		size_t lcount       = 0, rcount = 0;
		float  costs[kBins] = {};

		for (int i = 0; i < kBins - 1; ++i)
		{
			laccum.merge(bins[i]);
			raccum.merge(bins[kBins - 1 - i]);

			lcount               += counts[i];
			costs[i]             += lcount ? surface(laccum) * static_cast<float>(lcount) : 0.f;
			rcount               += counts[kBins - 1 - i];
			costs[kBins - 2 - i] += rcount ? surface(raccum) * static_cast<float>(rcount) : 0.f;
		}

		for (int i = 0; i < kBins - 1; ++i)
			if (costs[i] < best_cost)
			{
				best_cost = costs[i];
				best_bin  = i;
				best_axis = axis;
			}
	}

	if (best_axis == -1)
		return surface(total) * float(count);

	float best_extent = total.max.e[best_axis] - total.min.e[best_axis];

	size_t offset0 = 0, offset1 = count;

	for (size_t i = 0; i < count; ++i)
	{
		unsigned int index = order[i];
		float        p     = (boxes[index].min.e[best_axis] + boxes[index].max.e[best_axis]) * 0.5f;
		int          bin   = int((p - total.min.e[best_axis]) / best_extent * (kBins - 1) + 0.5f);
		assert(bin >= 0 && bin < kBins);

		if (bin <= best_bin)
			temp[offset0++] = index;
		else
			temp[--offset1] = index;
	}

	assert(offset0 == offset1);

	if (offset0 == 0 || offset0 == count)
		return surface(total) * float(count);

	return surface(total) + sahCost(boxes, temp, order, offset0) + sahCost(boxes, temp + offset0, order + offset0, count - offset0);
}

float ClusterGenerator::sahOverhead(const std::vector<std::vector<unsigned int>>& clusters, Span<Vertex> vertices) {
	std::vector<Box> all_tris, cluster_tris, cluster_boxes;

	float sahc = 0.f;

	for (size_t i = 0; i < clusters.size(); ++i)
	{
		if (clusters[i].empty())
			continue;

		cluster_tris.clear();

		Box cluster_box = Box::s_empty();

		for (size_t k = 0; k < clusters[i].size(); k += 3)
		{
			Box box = Box::s_empty();

			for (int v = 0; v < 3; ++v)
			{
				const Vertex& vertex = vertices[clusters[i][k + v]];
				box.includePoint(vertex.pos);
			}

			cluster_box.merge(box);

			all_tris.push_back(box);
			cluster_tris.push_back(box);
		}

		cluster_boxes.push_back(cluster_box);

		sahc += sahCost(&cluster_tris[0], cluster_tris.size());
		sahc -= surface(cluster_box); // box will be accounted for in tlas
	}

	sahc += sahCost(&cluster_boxes[0], cluster_boxes.size());

	float saht = sahCost(all_tris.data(), all_tris.size());

	return sahc / saht;
}

void ClusterGenerator::nanite(MeshObject& outMesh, Span<Vertex> vertices, Span<u32> indices)
{
	static const char* dump = NULL;
	static const char* clrt = NULL;

// #ifdef _MSC_VER
// 	static const char* dump = NULL; // tired of C4996
// 	static const char* clrt = NULL;
// #else
// 	static const char* dump = getenv("DUMP");
// 	static const char* clrt = getenv("CLRT");
// #endif

	clodConfig config = clodDefaultConfig(AX_HLSL_MESH_SHADER_MAX_VERT_COUNT);

	if (clrt)
	{
		config = clodDefaultConfigRT(config.max_triangles);
		config.cluster_fill_weight = float(atof(clrt));
	}

	const float attribute_weights[3] = {0.5f, 0.5f, 0.5f};

	clodMesh mesh = {};
	mesh.indices = indices.data();
	mesh.index_count = indices.size();
	mesh.vertex_count = vertices.size();
	mesh.vertex_positions = vertices[0].pos.data();
	mesh.vertex_positions_stride = sizeof(Vertex);
	mesh.vertex_attributes = vertices[0].normal.data();
	mesh.vertex_attributes_stride = sizeof(Vertex);
	mesh.attribute_weights = attribute_weights;
	mesh.attribute_count = sizeof(attribute_weights) / sizeof(attribute_weights[0]);
	mesh.attribute_protect_mask = (1 << 3) | (1 << 4); // protect UV seams, maps to Vertex::tx/ty

	struct Stats
	{
		size_t groups;
		size_t clusters;
		size_t triangles;
		size_t vertices;

		size_t full_clusters;
		size_t singleton_groups;

		size_t stuck_clusters;
		size_t stuck_triangles;

		double radius;

		std::vector<std::vector<unsigned int> > indices; // for detailed connectivity analysis
	};

	std::vector<Stats> stats;
	std::vector<clodBounds> groups;

	std::vector<std::vector<unsigned int> > cut;
	int cut_level = dump ? atoi(dump) : -2;

	// for testing purposes, we can compute a DAG cut from a given viewpoint and dump it as an OBJ
	Vec3f maxV(0,0,0);
	for (Int i = 0; i < vertices.size(); ++i)
	{
		maxV = Math::max(maxV, vertices[i].pos * 2);
	}

	float threshold = 2e-3f; // 2 pixels at 1080p
	float fovy = 60.f;
	float znear = 1e-2f;
	float proj = 1.f / tanf(fovy * 3.1415926f / 180.f * 0.5f);
	
	BBox3f meshBounds = BBox3f::s_empty();

	clodBuild(config, mesh, [&](clodGroup group, const clodCluster* clusters, size_t cluster_count) -> int { // clang-format!
		if (stats.size() <= size_t(group.depth))
			stats.push_back({});

		Stats& level = stats[group.depth];

		level.groups++;
		level.clusters += cluster_count;
		if (group.simplified.error == FLT_MAX)
			level.stuck_clusters += cluster_count;
		level.singleton_groups += cluster_count == 1;
		
		for (size_t i = 0; i < cluster_count; ++i)
		{
			const clodCluster& cluster = clusters[i];

			Array<AxGpuMeshletVert> outVertArray;
			Array<AxGpuMeshletPrim> outPrimArray;
			
			Dict<u32, u32> outVertIndexDict;
			
			auto getLocalVertIndex = [&](u32 index) {
				if (auto* vi = outVertIndexDict.find(index)) {
					return *vi;
				}
				
				// add new vertex
				auto& newVi = outVertIndexDict.add(index);
				auto& srcVert = vertices[index];
				newVi = ax_safe_cast_from(outVertArray.size());
				outVertArray.emplaceBack(srcVert.pack());
				
				meshBounds.includePoint(srcVert.pos);
				return newVi;
			};
			
			Int primCount = cluster.index_count / 3;
			
			for (Int j = 0; j < primCount; ++j) {
				auto* srcIndices = cluster.indices + j * 3;
				auto& outPrim = outPrimArray.emplaceBack();
				outPrim.packedTriIndices = ax_pack_tri_indices(	getLocalVertIndex(srcIndices[0]),
																getLocalVertIndex(srcIndices[1]),
																getLocalVertIndex(srcIndices[2]));
			}

			AxGpuMeshletCluster outCluster;
			outCluster.meshObjectId   = outMesh.objectSlot.slotId();
			outCluster.groupId        = ax_safe_cast_from(groups.size());
			outCluster.refinedGroupId = cluster.refined;
			outCluster.lod            = group.depth;
			
			outCluster.primOffset     = ax_safe_cast_from(outMesh.meshlet.primBuffer.count());
			outCluster.vertOffset     = ax_safe_cast_from(outMesh.meshlet.vertBuffer.count());
			outCluster.primCount      = ax_safe_cast_from(outPrimArray.size());
			outCluster.vertCount      = ax_safe_cast_from(outVertArray.size());
			
			outMesh.meshlet.primBuffer.appendValues(outPrimArray);
			outMesh.meshlet.vertBuffer.appendValues(outVertArray);
			outMesh.meshlet.clusterBuffer.appendValues(outCluster);

//-------------
			level.triangles += cluster.index_count / 3;
			if (group.simplified.error == FLT_MAX)
				level.stuck_triangles += cluster.index_count / 3;
			level.vertices += cluster.vertex_count;

			level.full_clusters += (cluster.index_count == config.max_triangles * 3);
			level.radius += cluster.bounds.radius;

			level.indices.push_back(std::vector<unsigned int>(cluster.indices, cluster.indices + cluster.index_count));

			// when requesting DAG cut at a given level, we need to render all terminal clusters at lower depth as well
			if (cut_level >= 0 && (group.depth == cut_level || (group.depth < cut_level && group.simplified.error == FLT_MAX)))
				cut.push_back(std::vector<unsigned int>(cluster.indices, cluster.indices + cluster.index_count));

			// when requesting DAG cut from a viewpoint, we need to check if each cluster is the least detailed cluster that passes the error threshold
			if (cut_level == -1 
				&& (cluster.refined < 0 || boundsError(groups[cluster.refined], maxV, proj, znear) <= threshold) 
				&& boundsError(group.simplified, maxV, proj, znear) > threshold)
				cut.push_back(std::vector<unsigned int>(cluster.indices, cluster.indices + cluster.index_count));
		}

		level.indices.push_back(std::vector<unsigned int>()); // mark end of group for measureBoundary

		{
			AxGpuMeshletGroup dst = {};
			dst.center = Vec3f(group.simplified.center[0], group.simplified.center[1], group.simplified.center[2]);
			dst.clusterError = group.simplified.error;
			dst.radius       = group.simplified.radius;
			dst.clusterCount = ax_safe_cast_from(cluster_count);
			dst.meshObjectId = outMesh.objectSlot.slotId();
			outMesh.meshlet.groupBuffer.appendValues(dst);
		}
		
		groups.push_back(group.simplified);
		return int(groups.size() - 1);
	});
	
	outMesh.setBounds(meshBounds);
	
	// for cluster connectivity analysis and boundary statistics, we need a position-only remap that maps vertices with the same position to the same index
	std::vector<unsigned int> remap(vertices.size());
	meshopt_generatePositionRemap(&remap[0], vertices[0].pos.data(), vertices.size(), sizeof(Vertex));

	std::vector<int> used(vertices.size());

	size_t lowest_clusters = 0;
	size_t lowest_triangles = 0;

	for (size_t i = 0; i < stats.size(); ++i)
	{
		Stats& level = stats[i];

		lowest_clusters += level.stuck_clusters;
		lowest_triangles += level.stuck_triangles;

		size_t connected = 0;
		for (const auto& cluster : level.indices)
			connected += measureComponents(used, cluster, remap);

		size_t boundary = measureBoundary(used, level.indices, remap);
		float saho = clrt ? sahOverhead(level.indices, vertices) : 0.f;

		double inv_clusters = 1.0 / double(level.clusters);

		printf("lod %d: %d clusters (%.1f%% full, %.1f tri/cl, %.1f vtx/cl, %.2f connected, %.1f boundary, %.1f partition, %d singletons, %.3f sah overhead, %f radius), %d triangles",
		    int(i), int(level.clusters),
		    double(level.full_clusters) * inv_clusters * 100, double(level.triangles) * inv_clusters, double(level.vertices) * inv_clusters,
		    double(connected) * inv_clusters, double(boundary) * inv_clusters,
		    double(level.clusters) / double(level.groups), int(level.singleton_groups),
		    saho, level.radius * inv_clusters,
		    int(level.triangles));
		if (level.stuck_clusters && level.clusters > 1)
			printf("; stuck %d clusters (%d triangles)", int(level.stuck_clusters), int(level.stuck_triangles));
		printf("\n");
	}

	printf("lowest lod: %d clusters, %d triangles\n", int(lowest_clusters), int(lowest_triangles));

	if (cut_level >= -1)
	{
		size_t cut_tris = 0;
		for (auto& cluster : cut)
			cut_tris += cluster.size() / 3;

		if (cut_level >= 0)
			printf("cut (level %d): %d triangles\n", cut_level, int(cut_tris));
		else
			printf("cut (error %.3f): %d triangles\n", threshold, int(cut_tris));

		dumpWaveFrontObjFile(vertices, Span<u32>());

		for (auto& cluster : cut)
			dumpWaveFrontObjFile("cluster", cluster);
	}
}
	
void ClusterGenerator::dumpWaveFrontObjFile(Span<Vertex> vertices, Span<u32> indices, bool recomputeNormals)
{
	std::vector<float> normals;

	if (recomputeNormals)
	{
		normals.resize(vertices.size() * 3);

		for (Int i = 0; i < indices.size(); i += 3)
		{
			unsigned int a = indices[i], b = indices[i + 1], c = indices[i + 2];

			const Vertex& va = vertices[a];
			const Vertex& vb = vertices[b];
			const Vertex& vc = vertices[c];
			auto nl = (vb.pos - va.pos).cross(vc.pos - va.pos);

			for (int k = 0; k < 3; ++k)
			{
				unsigned int index = indices[i + k];

				normals[index * 3 + 0] += nl.x;
				normals[index * 3 + 1] += nl.y;
				normals[index * 3 + 2] += nl.z;
			}
		}
	}

	for (Int i = 0; i < vertices.size(); ++i)
	{
		const Vertex& v = vertices[i];

		float nx = v.normal.x, ny = v.normal.y, nz = v.normal.z;

		if (recomputeNormals)
		{
			nx = normals[i * 3 + 0];
			ny = normals[i * 3 + 1];
			nz = normals[i * 3 + 2];

			float l = sqrtf(nx * nx + ny * ny + nz * nz);
			float s = l == 0.f ? 0.f : 1.f / l;

			nx *= s;
			ny *= s;
			nz *= s;
		}

		fprintf(stderr, "v %f %f %f\n", v.pos.x, v.pos.y, v.pos.z);
		fprintf(stderr, "vn %f %f %f\n", nx, ny, nz);
	}

	for (Int i = 0; i < indices.size(); i += 3)
	{
		unsigned int a = indices[i], b = indices[i + 1], c = indices[i + 2];

		fprintf(stderr, "f %d//%d %d//%d %d//%d\n", a + 1, a + 1, b + 1, b + 1, c + 1, c + 1);
	}
}

void ClusterGenerator::dumpWaveFrontObjFile(const char* section, const std::vector<unsigned int>& indices)
{
	fprintf(stderr, "o %s\n", section);

	for (size_t j = 0; j < indices.size(); j += 3)
	{
		unsigned int a = indices[j], b = indices[j + 1], c = indices[j + 2];

		fprintf(stderr, "f %d//%d %d//%d %d//%d\n", a + 1, a + 1, b + 1, b + 1, c + 1, c + 1);
	}
}

} // namespace
