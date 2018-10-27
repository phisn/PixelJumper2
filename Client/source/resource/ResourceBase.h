#pragma once

#include <Client/source/resource/ByteBuffer.h>

#ifndef RESOURCE
#define RESOURCE ::Resource
#endif

namespace Resource
{
	struct Base
	{
	public:
		virtual bool writeToBuffer(
			ByteWriter* buffer) = 0;
		virtual bool readFromBuffer(
			ByteReader* buffer) = 0;
	};
}