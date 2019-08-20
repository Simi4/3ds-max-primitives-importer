#pragma once
#include "bw_common.h"
#include <boost/property_tree/ptree.hpp>
#include <matrix3.h>
#include <map>


class RenderSet;
typedef std::shared_ptr<RenderSet> RenderSetPtr;

class RenderSet {
public:
	std::string vertices_name;
	std::string primitive_name;
	std::string stream_name;
	std::map<int, std::string> materials;
};


class Node;
typedef std::shared_ptr<Node> NodePtr;

class Node {
public:
	std::string identifier;
	Matrix3 transform;
	std::map<std::string, NodePtr> nodes;
};


class BWVisual {
	boost::property_tree::ptree mTree;
	std::vector<RenderSetPtr> mRenderSets;
	NodePtr mSceneRoot;

public:
	BWVisual();
	~BWVisual();

	int load(const std::string& visual_path);
	const std::vector<RenderSetPtr>& renderSets() const { return mRenderSets; }
	const NodePtr sceneRoot() const { return mSceneRoot; }
};
