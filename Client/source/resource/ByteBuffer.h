#pragma once

#include <memory>
#include <string>

#include <SFML/Main.hpp>

#define BYTEBUFFER_DEF_SIZE 32

namespace Resource
{
	class ByteBuffer
	{
	public:
		ByteBuffer(
			const sf::Uint32 size)
		{
			buffer = new char[size];
			memset(buffer, 0, size);

			realLength = size;
			virtualLength = 0;
			remaining = size;
		}

		void reset(
			const sf::Uint32 size = BYTEBUFFER_DEF_SIZE)
		{
			if (realLength != size)
			{
				delete buffer;
				buffer = new char[size];
			}

			memset(buffer, 0, size);

			realLength = size;
			virtualLength = 0;
			remaining = size;
		}

		void increaseLength()
		{
			const sf::Uint32 oldLength = realLength;

			while (remaining < 0)
			{
				remaining += realLength;
				realLength *= 2;
			}

			char* temp = new char[realLength];
			memcpy(temp, buffer, oldLength);
			memset(buffer, 0, realLength - oldLength);

			delete buffer;
			buffer = temp;
		}

		char* use(
			const sf::Uint32 size)
		{
			const sf::Uint32 temp = virtualLength;
			
			remaining -= size;
			virtualLength += size;

			if (remaining < 0)
			{
				increaseLength();
			}

			return buffer + temp;
		}

		char* write()
		{
			return buffer;
		}

		const char* read() const
		{
			return buffer;
		}

		sf::Uint32 getRealLength() const
		{
			return realLength;
		}

		sf::Uint32 getVirtualLength() const
		{
			return virtualLength;
		}

	private:
		sf::Uint32
			realLength,
			virtualLength = 0,
			remaining;

		char* buffer = NULL;
	};

	class ByteWriter
	{
	public:
		ByteWriter(
			const sf::Uint32 length)
			:
			buffer(length)
		{
		}

		ByteWriter()
			:
			ByteWriter(BYTEBUFFER_DEF_SIZE)
		{
		}

		void write(
			const char* mem,
			const sf::Uint32 length)
		{
			memcpy(
				buffer.use(length),
				mem,
				length);
		}

		void write(
			const wchar_t* str,
			const sf::Uint32 length)
		{
			writeValue(
				&length);
			write(
				(char*) str,
				length * sizeof(wchar_t)
			);
		}

		void write(
			const ByteBuffer* buffer)
		{
			write(buffer->read(), buffer->getVirtualLength());
		}

		void write(
			const ByteWriter* bb)
		{
			write(bb->readBuffer());
		}

		void write(
			const std::wstring* str)
		{
			write(str->c_str(), str->length());
		}

		void write(
			const std::wstring str)
		{
			write(&str);
		}

		template <typename T>
		void writeValue(
			const T* value)
		{
			write(
				(char*) value, sizeof(T));
		}

		ByteBuffer* writeBuffer()
		{
			return &buffer;
		}

		const ByteBuffer* readBuffer() const
		{
			return &buffer;
		}

		const sf::Uint32 getLength() const
		{
			return buffer.getVirtualLength();
		}

	private:
		ByteBuffer buffer;
	};

	class ByteReader
	{
	public:
		ByteReader(
			const ByteBuffer* buffer)
			:
			buffer(buffer)
		{
		}

		template <typename T>
		bool readValue(T* value)
		{
			if (!isValidSize( sizeof(T) ))
			{
				return false;
			}

			memcpy(
				value,
				buffer->read() + position,
				sizeof(T));
			position += sizeof(T);

			return true;
		}

		bool readString(
			wchar_t* strBuffer,
			const sf::Uint32 bufferSize)
		{
			const sf::Uint32 strSize = getStringSize();

			if (!strSize || strSize > bufferSize)
			{
				return false;
			}

			if (
				!isValidPosition(strSize * sizeof(wchar_t))
				)
			{
				return false;
			}

			memcpy(
				(char*) strBuffer,
				buffer->read() + sizeof(sf::Uint32),
				strSize * sizeof(wchar_t)
			);
			position += strSize * sizeof(wchar_t);

			return true;
		}

		bool readString(
			std::wstring* str)
		{
			const sf::Uint32 strSize = getStringSize();

			if (!strSize)
			{
				return false;
			}

			if (
				!isValidPosition(strSize * sizeof(wchar_t))
				)
			{
				return false;
			}

			str->resize(strSize + 1);
			str->assign(
				(wchar_t*) buffer->read() + sizeof(sf::Uint32),
				strSize);

			return true;
		}

		sf::Uint32 getStringSize()
		{
			if (
				!isValidPosition( sizeof(sf::Uint32) )
				)
			{
				return NULL;
			}

			return *(int*) buffer->read();
		}

		bool endOfBuffer() const
		{
			return position == buffer->getVirtualLength();
		}

		const ByteBuffer* getBuffer() const
		{
			return buffer;
		}

		const sf::Int32 getLength()
		{
			return buffer->getVirtualLength();
		}

	private:
		bool isValidPosition(
			const sf::Uint32 size)
		{
			return buffer->getVirtualLength() - position >= size;
		}

		sf::Uint32 position = 0;

		const ByteBuffer* buffer;
	};
}
