#include "bw_vertextypes.h"


Point3 packed_vec::unpack() const {
	float unpx, unpy, unpz;
	unpx = x / 127.f;
	unpy = y / 127.f;
	unpz = z / 127.f;
	return Point3(unpx, unpz, unpy).Normalize();
}


Point3 packed_vec_old::unpack() const {
	float unpx, unpy, unpz;
	unpx = x / 1023.f;
	unpy = y / 1023.f;
	unpz = z / 511.f;
	return Point3(unpx, unpz, unpy).Normalize();
}
