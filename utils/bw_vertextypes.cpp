#include "bw_vertextypes.h"


Point3 packed_vec::unpack() const {
	float unpx, unpy, unpz;
	unpx = x / 127.f;
	unpy = y / 127.f;
	unpz = z / 127.f;
	return Point3(unpx, unpy, unpz); // TODO
}


Point3 packed_vec_old::unpack() const {
	float unpx, unpy, unpz;
	unpx = x / 511.f;
	unpy = y / 511.f;
	unpz = z / 255.f;
	return Point3(unpx, unpy, unpz); // TODO
}
