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
			boost::property_tree::read_xml(visual_path, mTree);
		}
		catch (...) {
			return 1;
		}
	}

	return 0;
}

std::vector<RenderSet>& BWVisual::renderSets()
{
	// get root node
	auto root = mTree.front().second;

	for (auto &v : root) {
		if (v.first == "renderSet") {
			RenderSet renderSet;

			renderSet.vres_name = v.second.get<std::string>("geometry.vertices", "vertices");
			renderSet.pres_name = v.second.get<std::string>("geometry.primitive", "indices");

			// get uv2 sream name if exist
			auto g_stream = v.second.get_optional<std::string>("geometry.stream");
			if (g_stream && g_stream->find("uv2") != std::string::npos) {
				renderSet.uv2_name = g_stream.value();
			}

			boost::trim(renderSet.vres_name);
			boost::trim(renderSet.pres_name);
			boost::trim(renderSet.uv2_name);

			mRenderSets.push_back(renderSet);
		}
	}

	return mRenderSets;
}
