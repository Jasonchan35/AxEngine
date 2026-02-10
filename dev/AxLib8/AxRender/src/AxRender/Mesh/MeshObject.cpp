module;

#include "meshoptimizer.h"
#include "AxClusterLod.h"

module AxRender;
import :MeshObject;
import :RenderObjectManager_Backend;
import :EditableMesh;

namespace ax {

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<MeshObject_Backend> o;
	RenderObjectManager_Backend::s_instance()->getOrNewObject(o, req, desc, desc.assetPath);
	return o;
}

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req) {
	MeshObject_CreateDesc desc;
	return s_new(req, desc);
}

MeshObject::MeshObject(const CreateDesc& desc)
	: _assetPath(desc.assetPath)
	, objectSlot(this) 
{
	_gpuData.bounds = BBox3f::s_empty();
}

auto MeshObject::onGetGpuData(RenderRequest* req) -> const GpuData*{
	if (meshletInfo.size() <= 0) return nullptr;
		
	meshletVert.buffer->getUploadedGpuBuffer(req);
	meshletPrim.buffer->getUploadedGpuBuffer(req);

	u32  vertOffset  = ax_safe_cast_from(meshletVert.buffer->gpuBufferIndex());
	u32  primOffset  = ax_safe_cast_from(meshletPrim.buffer->gpuBufferIndex());
	auto srcMeshlets = meshletInfo.span();
	auto dstMeshlets = meshlet.editData(0, srcMeshlets.size());
	
	for (Int i = 0; i < srcMeshlets.size(); ++i) {
		auto& dst = dstMeshlets[i];
		dst = srcMeshlets[i];
		dst.draw.vertOffset += vertOffset;
		dst.draw.primOffset += primOffset;
	}
	
	meshlet.buffer->getUploadedGpuBuffer(req);

	_gpuData.meshletOffset  = ax_safe_cast_from(meshlet.buffer->gpuBufferIndex());
	_gpuData.meshletCount   = ax_safe_cast_from(meshlet.buffer->count());
	_gpuData.totalVertCount = ax_safe_cast_from(meshletVert.buffer->count());
	_gpuData.totalPrimCount = ax_safe_cast_from(meshletPrim.buffer->count());
	return &_gpuData;
}

void MeshObject::createBuffers() {
	auto* objMgr = RenderObjectManager_Backend::s_instance();
	    meshlet.create(AX_NEW, "axGpuMeshlet"    , objMgr->_structBufferPools.axMeshlet);
	meshletVert.create(AX_NEW, "axGpuMeshletVert", objMgr->_structBufferPools.axMeshletVert);
	meshletPrim.create(AX_NEW, "axGpuMeshletPrim", objMgr->_structBufferPools.axMeshletPrim);
}

void MeshObject::createFromEditableMesh(const EditableMesh& srcMesh) {
	createBuffers();
	constexpr Int kMaxVertexCountPerMeshlet = AX_HLSL_MESH_SHADER_MAX_VERT_COUNT;
	
	meshletInfo.clear();
	auto* curMeshlet = &meshletInfo.emplaceBack();
	*curMeshlet = {};
	
	BBox3f bounds = BBox3f::s_empty();
	for (auto& pt : srcMesh.points()) {
		bounds.includePoint(Vec3f::s_cast(pt.pos));
	}
	_gpuData.bounds = bounds;

	Array<Vec3d, 64> facePositions;
	for (auto& face : srcMesh.faces()) {
		auto fvCount = static_cast<u32>(face.pointCount());
		if (fvCount < 3) { AX_ASSERT(false); continue; }
		u32 triCount = fvCount - 2;
		
		if (curMeshlet->draw.vertCount + fvCount > kMaxVertexCountPerMeshlet) {
			if (fvCount > kMaxVertexCountPerMeshlet) throw Error_Undefined(Fmt("Face vertex count > kMaxVertexCountPerMeshlet"));
			
			u32 nextVertOffset = curMeshlet->draw.vertCount + curMeshlet->draw.vertOffset;
			u32 nextPrimOffset = curMeshlet->draw.primCount + curMeshlet->draw.primOffset;
			
			curMeshlet = &meshletInfo.emplaceBack();
			*curMeshlet = {};
			curMeshlet->draw.vertOffset = nextVertOffset;
			curMeshlet->draw.primOffset = nextPrimOffset;
		}
		
		auto dstVertices = meshletVert.extendsData(fvCount);
		auto srcNormals = face.getNormals(srcMesh);
		
		face.getPositions(srcMesh, facePositions);
		for (Int j = 0; j < fvCount; ++j) {
			auto& dstVert = dstVertices[j];
			dstVert.pos      = Vec3f::s_cast(facePositions[j]);
			dstVert.normal   = Vec3f::s_cast(srcNormals[j]);
			dstVert.rawColor = 0xffffffff;
		}

		u32 viBase = curMeshlet->draw.vertCount;
		auto dstIdx  = meshletPrim.extendsData(triCount);
		for (u32 j = 0; j < triCount; ++j) {
			// triangle fan
			dstIdx[j].tri = u32x3(0, j+1, j+2) + viBase;
		}

		curMeshlet->draw.vertCount += fvCount;
		curMeshlet->draw.primCount += triCount;
	}
	
	objectSlot.markDirty();
}

