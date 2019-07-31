#include "bw_primitives.h"


BWPrimitives::BWPrimitives(const std::string& primitives_path)
{
	in.open(primitives_path, std::ios_base::binary);

	uint32_t magic = get<uint32_t>();
	
	if (magic != 0x42a14e65)
		throw std::runtime_error("Wrong magic!");
}

int BWPrimitives::openSection(const std::string& name, std::vector<char>& buf)
{
	in.seekg(0, std::ios_base::end);
	size_t length = in.tellg();

	in.seekg(length - 4, std::ios_base::beg);

	uint32_t indexLen = get<uint32_t>();
	in.seekg(length - indexLen - 4, std::ios_base::beg);

	uint32_t oldDataLen = 4;

	while (in.tellg() < length - 4) {
		uint32_t entryDataLen = get<uint32_t>();

		// skip
		in.seekg(16, std::ios_base::cur);

		uint32_t entryNameLen = get<uint32_t>();

		auto entryStr = getString(entryNameLen);

		if (entryNameLen % 4 > 0)
			in.seekg(4 - (entryNameLen % 4), std::ios_base::cur);

		if (entryStr == name) {
			buf.resize(entryDataLen);
			in.seekg(oldDataLen, std::ios_base::beg);
			in.read(buf.data(), entryDataLen);
			return 0;
		}

		oldDataLen += (entryDataLen + 3) & (~3L);
	}

	return 1;
}

std::string BWPrimitives::getString(size_t len)
{
	std::string ret;
	if (len) {
		ret.resize(len);
		in.read(ret.data(), len);
	}
	return ret;
}
