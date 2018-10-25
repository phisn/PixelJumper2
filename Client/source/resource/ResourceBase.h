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
		virtual int appendToBuffer(
			ByteBuffer* buffer) = 0;

		virtual int loadFromBuffer(
			char* buffer,
			const int length) = 0;
		virtual int stripFromBuffer(
			ByteBuffer* buffer,
			const int position)
		{
			if (buffer->getLength() - position <= 0)
			{
				return 0;
			}

			return loadFromBuffer(
				buffer->begin() + position,
				buffer->getLength() - position);
		}
	};
}