#pragma once
#include "bw_common.h"
#include <boost/property_tree/ptree.hpp>


struct RenderSet {
	std::string vres_name;
	std::string pres_name;
	std::string uv2_name;
};


class BWVisual {
public:
	BWVisual();
	~BWVisual();
	int load(const std::string& visual_path);
	std::vector<RenderSet>& renderSets();

private:
	boost::property_tree::ptree mTree;
	std::vector<RenderSet> mRenderSets;
};
