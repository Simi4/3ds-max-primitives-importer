#include "bw_model.h"
#include "bw_visual.h"
#include "bw_primitives.h"
#include "bw_vertextypes.h"
#include <boost/algorithm/string.hpp>
#include <filesystem>
// #include <meshdelta.h>


static int pre_load_vertices(BWPrimitives& prim, const std::string& vres_name, Mesh& mesh);
static int pre_load_uv2(BWPrimitives& prim, const std::string& uv2_name, Mesh& mesh);
static int pre_load_indices(BWPrimitives& prim, const std::string& prim_name, Mesh& mesh);


BWModel::BWModel()
{
}


BWModel::~BWModel()
{
}


int BWModel::load(std::string primitives_path_str, ImpInterface *i)
{
	namespace fs = std::filesystem;

	boost::algorithm::to_lower(primitives_path_str); // case insensitive
	auto primitives_path = fs::path(primitives_path_str);

	if (!primitives_path.has_extension()) {
		ERROR_MSG("Primitives file has no extension");
		return 1;
	}

	if (!fs::exists(primitives_path)) {
		ERROR_MSG("Primitives file does not exist");
		return 2;
	}

	auto visual_path = primitives_path;
	if (primitives_path.extension() == ".primitives") {
		visual_path.replace_extension(".visual");
	}
	else if (primitives_path.extension() == ".primitives_processed") {
		visual_path.replace_extension(".visual_processed");
	}
	else {
		ERROR_MSG("Wrong file format");
		return 3;
	}

	if (!fs::exists(visual_path)) {
		ERROR_MSG("Visual file does not exist");
		return 4; // TODO: not critical
	}
	else {
		visual = std::make_unique<BWVisual>();
		if (visual->load(visual_path.string())) {
			ERROR_MSG("Visual file is incorrect");
			return 5; // TODO: not critical
		}
	}

	BWPrimitives prim(primitives_path_str);

	for (const auto& renderSet : visual->renderSets()) {
		ImpNode *node = i->CreateNode();
		if (!node) {
			ERROR_MSG("Unable to create node");
			return 1;
		}

		TriObject *triObject = CreateNewTriObject();
		node->Reference(triObject);
		node->SetName("primitives"); // TODO: use name from visual

		Mesh& mesh = triObject->GetMesh();

		mesh.setNumMaps(2, TRUE);

		if (pre_load_vertices(prim, renderSet.vres_name, mesh)) {
			return 2;
		}

		if (renderSet.uv2_name.length()) {
			pre_load_uv2(prim, renderSet.uv2_name, mesh);
		}

		if (pre_load_indices(prim, renderSet.pres_name, mesh)) {
			return 3;
		}

		/*
		{
			// weld vertices
			MeshDelta tmd(mesh);
			BitArray vTempSel;
			vTempSel.SetSize(mesh.getNumVerts());
			vTempSel.SetAll();
			tmd.WeldByThreshold(mesh, vTempSel, 0.0f);
			tmd.Apply(mesh);
		}
		*/

		mesh.buildNormals();

		i->AddNodeToScene(node);
	}

	return 0;
}



template <class T>
static void load_vertices(const char*, uint32_t, Mesh&);

static int pre_load_vertices(BWPrimitives& prim, const std::string& vres_name, Mesh& mesh)
{
	std::vector<char> vertBuf;
	if (prim.openSection(vres_name, vertBuf)) {
		return 1;
	}

	char *dataPtr = vertBuf.data();
	std::string vertices_subname(dataPtr, strnlen(dataPtr, 64));
	dataPtr += 64;

	std::string vertexFormat;

	bool flgNewFormat = false;
	if (boost::starts_with(vertices_subname, "BPVT")) {
		dataPtr += 4;
		vertexFormat.assign(dataPtr, strnlen(dataPtr, 64));
		dataPtr += 64;
		flgNewFormat = true;
	}

	uint32_t verticesCount = *reinterpret_cast<uint32_t*>(dataPtr);
	dataPtr += 4;

	if (flgNewFormat) {
		if (set3_xyznuviiiwwtbpc_s::check_vertex_type(vertexFormat)) {
			load_vertices<set3_xyznuviiiwwtbpc_s>(dataPtr, verticesCount, mesh);
		}
		else if (set3_xyznuvtbpc_s::check_vertex_type(vertexFormat)) {
			load_vertices<set3_xyznuvtbpc_s>(dataPtr, verticesCount, mesh);
		}
		else if (set3_xyznuvpc_s::check_vertex_type(vertexFormat)) {
			load_vertices<set3_xyznuvpc_s>(dataPtr, verticesCount, mesh);
		}
		else {
			ERROR_MSG("Unknown vertexFormat");
			return 2;
		}
	}
	else {
		if (xyznuviiiwwtb_s::check_vertex_type(vertices_subname)) {
			load_vertices<xyznuviiiwwtb_s>(dataPtr, verticesCount, mesh);
		}
		else if (xyznuvtb_s::check_vertex_type(vertices_subname)) {
			load_vertices<xyznuvtb_s>(dataPtr, verticesCount, mesh);
		}
		else if (xyznuv_s::check_vertex_type(vertices_subname)) {
			load_vertices<xyznuv_s>(dataPtr, verticesCount, mesh);
		}
		else {
			ERROR_MSG("Unknown vertices_subname");
			return 3;
		}
	}

	return 0;
}


