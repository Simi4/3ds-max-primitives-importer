#pragma once
#include "bw_common.h"
#include "Max.h"

class BWVisual;

class BWModel {
public:
	BWModel();
	~BWModel();

	int load(std::string primitives_path, ImpInterface *i);

private:
	std::unique_ptr<BWVisual> visual;
};
