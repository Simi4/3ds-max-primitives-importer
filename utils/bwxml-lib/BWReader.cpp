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

#include "BWReader.h"
#include "Base64.h"
#include "BWCommon.h"

#include <sstream>
#include <iomanip>


namespace BWPack
{
	using boost::property_tree::ptree;
	using namespace BigWorld;

	BWXMLReader::BWXMLReader(const std::string& fname) : mStream(fname)
	{
		uint32_t magic = mStream.get<uint32_t>();
		if (magic != PACKED_SECTION_MAGIC)
			throw std::runtime_error("Wrong header magic");

		uint8_t version = mStream.get<uint8_t>();
		if (version != 0)
			throw std::runtime_error("Unsupported file version");
		ReadStringTable();

		mTree.put_child("root", ReadSection());
	}

	void BWXMLReader::ReadStringTable()
	{
		for (std::string tmp = mStream.getNullTerminatedString(); 
			!tmp.empty(); 
			tmp = mStream.getNullTerminatedString())
			mStrings.push_back(tmp);
		//std::cout << "Collected " << mStrings.size() << " strings." << std::endl;
	}

	void BWXMLReader::readData( BigWorld::DataDescriptor descr, boost::property_tree::ptree& current_node, uint32_t prev_offset )
	{
		current_node.clear();
		uint32_t startPos = prev_offset, endPos = descr.offset();
		uint32_t var_size = endPos - startPos;
		assert(var_size >= 0);

		std::stringstream contentBuffer;
		switch(descr.typeId())
		{
		case BW_Section:
			current_node = ReadSection(); //yay recursion!
			break;

		case BW_String:
			contentBuffer << mStream.getString(var_size);
			current_node.put_value(contentBuffer.str());
			break;

		case BW_Int:
			switch (var_size)
			{
			case 8:
				current_node.put_value(mStream.get<int64_t>());
				break;
			case 4:
				current_node.put_value(mStream.get<int32_t>());
				break;
			case 2:
				current_node.put_value(mStream.get<int16_t>());
				break;
			case 1:
				current_node.put_value(mStream.get<int8_t>());
				break;
			case 0:
				current_node.put_value(0);
				break;
			default:
				throw std::runtime_error("Unsupported int size!");
			}
			break;

		case BW_Float:
			assert(var_size % sizeof(float) == 0);
			contentBuffer << std::fixed << std::setfill('\t');
			if (var_size / sizeof(float) == BW_MATRIX_SIZE) // we've got a matrix!
			{
				for (size_t i=0; i<BW_MATRIX_NROWS; ++i)
				{
					for (size_t j=0; j<BW_MATRIX_NCOLS; ++j)
					{
						if (!contentBuffer.str().empty())
							contentBuffer << " ";
						contentBuffer << mStream.get<float>();
					}
					current_node.put("row" + std::to_string(i), contentBuffer.str());
					contentBuffer.str(""); // clearing our buffer
				}
				break;
			}
			// not a matrix, building a plain string
			for (size_t i=0; i<(var_size / sizeof(float)); ++i)
			{
				if (!contentBuffer.str().empty())
					contentBuffer << " ";
				contentBuffer << mStream.get<float>();
			}
			current_node.put_value(contentBuffer.str());
			break;

		case BW_Bool:
			current_node.put_value((var_size != 0));
			mStream.getString(var_size);
			break;

		case BW_Blob:
			current_node.put_value(B64::Encode(mStream.getString(var_size)));
			break;

		case BW_Enc_blob:
			mStream.getString(var_size); // TBD?
			current_node.put_value("TYPE_ENCRYPTED_BLOB is (yet) unsupported!");
			std::cerr <<"unsupported section TYPE_ENCRYPTED_BLOB!" << std::endl;
			break;

		default:
			throw std::runtime_error("Unsupported section type!");
		}
	}

	ptree BWXMLReader::ReadSection()
	{
		ptree current_node;
		int nChildren = mStream.get<uint16_t>();

		DataDescriptor ownData = mStream.get<DataDescriptor>();

		std::vector<DataNode> children;
		children.reserve(nChildren);
		for (int i=0; i<nChildren; ++i)
		{
			children.push_back(mStream.get<DataNode>());
		}

		readData(ownData, current_node, 0);

		int prev_offset = ownData.offset();
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			assert(it->nameIdx < mStrings.size());
			//keys may contain dots, they confuse the ptree
			auto path = ptree::path_type(mStrings[it->nameIdx], '\0'); // so we make a custom path
			readData(it->data, current_node.add(path, ""), prev_offset); 
			prev_offset = it->data.offset();
		}
		return current_node;
	}
}
