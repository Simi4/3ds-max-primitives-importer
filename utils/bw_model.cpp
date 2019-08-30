#include "bw_model.h"
#include "bw_visual.h"
#include "bw_primitives.h"
#include "bw_vertextypes.h"
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <shaders.h>
#include <meshdelta.h>


BWModel::BWModel(ImpInterface *i, Interface *gi) : i(i), gi(gi)
{
}


BWModel::~BWModel()
{
}


int BWModel::load(MaxSDK::Util::Path max_primitives_path)
{
	max_primitives_path.ConvertToLowerCase();

	namespace fs = std::filesystem;
	auto primitives_path = fs::path(max_primitives_path.GetCStr());

	if (!fs::exists(primitives_path)) {
		ERROR_MSG("Primitives file does not exist");
		return 1;
	}

	if (!primitives_path.has_extension()) {
		ERROR_MSG("Primitives file has no extension");
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

	BWPrimitives prim(primitives_path.string());

	for (const auto& renderSet : visual->renderSets()) {
		node = i->CreateNode();
		if (!node) {
			ERROR_MSG("Unable to create node");
			return 1;
		}

		TriObject *triObject = CreateNewTriObject();
		node->Reference(triObject);
		node->SetName(primitives_path.stem().string().c_str()); // TODO: use name from visual

		Mesh& mesh = triObject->GetMesh();
		meshPtr = &mesh;

		if (pre_load_vertices(prim, renderSet->vertices_name)) {
			return 2;
		}

		if (pre_load_indices(renderSet, prim)) {
			return 3;
		}

		for (const auto& name : renderSet->stream_names) {
			if (boost::ends_with(name, "uv2")) {
				if (pre_load_uv2(prim, name)) {
					return 4;
				}
			}
			else if (boost::ends_with(name, "colour")) {
				if (pre_load_colour(prim, name)) {
					return 5;
				}
			}
			else {
				assert(false);
				return 6;
			}
		}


		mesh.buildNormals();

		{
			// weld vertices
			MeshDelta tmd(mesh);
			BitArray vTempSel;
			vTempSel.SetSize(mesh.getNumVerts());
			vTempSel.SetAll();
			tmd.WeldByThreshold(mesh, vTempSel, 0.0f);
			tmd.Apply(mesh);
		}

		i->AddNodeToScene(node);
		//node->GetINode()->EvalWorldState(0);
	}

	return 0;
}


int BWModel::pre_load_vertices(BWPrimitives& prim, const std::string& vres_name)
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

	meshPtr->setNumVerts(verticesCount);
	meshPtr->setMapSupport(1, TRUE);
	meshPtr->setNumTVerts(verticesCount);

	if (flgNewFormat) {
		if (set3_xyznuviiiwwtbpc_s::check_vertex_type(vertexFormat)) {
			load_vertices<set3_xyznuviiiwwtbpc_s>(dataPtr);
		}
		else if (set3_xyznuvtbpc_s::check_vertex_type(vertexFormat)) {
			load_vertices<set3_xyznuvtbpc_s>(dataPtr);
		}
		else if (set3_xyznuvpc_s::check_vertex_type(vertexFormat)) {
			load_vertices<set3_xyznuvpc_s>(dataPtr);
		}
		else {
			ERROR_MSG("Unknown vertexFormat");
			return 2;
		}
	}
	else {
		if (xyznuviiiwwtb_s::check_vertex_type(vertices_subname)) {
			load_vertices<xyznuviiiwwtb_s>(dataPtr);
		}
		else if (xyznuvtb_s::check_vertex_type(vertices_subname)) {
			load_vertices<xyznuvtb_s>(dataPtr);
		}
		else if (xyznuv_s::check_vertex_type(vertices_subname)) {
			load_vertices<xyznuv_s>(dataPtr);
		}
		else {
			ERROR_MSG("Unknown vertices_subname");
			return 3;
		}
	}

	return 0;
}


