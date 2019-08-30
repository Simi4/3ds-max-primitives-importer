#pragma once
#include "bw_common.h"
#include <matrix3.h>


class RenderSet;
typedef std::shared_ptr<RenderSet> RenderSetPtr;

class RenderSet {
public:
	bool treat_as_world_space_object;
	std::vector<std::string> node_names;

	std::string vertices_name;
	std::string primitive_name;
	std::vector<std::string> stream_names;
	std::map<int, std::string> materials;
};


class VisualNode;
typedef std::shared_ptr<VisualNode> VisualNodePtr;

class VisualNode {
public:
	Matrix3 transform;
	std::map<std::string, VisualNodePtr> nodes;
};


class BWVisual {
	std::vector<RenderSetPtr> mRenderSets;
	VisualNodePtr mSceneRoot;

public:
	BWVisual();
	~BWVisual();

	int load(const std::string& visual_path);
	const std::vector<RenderSetPtr>& renderSets() const { return mRenderSets; }
	const VisualNodePtr sceneRoot() const { return mSceneRoot; }
};
