#pragma once
#include "bw_common.h"
#include "Max.h"


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

	int pre_load_stream(BWPrimitives&, const std::string&);

	int pre_load_indices(BWPrimitives&, const std::string&);
	template <typename T> void load_indices(const char*, uint32_t, uint32_t);

public:
	BWModel(ImpInterface *i, Interface *gi);
	~BWModel();

	int load(std::string primitives_path);
};
