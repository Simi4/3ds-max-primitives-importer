#pragma once
#include "bw_common.h"
#include <Max.h>
#include <Path.h>


class RenderSet;
typedef std::shared_ptr<RenderSet> RenderSetPtr;

class BWPrimitives;
class BWVisual;

class BWModel {
	ImpInterface *i;
	Interface *gi;

	std::unique_ptr<BWVisual> visual;
	Mesh *meshPtr;
	ImpNode *node;

	int pre_load_vertices(BWPrimitives&, const std::string&);
	template <class T> void load_vertices(const char*);

	int pre_load_uv2(BWPrimitives&, const std::string&);
	int pre_load_colour(BWPrimitives&, const std::string&);

	int pre_load_indices(RenderSetPtr, BWPrimitives&);
	template <typename T> void load_indices(RenderSetPtr, const char*, uint32_t, uint32_t);

public:
	BWModel(ImpInterface *i, Interface *gi);
	~BWModel();

	int load(MaxSDK::Util::Path max_primitives_path);
};
