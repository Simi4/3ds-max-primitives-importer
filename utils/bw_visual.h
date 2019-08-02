#pragma once
#include "bw_common.h"
#include <boost/property_tree/ptree.hpp>
#include <map>


struct RenderSet;
typedef std::shared_ptr<RenderSet> RenderSetPtr;

struct RenderSet {
	std::string vertices_name;
	std::string primitive_name;
	std::string stream_name;
	std::map<int, std::string> materials;
};


class BWVisual {
	boost::property_tree::ptree mTree;
	std::vector<RenderSetPtr> mRenderSets;

public:
	BWVisual();
	~BWVisual();
	int load(const std::string& visual_path);
	std::vector<RenderSetPtr>& renderSets();
};
