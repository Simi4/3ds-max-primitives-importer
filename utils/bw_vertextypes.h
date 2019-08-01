#pragma once
#include <string>
#include <cstdint>
#include <max.h>


#pragma pack(push, 1)

class packed_vec {
	int32_t x : 8;
	int32_t y : 8;
	int32_t z : 8;
public:
	Point3 unpack() const;
};
static_assert(sizeof(packed_vec) == 4);


class packed_vec_old {
	int32_t x : 11;
	int32_t y : 11;
	int32_t z : 10;
public:
	Point3 unpack() const;
};
static_assert(sizeof(packed_vec_old) == 4);


class set3_xyznuviiiwwtbpc_s {
	Point3     pos;
	packed_vec n;
	Point2     uv;
    uint8_t    iiiww[8];
	packed_vec tb[2];

public:
	Point3 position() const {
		return Point3(pos.x, -pos.z, pos.y);
	}

	Point3 normal() const {
		return n.unpack();
	}

	UVVert uvw() const {
		return UVVert(uv.x, 1.0f - uv.y, 0.0f);
	}

    static bool check_vertex_type(const std::string& vertex_type) {
		return vertex_type == "set3/xyznuviiiwwtbpc";
    }
};
static_assert(sizeof(set3_xyznuviiiwwtbpc_s) == 40);


class set3_xyznuvtbpc_s {
	Point3     pos;
	packed_vec n;
	Point2     uv;
	packed_vec tb[2];

public:
	Point3 position() const {
		return Point3(pos.x, pos.z, pos.y);
	}

	Point3 normal() const {
		return n.unpack();
	}

	UVVert uvw() const {
		return UVVert(uv.x, 1.0f - uv.y, 0.0f);
	}

	static bool check_vertex_type(const std::string& vertex_type) {
		return vertex_type == "set3/xyznuvtbpc";
	}
};
static_assert(sizeof(set3_xyznuvtbpc_s) == 32);


class set3_xyznuvpc_s {
	Point3     pos;
	packed_vec n;
	Point2     uv;

public:
	Point3 position() const {
		return Point3(pos.x, pos.z, pos.y);
	}

	Point3 normal() const {
		return n.unpack();
	}

	UVVert uvw() const {
		return UVVert(uv.x, 1.0f - uv.y, 0.0f);
	}

	static bool check_vertex_type(const std::string& vertex_type) {
		return vertex_type == "set3/xyznuvpc";
	}
};
static_assert(sizeof(set3_xyznuvpc_s) == 24);


class xyznuviiiwwtb_s {
	Point3         pos;
	packed_vec_old n;
	Point2         uv;
	uint8_t        iiiww[5];
	packed_vec_old tb[2];

public:
	Point3 position() const {
		return Point3(pos.x, -pos.z, pos.y);
	}

	Point3 normal() const {
		return n.unpack();
	}

	UVVert uvw() const {
		return UVVert(uv.x, 1.0f - uv.y, 0.0f);
	}

	static bool check_vertex_type(const std::string& vertex_type) {
		return vertex_type == "xyznuviiiwwtb";
	}
};
static_assert(sizeof(xyznuviiiwwtb_s) == 37);


class xyznuvtb_s {
	Point3         pos;
	packed_vec_old n;
	Point2         uv;
	packed_vec_old tb[2];

public:
	Point3 position() const {
		return Point3(pos.x, pos.z, pos.y);
	}

	Point3 normal() const {
		return n.unpack();
	}

	UVVert uvw() const {
		return UVVert(uv.x, 1.0f - uv.y, 0.0f);
	}

	static bool check_vertex_type(const std::string& vertex_type) {
		return vertex_type == "xyznuvtb";
	}
};
static_assert(sizeof(xyznuvtb_s) == 32);


class xyznuv_s {
	Point3 pos;
	Point3 n;
	Point2 uv;

public:
	Point3 position() const {
		return Point3(pos.x, pos.z, pos.y);
	}

	Point3 normal() const {
		return n;
	}

	UVVert uvw() const {
		return UVVert(uv.x, 1.0f - uv.y, 0.0f);
	}

	static bool check_vertex_type(const std::string& vertex_type) {
		return vertex_type == "xyznuv";
	}
};
static_assert(sizeof(xyznuv_s) == 32);

#pragma pack(pop)
