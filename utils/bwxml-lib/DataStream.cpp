/*
Copyright 2013-2014 hedger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "DataStream.h"

#include <vector>
#include <iterator>
#include <stdexcept>

namespace BWPack
{
	namespace IO
	{
		StreamReader::StreamReader(const std::string& fname)
		{
			mInput.open(fname, std::ios::binary);
			if (!mInput.is_open())
				throw std::runtime_error("File not found");
		}

		StreamReader::~StreamReader()
		{
			mInput.close();
		}

		std::string StreamReader::getString(size_t len)
		{
			std::string ret;
			if (len) {
				ret.resize(len);
				mInput.read(ret.data(), len);
			}
			return ret;
		}

		std::string StreamReader::getNullTerminatedString()
		{
			std::string ret;
			std::getline(mInput, ret, '\0');
			return ret;
		}
	}
}
