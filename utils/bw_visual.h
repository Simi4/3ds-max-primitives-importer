#pragma once
#include "bw_common.h"
#include <boost/property_tree/ptree.hpp>


struct RenderSet {
	std::string vertices_name;
	std::string primitive_name;
	std::string stream_name;
};


class BWVisual {
	typedef std::vector<std::unique_ptr<RenderSet>> RenderSets;

	boost::property_tree::ptree mTree;
	RenderSets mRenderSets;

public:
	BWVisual();
	~BWVisual();
	int load(const std::string& visual_path);
	RenderSets& renderSets();
};
