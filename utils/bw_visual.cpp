#include "bw_visual.h"
#include "bwxml-lib/BWReader.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>


BWVisual::BWVisual()
{
}


BWVisual::~BWVisual()
{
}


static void parse_nodes(boost::property_tree::ptree& tree, NodePtr& parent)
{
	if (!tree.count("identifier") || !tree.count("transform")) {
		return;
	}

	NodePtr newNode = std::make_shared<Node>();

	newNode->identifier = tree.get<std::string>("identifier");
	auto row0 = tree.get<std::string>("transform.row0");
	auto row1 = tree.get<std::string>("transform.row1");
	auto row2 = tree.get<std::string>("transform.row2");
	auto row3 = tree.get<std::string>("transform.row3");

	std::vector<std::string> splitted;
	Point3 p3;

	// todo: need to refactor
	boost::split(splitted, row0, boost::is_any_of("\t "));
	p3.Set(std::stod(splitted[0]), std::stod(splitted[1]), std::stod(splitted[2]));
	newNode->transform.SetRow(0, p3);

	boost::split(splitted, row1, boost::is_any_of("\t "));
	p3.Set(std::stod(splitted[0]), std::stod(splitted[1]), std::stod(splitted[2]));
	newNode->transform.SetRow(1, p3);

	boost::split(splitted, row2, boost::is_any_of("\t "));
	p3.Set(std::stod(splitted[0]), std::stod(splitted[1]), std::stod(splitted[2]));
	newNode->transform.SetRow(2, p3);

	boost::split(splitted, row3, boost::is_any_of("\t "));
	p3.Set(std::stod(splitted[0]), std::stod(splitted[1]), std::stod(splitted[2]));
	newNode->transform.SetRow(3, p3);

	if (!parent) {
		// for Scene Root node
		parent = newNode;
	}
	else {
		parent->nodes[newNode->identifier] = newNode;
	}

	for (auto& child : tree) {
		if (child.first == "node") {
			parse_nodes(child.second, newNode);
		}
	}
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

	// get root node
	auto root = mTree.front().second;

	for (auto &v : root) {
		if (v.first == "node") {
			parse_nodes(v.second, mSceneRoot);
		}
		else if (v.first == "renderSet") {
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

	return 0;
}