template <class T>
static void load_vertices(const char* buf, uint32_t vcount, Mesh& mesh)
{
	const T *data = reinterpret_cast<const T*>(buf);

	// Sets the number of geometric vertices in the mesh
	if (!mesh.setNumVerts(vcount)) {
		assert(false);
	}

	// Sets the number of texture vertices (in mapping channel 1)
	if (!mesh.setNumTVerts(vcount)) {
		assert(false);
	}

	for (int i = 0; i < vcount; ++i) {
		const T& vert = data[i];
		mesh.setVert(i, vert.position());
		mesh.setNormal(i, vert.normal());
		mesh.setTVert(i, vert.uvw());
	}

	mesh.setMapSupport(1, TRUE);
}


#pragma pack(push, 1)
struct PrimitiveGroup {
	uint32_t startIndex;
	uint32_t nPrimitives;
	uint32_t startVertex;
	uint32_t nVertices;
};
static_assert(sizeof(PrimitiveGroup) == 16);
#pragma pack(pop)


template <typename T>
static void load_indices(const char* buf, Mesh& mesh, uint32_t, uint32_t);

static int pre_load_indices(BWPrimitives& prim, const std::string& pres_name, Mesh& mesh)
{
	std::vector<char> primBuf;
	if (prim.openSection(pres_name, primBuf)) {
		return 1;
	}

	char *dataPtr = primBuf.data();
	std::string indexFormat(dataPtr, strnlen(dataPtr, 64));
	dataPtr += 64;

	uint32_t nIndices = *reinterpret_cast<uint32_t*>(dataPtr);
	dataPtr += 4;

	uint32_t nTriangleGroups = *reinterpret_cast<uint32_t*>(dataPtr);
	dataPtr += 4;

	if (indexFormat == "list") {
		load_indices<uint16_t>(dataPtr, mesh, nIndices, nTriangleGroups);
	}
	else if (indexFormat == "list32") {
		load_indices<uint32_t>(dataPtr, mesh, nIndices, nTriangleGroups);
	}
	else {
		ERROR_MSG("Unknown indexFormat");
		return 2;
	}

	return 0;
}


template <typename T>
static void load_indices(const char* buf, Mesh& mesh,
	uint32_t nIndices, uint32_t nTriangleGroups)
{
	const T *data = reinterpret_cast<const T*>(buf);

	const PrimitiveGroup* pg = reinterpret_cast<const PrimitiveGroup*>(
		buf + nIndices * sizeof(T));

	int n = nIndices / 3;
	int nm = mesh.getNumMaps();

	mesh.setNumFaces(n);
	mesh.setNumTVFaces(nm);

	for (int j = 1; j <= nm; ++j)
		mesh.setNumMapFaces(j, n, TRUE);

	for (int matId = 0; matId < nTriangleGroups; matId++) {
		int begP = pg[matId].startIndex / 3;
		int endP = begP + pg[matId].nPrimitives;
		for (int i = begP; i < endP; ++i) {
			int a = data[i * 3 + 0];
			int b = data[i * 3 + 1];
			int c = data[i * 3 + 2];

			Face& f = mesh.faces[i];
			f.setVerts(c, b, a);
			f.setMatID(matId);

			for (int j = 1; j <= nm; ++j) {
				if (TVFace *tvFace = mesh.mapFaces(j)) {
					tvFace[i].setTVerts(c, b, a);
				}
			}
		}
	}
}


static void load_uv2(const char*, Mesh&, uint32_t);

static int pre_load_uv2(BWPrimitives& prim, const std::string& uv2_name, Mesh& mesh)
{
	std::vector<char> uv2Buf;
	if (prim.openSection(uv2_name, uv2Buf)) {
		return 1;
	}

	mesh.setNumMaps(3, TRUE);

	char *dataPtr = uv2Buf.data();
	std::string uv2_subname(dataPtr, strnlen(dataPtr, 64));
	dataPtr += 64;

	uint32_t uv2_count = mesh.getNumVerts();

	if (boost::starts_with(uv2_subname, "BPVS")) {
		dataPtr += 4;
		dataPtr += 64;
		// if set3/uv2pc
		uv2_count = *reinterpret_cast<uint32_t*>(dataPtr);
		dataPtr += 4;
	}
	else {
		dataPtr = uv2Buf.data(); // reset pos
	}

	load_uv2(dataPtr, mesh, uv2_count);

	return 0;
}


static void load_uv2(const char* buf, Mesh& mesh, uint32_t uv2_count)
{
	const Point2 *data = reinterpret_cast<const Point2*>(buf);

	mesh.setNumMapVerts(2, uv2_count, TRUE);

	for (int i = 0; i < uv2_count; ++i) {
		mesh.setMapVert(2, i, UVVert(data[i].x, 1.0f - data[i].y, 0.0f));
	}

	mesh.setMapSupport(2, TRUE);
}
