#pragma once
#include "bw_common.h"
#include <fstream>
#include <Path.h>


class BWPrimitives {
public:
	BWPrimitives(const std::string& primitives_path);
	int openSection(const std::string& name, std::vector<char>& buf);

private:
	std::ifstream in;

	std::string getString(size_t len);
	template<typename T>
	inline T get() {
		T buf;
		in.read(reinterpret_cast<char*>(&buf), sizeof(buf));
		return buf;
	}
};
