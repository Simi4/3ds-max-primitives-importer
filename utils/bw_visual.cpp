#include "bw_visual.h"
#include "bwxml-lib/BWReader.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
namespace pt = boost::property_tree;


BWVisual::BWVisual()
{
}


BWVisual::~BWVisual()
{
}


static Point3 Point3FromStr(const std::string& str)
{
	std::istringstream sstr(str);
	float x, y, z;
	sstr >> x >> y >> z;
	assert(!sstr.fail());
	return Point3(x, y, z);
}

static void parse_nodes(const pt::ptree& tree, VisualNodePtr& parent)
{
	if (!tree.count("identifier") || !tree.count("transform")) {
		assert(false);
		return;
	}

	auto newNode = std::make_shared<VisualNode>();

	auto identifier = tree.get<std::string>("identifier");
	auto row0 = tree.get<std::string>("transform.row0");
	auto row1 = tree.get<std::string>("transform.row1");
	auto row2 = tree.get<std::string>("transform.row2");
	auto row3 = tree.get<std::string>("transform.row3");

	newNode->transform.SetRow(0, Point3FromStr(row0));
	newNode->transform.SetRow(1, Point3FromStr(row1));
	newNode->transform.SetRow(2, Point3FromStr(row2));
	newNode->transform.SetRow(3, Point3FromStr(row3));

	if (!parent) {
		// for Scene Root node
		parent = newNode;
	}
	else {
		parent->nodes[identifier] = newNode;
	}

	for (const auto& child : tree) {
		if (child.first == "node") {
			parse_nodes(child.second, newNode);
		}
	}
}


int BWVisual::load(const std::string& visual_path)
{
	pt::ptree tree;

	try {
		BWPack::BWXMLReader xmlReader(visual_path);
		tree = move(xmlReader.mTree);
	}
	catch (...) {
		try {
			pt::read_xml(visual_path, tree, pt::xml_parser::trim_whitespace | pt::xml_parser::no_comments);
		}
		catch (...) {
			return 1;
		}
	}

	// get root node
	auto root = tree.front().second;

	parse_nodes(root.get_child("node"), mSceneRoot);

	for (const auto &v : root) {
		if (v.first != "renderSet") {
			continue;
		}

		auto renderSet = std::make_shared<RenderSet>();

		renderSet->treat_as_world_space_object = v.second.get<bool>(
			"treatAsWorldSpaceObject", "true");

		if (renderSet->treat_as_world_space_object == true) {
			for (const auto &[tag, value] : v.second) {
				if (tag != "node") {
					continue;
				}
				renderSet->node_names.push_back(
					value.get_value<std::string>());
			}
		}

		auto geometry = v.second.get_child("geometry");
		renderSet->vertices_name = geometry.get<std::string>("vertices", "vertices");
		renderSet->primitive_name = geometry.get<std::string>("primitive", "indices");

		for (const auto &[tag, value] : geometry) {
			if (tag == "stream") {
				renderSet->stream_names.push_back(value.get_value<std::string>());
			}
			else if (tag == "primitiveGroup") {
				int pgId = value.get_value<int>();
				auto matName = value.get<std::string>("material.identifier", "unnamed_material");
				renderSet->materials[pgId] = matName;
			}
		}

		mRenderSets.push_back(renderSet);
	}

	return 0;
}
