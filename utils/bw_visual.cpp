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

BWVisual::RenderSets& BWVisual::renderSets()
{
	// get root node
	auto root = mTree.front().second;

	for (auto &v : root) {
		if (v.first == "renderSet") {
			auto renderSet = std::make_unique<RenderSet>();

			renderSet->vertices_name = v.second.get<std::string>("geometry.vertices", "vertices");
			renderSet->primitive_name = v.second.get<std::string>("geometry.primitive", "indices");

			auto geometry_stream = v.second.get_optional<std::string>("geometry.stream");
			if (geometry_stream) {
				renderSet->stream_name = geometry_stream.value();
			}

			mRenderSets.push_back(std::move(renderSet));
		}
	}

	return mRenderSets;
}
