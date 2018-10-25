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
		_Success_(return > 0)
		virtual int appendToBuffer(
			ByteWriter* buffer) = 0;

		_Success_(return > 0)
		virtual int loadFromBuffer(
			char* buffer,
			const int length) = 0;
		_Success_(return > 0)
		virtual int stripFromBuffer(
			ByteWriter* buffer,
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