#pragma once

#include <memory>
#include <string>

#include <SFML/Main.hpp>

#define BYTEBUFFER_DEF_SIZE 32

namespace Resource
{
	class ByteWriter
	{
	public:
		ByteWriter(
			const sf::Uint32 length)
			:
			realLength(length),
			remaining(length)
		{
			buffer = new char[length];
		}

		ByteWriter()
			:
			ByteWriter(BYTEBUFFER_DEF_SIZE)
		{
		}

		~ByteWriter()
		{
			delete[] buffer;
		}

		void append(
			char* mem,
			const sf::Uint32 length)
		{
			remaining -= length;

			incLength();

			memcpy(
				buffer + virtualLength,
				mem,
				length);
			virtualLength += length;
		}

		void append(
			ByteWriter* bb)
		{
			append(bb->begin(), bb->getLength());
		}

		void append(
			std::wstring str)
		{
			append(
				(char*) str.c_str(), 
				str.length() 
				* sizeof(wchar_t));
		}

		template <typename T>
		void appendValue(
			const T* value)
		{
			appendRaw((char*)value, sizeof(T));
		}

		char* begin()
		{
			return buffer;
		}

		const sf::Uint32 getLength() const
		{
			return virtualLength;
		}

	private:
		void incLength()
		{
			const sf::Uint32 oldLength = realLength;

			while (remaining < 0)
			{
				remaining += realLength;
				realLength *= 2;
			}

			char* temp = new char[realLength];
			memcpy(temp, buffer, oldLength);
			
			delete buffer;
			buffer = temp;
		}

		sf::Uint32 
			realLength,
			virtualLength = 0,
			remaining;

		char* buffer = NULL;
	};

	class ByteReader
	{

	};
}