//---------------------------------------------

class ClusterGenerator {
public:	
	
using Vertex = AxGpuMeshletVert;
using BBox   = BBox3f;

// from meshoptimizer demo

// computes approximate (perspective) projection error of a cluster in screen space (0..1; multiply by screen height to get pixels)
// camera_proj is projection[1][1], or cot(fovy/2); camera_znear is *positive* near plane distance
// for DAG cut to be valid, boundsError must be monotonic: it must return a larger error for parent cluster
// for simplicity, we ignore perspective distortion and use rotationally invariant projection size estimation
static float boundsError(const clodBounds& bounds, float camera_x, float camera_y, float camera_z, float camera_proj, float camera_znear)
{
	float dx = bounds.center[0] - camera_x, dy = bounds.center[1] - camera_y, dz = bounds.center[2] - camera_z;
	float d = sqrtf(dx * dx + dy * dy + dz * dz) - bounds.radius;
	return bounds.error / (d > camera_znear ? d : camera_znear) * (camera_proj * 0.5f);
}

// What follows is code that is helpful for collecting metrics, visualizing cuts, etc.
// This code is not used in the actual clustering implementation and can be ignored.
static Int follow(Array<Int>& parents, Int index)
{
	while (index != parents[index])
	{
		Int parent = parents[index];
		parents[index] = parents[parent];
		index = parent;
	}

	return index;
}

static Int measureComponents(Array<Int>& parents, const Array<u32>& indices, const Array<u32>& remap)
{
	assert(parents.size() == remap.size());

	for (Int i = 0; i < indices.size(); ++i)
	{
		Int v = remap[indices[i]];
		parents[v] = v;
	}

	for (Int i = 0; i < indices.size(); ++i)
	{
		Int v0 = remap[indices[i]];
		Int v1 = remap[indices[i + (i % 3 == 2 ? -2 : 1)]];

		v0 = follow(parents, v0);
		v1 = follow(parents, v1);

		parents[v0] = v1;
	}

	for (Int i = 0; i < indices.size(); ++i)
	{
		Int v = remap[indices[i]];
		parents[v] = follow(parents, v);
	}

	Int roots = 0;
	for (Int i = 0; i < indices.size(); ++i)
	{
		Int v = remap[indices[i]];
		roots += parents[v] == Int(v);
		parents[v] = -1; // make sure we only count each root once
	}

	return roots;
}

static Int measureBoundary(Array<Int>& used, const Array<Array<u32> >& clusters, const Array<u32>& remap)
{
	for (Int i = 0; i < used.size(); ++i)
		used[i] = -1;

	// mark vertices that are used by multiple groups with -2
	Int group = 0;

	for (Int i = 0; i < clusters.size(); ++i)
	{
		group += clusters[i].size() <= 0;

		for (Int j = 0; j < clusters[i].size(); ++j)
		{
			Int v = remap[clusters[i][j]];

			used[v] = (used[v] == -1 || used[v] == group) ? group : -2;
		}
	}

	Int result = 0;

	for (Int i = 0; i < clusters.size(); ++i)
	{
		// count vertices that are used by multiple groups and change marks to -1
		for (Int j = 0; j < clusters[i].size(); ++j)
		{
			Int v = remap[clusters[i][j]];

			result += (used[v] == -2);
			used[v] = (used[v] == -2) ? -1 : used[v];
		}

		// change marks back from -1 to -2 for the next pass
		for (Int j = 0; j < clusters[i].size(); ++j)
		{
			Int v = remap[clusters[i][j]];

			used[v] = (used[v] == -1) ? -2 : used[v];
		}
	}

	return Int(result);
}

static inline float surface(const BBox& box)
{
	auto s = box.size();
	return s.lengthSq();
}
	
static float sahCost(const BBox* boxes, unsigned int* order, unsigned int* temp, Int count)
{
	BBox total = boxes[order[0]];
	for (Int i = 1; i < count; ++i)
		total.merge(boxes[order[i]]);

	int best_axis = -1;
	int best_bin = -1;
	float best_cost = FLT_MAX;

	const int kBins = 15;

	for (int axis = 0; axis < 3; ++axis)
	{
		BBox bins[kBins];
		u32 counts[kBins] = {};

		float extent = total.max.e[axis] - total.min.e[axis];
		if (extent <= 0.f)
			continue;
		
		for (int i = 0; i < kBins; ++i)
			bins[i] = BBox::s_empty();

		for (Int i = 0; i < count; ++i)
		{
			unsigned int index = order[i];
			float p = (boxes[index].min.e[axis] + boxes[index].max.e[axis]) * 0.5f;
			int bin = int((p - total.min.e[axis]) / extent * (kBins - 1) + 0.5f);
			assert(bin >= 0 && bin < kBins);

			bins[bin].merge(boxes[index]);
			counts[bin]++;
		}

		BBox laccum = BBox::s_empty(), raccum = BBox::s_empty();
		Int lcount = 0, rcount = 0;
		float costs[kBins] = {};

		for (int i = 0; i < kBins - 1; ++i)
		{
			laccum.merge(bins[i]);
			raccum.merge(bins[kBins - 1 - i]);

			lcount += counts[i];
			costs[i] += lcount ? surface(laccum) * static_cast<f32>(lcount) : 0.f;
			rcount += counts[kBins - 1 - i];
			costs[kBins - 2 - i] += rcount ? surface(raccum) * static_cast<f32>(rcount) : 0.f;
		}

		for (int i = 0; i < kBins - 1; ++i)
			if (costs[i] < best_cost)
			{
				best_cost = costs[i];
				best_bin = i;
				best_axis = axis;
			}
	}

	if (best_axis == -1)
		return surface(total) * float(count);

	float best_extent = total.max.e[best_axis] - total.min.e[best_axis];

	Int offset0 = 0, offset1 = count;

	for (Int i = 0; i < count; ++i)
	{
		unsigned int index = order[i];
		float p = (boxes[index].min.e[best_axis] + boxes[index].max.e[best_axis]) * 0.5f;
		int bin = int((p - total.min.e[best_axis]) / best_extent * (kBins - 1) + 0.5f);
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

static float sahCost(const BBox* boxes, Int count)
{
	if (count == 0)
		return 0.f;

	Array<u32> order;
	order.resize(count);
	for (Int i = 0; i < count; ++i)
		order[i] = u32(i);

	Array<u32> temp;
	temp.resize(count);
	return sahCost(boxes, &order[0], &temp[0], count);
}


static float sahOverhead(const Array<Array<u32>> & clusters, Span<Vertex> vertices)
{
	Array<BBox> all_tris, cluster_tris, cluster_boxes;

	float sahc = 0.f;

	for (Int i = 0; i < clusters.size(); ++i)
	{
		if (clusters[i].size() <= 0)
			continue;

		cluster_tris.clear();

		BBox cluster_box = BBox::s_empty();

		for (Int k = 0; k < clusters[i].size(); k += 3)
		{
			BBox box = BBox::s_empty();

			for (int v = 0; v < 3; ++v)
			{
				const Vertex& vertex = vertices[clusters[i][k + v]];
				box.includePoint(vertex.pos);
			}

			cluster_box.merge(box);

			all_tris.emplaceBack(box);
			cluster_tris.emplaceBack(box);
		}

		cluster_boxes.emplaceBack(cluster_box);

		sahc += sahCost(&cluster_tris[0], cluster_tris.size());
		sahc -= surface(cluster_box); // box will be accounted for in tlas
	}

	sahc += sahCost(&cluster_boxes[0], cluster_boxes.size());

	float saht = sahCost(all_tris.data(), all_tris.size());

	return sahc / saht;
}


void nanite(Span<Vertex> vertices, Span<u32> indices)
{
#ifdef _MSC_VER
	static const char* dump = NULL; // tired of C4996
	static const char* clrt = NULL;
#else
	static const char* dump = getenv("DUMP");
	static const char* clrt = getenv("CLRT");
#endif

	clodConfig config = clodDefaultConfig(/*max_triangles=*/128);

	if (clrt)
	{
		config = clodDefaultConfigRT(config.max_triangles);
		config.cluster_fill_weight = float(atof(clrt));
	}

	const float attribute_weights_data[] = {0.5f, 0.5f, 0.5f};
	auto attribute_weights = Span(attribute_weights_data);

	clodMesh mesh                 = {};
	mesh.indices                  = indices.data();
	mesh.index_count              = indices.size();
	mesh.vertex_count             = vertices.size();
	mesh.vertex_positions         = vertices[0].pos.data();
	mesh.vertex_positions_stride  = sizeof(vertices[0]);
	mesh.vertex_attributes        = vertices[0].normal.data();
	mesh.vertex_attributes_stride = sizeof(vertices[0]);
	mesh.attribute_weights        = attribute_weights.data();
	mesh.attribute_count          = ax_safe_cast_from(attribute_weights.size());
	mesh.attribute_protect_mask   = (1 << 3) | (1 << 4); // protect UV seams, maps to Vertex::tx/ty

	struct Stats
	{
		Int groups;
		Int clusters;
		Int triangles;
		Int vertices;

		Int full_clusters;
		Int singleton_groups;

		Int stuck_clusters;
		Int stuck_triangles;

		double radius;

		Array<Array<u32> > indices; // for detailed connectivity analysis
	};

	Array<Stats> stats;
	Array<clodBounds> groups;

	Array<Array<u32> > cut;
	int cut_level = dump ? atoi(dump) : -2;

	// for testing purposes, we can compute a DAG cut from a given viewpoint and dump it as an OBJ
	float maxx = 0.f, maxy = 0.f, maxz = 0.f;
	for (Int i = 0; i < vertices.size(); ++i)
	{
		maxx = std::max(maxx, vertices[i].pos.x * 2);
		maxy = std::max(maxy, vertices[i].pos.y * 2);
		maxz = std::max(maxz, vertices[i].pos.z * 2);
	}

	float threshold = 2e-3f; // 2 pixels at 1080p
	float fovy = 60.f;
	float znear = 1e-2f;
	float proj = 1.f / tanf(fovy * 3.1415926f / 180.f * 0.5f);

	clodBuild(config, mesh, [&](clodGroup group, const clodCluster* clusters, size_t cluster_count) -> int { // clang-format!
		if (stats.size() <= group.depth)
			stats.emplaceBack() = {};

		Stats& level = stats[group.depth];

		level.groups++;
		level.clusters += cluster_count;
		if (group.simplified.error == FLT_MAX)
			level.stuck_clusters += cluster_count;
		level.singleton_groups += cluster_count == 1;

		for (size_t i = 0; i < cluster_count; ++i)
		{
			const clodCluster& cluster = clusters[i];

			level.triangles += cluster.index_count / 3;
			if (group.simplified.error == FLT_MAX)
				level.stuck_triangles += cluster.index_count / 3;
			level.vertices += cluster.vertex_count;

			level.full_clusters += (cluster.index_count == config.max_triangles * 3);
			level.radius += cluster.bounds.radius;

			level.indices.emplaceBack() = Span(cluster.indices, cluster.index_count);

			// when requesting DAG cut at a given level, we need to render all terminal clusters at lower depth as well
			if (cut_level >= 0 && (group.depth == cut_level || (group.depth < cut_level && group.simplified.error == FLT_MAX)))
				cut.emplaceBack() = Span(cluster.indices, cluster.index_count);

			// when requesting DAG cut from a viewpoint, we need to check if each cluster is the least detailed cluster that passes the error threshold
			if (cut_level == -1 && (cluster.refined < 0 || boundsError(groups[cluster.refined], maxx, maxy, maxz, proj, znear) <= threshold) && boundsError(group.simplified, maxx, maxy, maxz, proj, znear) > threshold)
				cut.emplaceBack() = Span(cluster.indices, cluster.index_count);
		}

		level.indices.emplaceBack(); // mark end of group for measureBoundary

		groups.emplaceBack(group.simplified);
		return int(groups.size() - 1);
	});

	// for cluster connectivity analysis and boundary statistics, we need a position-only remap that maps vertices with the same position to the same index
	Array<u32> remap(static_cast<u32>(vertices.size()));
	meshopt_generatePositionRemap(&remap[0], vertices[0].pos.data(), vertices.size(), sizeof(Vertex));

	Array<Int> used(vertices.size());

	Int lowest_clusters = 0;
	Int lowest_triangles = 0;

	for (Int i = 0; i < stats.size(); ++i)
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

#if 0 // TODO
		dumpObj(vertices, std::vector<unsigned int>());

		for (auto& cluster : cut)
			dumpObj("cluster", cluster);
#endif		
	}
}
}; // ClusterGenerator

void MeshObject::createFromEditableMesh2(const EditableMesh& srcMesh) {
	
}

} // namespace
