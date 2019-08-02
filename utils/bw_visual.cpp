#include "bw_visual.h"
#include "bwxml-lib/BWReader.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>


BWVisual::BWVisual()
{
}


BWVisual::~BWVisual()
{
}


int BWVisual::load(const std::string& visual_path)
{
	try {
		BWPack::BWXMLReader xmlReader(visual_path);
		mTree = std::move(xmlReader.mTree);
	}
	catch (...) {
		try {
			using namespace boost::property_tree::xml_parser;
			read_xml(visual_path, mTree, trim_whitespace | no_comments);
		}
		catch (...) {
			return 1;
		}
	}

	return 0;
}

std::vector<RenderSetPtr>& BWVisual::renderSets()
{
	// get root node
	auto root = mTree.front().second;

	for (auto &v : root) {
		if (v.first == "renderSet") {
			auto renderSet = std::make_shared<RenderSet>();

			auto geometry = v.second.get_child("geometry");
			renderSet->vertices_name = geometry.get<std::string>("vertices", "vertices");
			renderSet->primitive_name = geometry.get<std::string>("primitive", "indices");
			renderSet->stream_name = geometry.get<std::string>("stream", "");

			for (auto &pg : geometry) {
				if (pg.first == "primitiveGroup") {
					int pgId = pg.second.get_value<int>();
					std::string matName = pg.second.get<std::string>("material.identifier", "unnamed_material");
					renderSet->materials[pgId] = matName;
				}
			}

			mRenderSets.push_back(renderSet);
		}
	}

	return mRenderSets;
}