template <class T>
void BWModel::load_vertices(const char* buf)
{
	const T *data = reinterpret_cast<const T*>(buf);

	Mesh& mesh = *meshPtr;
	for (int i = 0; i < mesh.getNumVerts(); ++i) {
		const T& vert = data[i];
		mesh.setVert(i, vert.position());
		mesh.setNormal(i, vert.normal());
		mesh.setTVert(i, vert.uvw());
	}
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


int BWModel::pre_load_indices(RenderSetPtr renderSet, BWPrimitives& prim)
{
	std::vector<char> primBuf;
	if (prim.openSection(renderSet->primitive_name, primBuf)) {
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
		load_indices<uint16_t>(renderSet, dataPtr, nIndices, nTriangleGroups);
	}
	else if (indexFormat == "list32") {
		load_indices<uint32_t>(renderSet, dataPtr, nIndices, nTriangleGroups);
	}
	else {
		ERROR_MSG("Unknown indexFormat");
		return 2;
	}

	return 0;
}


template <typename T>
void BWModel::load_indices(RenderSetPtr renderSet, const char* buf,	uint32_t nIndices, uint32_t nTriangleGroups)
{
	Mesh& mesh = *meshPtr;

	const T *data = reinterpret_cast<const T*>(buf);

	const PrimitiveGroup* pg = reinterpret_cast<const PrimitiveGroup*>(
		buf + nIndices * sizeof(T));

	int n = nIndices / 3;

	mesh.setNumFaces(n);
	mesh.setNumMapFaces(1, n, TRUE);
	TVFace *tvFace = mesh.mapFaces(1);

	MultiMtl *mtl = NewDefaultMultiMtl();
	mtl->SetNumSubMtls(nTriangleGroups);

	gi->GetMaterialLibrary().Add(mtl);
	node->GetINode()->SetMtl(mtl);

	for (int matId = 0; matId < nTriangleGroups; matId++) {
		StdMat2 *submtl = NewDefaultStdMat();
		submtl->SetName(renderSet->materials.at(matId).c_str());
		mtl->SetSubMtlAndName(matId, submtl, submtl->GetName());

		int begP = pg[matId].startIndex / 3;
		int endP = begP + pg[matId].nPrimitives;
		for (int i = begP; i < endP; ++i) {
			int a = data[i * 3 + 0];
			int b = data[i * 3 + 1];
			int c = data[i * 3 + 2];

			Face& f = mesh.faces[i];
			f.setVerts(c, b, a);
			f.Show();
			f.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);
			f.setMatID(matId);
			
			tvFace[i].setTVerts(c, b, a);
		}
	}
}


static void load_uv2(const char*, Mesh&);

int BWModel::pre_load_uv2(BWPrimitives& prim, const std::string& uv2_name)
{
	std::vector<char> streamBuf;
	if (prim.openSection(uv2_name, streamBuf)) {
		return 1;
	}

	Mesh& mesh = *meshPtr;

	char *dataPtr = streamBuf.data();
	std::string streamType(dataPtr, strnlen(dataPtr, 64));
	dataPtr += 64;

	bool flgNewFormat = boost::starts_with(streamType, "BPVS");
	if (flgNewFormat) {
		streamType.erase(0, 4);
		dataPtr += 4;
		dataPtr += 64;
	}

	if (streamType == "uv2") {
		uint32_t uv2_vcount = *reinterpret_cast<uint32_t*>(dataPtr);
		dataPtr += 4;

		assert(uv2_vcount == mesh.getNumVerts());

		load_uv2(dataPtr, mesh);
	}
	else {
		assert(false);
		return 2;
	}

	return 0;
}


static void load_uv2(const char* buf, Mesh& mesh)
{
	const Point2 *data = reinterpret_cast<const Point2*>(buf);

	mesh.setMapSupport(2, TRUE);
	mesh.setNumMapVerts(2, mesh.getNumVerts());
	mesh.setNumMapFaces(2, mesh.getNumFaces());
	TVFace *tvFace = mesh.mapFaces(2);

	for (int i = 0; i < mesh.getNumVerts(); ++i) {
		mesh.setMapVert(2, i, UVVert(data[i].x, 1.0f - data[i].y, 0.0f));
	}

	for (int i = 0; i < mesh.getNumFaces(); ++i) {
		tvFace[i].t[0] = mesh.faces[i].v[0];
		tvFace[i].t[1] = mesh.faces[i].v[1];
		tvFace[i].t[2] = mesh.faces[i].v[2];
	}
}


static void load_colour(const char*, Mesh&);

int BWModel::pre_load_colour(BWPrimitives& prim, const std::string& colour_name)
{
	std::vector<char> streamBuf;
	if (prim.openSection(colour_name, streamBuf)) {
		return 1;
	}

	Mesh& mesh = *meshPtr;

	char *dataPtr = streamBuf.data();
	std::string streamType(dataPtr, strnlen(dataPtr, 64));
	dataPtr += 64;

	bool flgNewFormat = boost::starts_with(streamType, "BPVS");
	if (flgNewFormat) {
		streamType.erase(0, 4);
		dataPtr += 4;
		dataPtr += 64;
	}

	if (streamType == "colour") {
		uint32_t colour_vcount = *reinterpret_cast<uint32_t*>(dataPtr);
		dataPtr += 4;

		assert(colour_vcount == mesh.getNumVerts());

		load_colour(dataPtr, mesh);
	}
	else {
		assert(false);
		return 2;
	}

	return 0;
}



#pragma pack(push, 1)
class Colour {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
public:
	VertColor get() const {
		return VertColor(r / 255.f, g / 255.f, b / 255.f);
	}
};
#pragma pack(pop)

static void load_colour(const char* buf, Mesh& mesh)
{
	const Colour *colours = reinterpret_cast<const Colour*>(buf);

	mesh.setNumVertCol(mesh.getNumVerts());
	mesh.setNumVCFaces(mesh.getNumFaces());

	for (int i = 0; i < mesh.getNumVerts(); ++i) {
		mesh.vertCol[i] = colours[i].get();
	}

	for (int i = 0; i < mesh.getNumFaces(); ++i) {
		mesh.vcFace[i].t[0] = mesh.faces[i].v[0];
		mesh.vcFace[i].t[1] = mesh.faces[i].v[1];
		mesh.vcFace[i].t[2] = mesh.faces[i].v[2];
	}
}
