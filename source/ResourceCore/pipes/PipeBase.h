#pragma once

#include <SFML/Main.hpp>
#include <sfml/Network.hpp>

#include <string>

#ifdef _WIN32 // hton | ntoh
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

namespace Resource
{
	struct PipeBase
	{
		// the truth is this shit is actually not needed
		// but because the whole project works already around
		// this, I wont change it anymore. remarks for future
		// projects: rewrite this core and use isValid only
		// in pipe implementations
		virtual bool isValid() const = 0;
	};

	class WritePipe
		:
		public PipeBase
	{
	public:
		virtual ~WritePipe() = 0 { }

		virtual void reserveSize(const sf::Uint64) = 0;
		virtual void writeContent(
			const char* const buffer,
			const sf::Uint64 size) = 0;
		
		bool writeContentSafe(
			const char* const buffer,
			const sf::Uint64 size)
		{
			if (!isValid())
			{
				return false;
			}

			writeContent(buffer, size);

			return isValid();
		}

		template <typename T>
		bool writeVector(const std::vector<T>* const _vector)
		{
			static_assert(std::is_trivially_move_constructible_v<T> &&
				std::is_trivially_destructible_v<T>, "tried to write vector with non trivial type");

			uint32_t count = _vector->size();
			if (!writeValue(&count))
			{
				return false;
			}

			return writeContentSafe(
				(char*)_vector->data(),
				count * sizeof(T));
		}

		template <typename T>
		bool writeVectorResource(std::vector<T>* _vector)
		{
			uint32_t count = _vector->size();
			if (!writeValue(&count))
			{
				return false;
			}

			for (T& value : *_vector)
				if (!value.save(this))
				{
					return false;
				}

			return true;
		}

		template <typename T>
		bool writeValue(const T* const value)
		{
			return writeContentSafe((const char* const) value, sizeof(T));
		}

		template<> bool writeValue<sf::Uint16>(const sf::Uint16* const value);
		template<> bool writeValue<sf::Int16>(const sf::Int16* const value);

		template<> bool writeValue<sf::Uint32>(const sf::Uint32* const value);
		template<> bool writeValue<sf::Int32>(const sf::Int32* const value);

		template<> bool writeValue<sf::Uint64>(const sf::Uint64* const value);
		template<> bool writeValue<sf::Int64>(const sf::Int64* const value);

		template<> bool writeValue<float>(const float* const value);
		template<> bool writeValue<double>(const double* const value);

		template<> bool writeValue<bool>(const bool* const value)
		{
			const sf::Uint8 byte = *value != 0;
			return writeValue<sf::Uint8>(&byte);
		}

		template <typename String = std::wstring, typename Size = char>
		bool writeString(const String* const str)
		{
			Size size = (Size) str->size();

			if (size < 0)
			{
				return false;
			}

			if (!writeValue(&size))
			{
				return false;
			}

			return writeContentSafe(
				(char * const) str->c_str(), 
				size * sizeof(String::value_type)
			);
		}
	};

	class ReadPipe
		:
		public PipeBase
	{
	public:
		virtual ~ReadPipe() = 0 { }

		// return == truncated
		virtual sf::Int64 readContent(
			char* const buffer,
			const sf::Uint64 size) = 0;
		
		bool readContentForce(
			char* const buffer,
			const sf::Uint64 size)
		{
			return readContent(buffer, size) == size;
		}

		template <typename T>
		bool readVector(std::vector<T>* const vector)
		{
			static_assert(std::is_trivially_move_constructible_v<T> &&
				std::is_trivially_destructible_v<T>, "tried to read vector with non trivial type");

			uint32_t count;
			if (!readValue(&count)) 
			{
				return false;
			}

			vector->resize(count);

			return readContentForce(
				(char*) vector->data(),
				count * sizeof(T));
		}

		template <typename T>
		bool readVectorResource(std::vector<T>* _vector)
		{
			uint32_t count;
			if (!readValue(&count))
			{
				return false;
			}

			_vector->resize(count);

			for (T& value : *_vector)
				if (!value.make(this))
				{
					return false;
				}

			return true;
		}
		
		template <typename T>
		bool readValue(T* const value)
		{
			return readContentForce((char*) value, sizeof(T));
		}

		template<> bool readValue<sf::Uint16>(sf::Uint16* const value);
		template<> bool readValue<sf::Int16>(sf::Int16* const value);

		template<> bool readValue<sf::Uint32>(sf::Uint32* const value);
		template<> bool readValue<sf::Int32>(sf::Int32* const value);

		template<> bool readValue<sf::Uint64>(sf::Uint64* const value);
		template<> bool readValue<sf::Int64>(sf::Int64* const value);

		template<> bool readValue<float>(float* const value);
		template<> bool readValue<double>(double* const value);

		template<> bool readValue<bool>(bool* const value)
		{
			return readValue<sf::Uint8>((sf::Uint8*) value);
		}

		template <typename String = std::wstring, typename Size = unsigned char>
		bool readString(String* const str)
		{
			Size size;

			if ( !readValue(&size) )
			{
				return false;
			}

			if (size < 0)
			{
				return false;
			}

			str->resize(size);

			return readContentForce(
				(char*) str->c_str(), size * sizeof(String::value_type)
			);
		}
	};

	template<>
	inline bool ReadPipe::readValue(sf::Uint16* const value)
	{
		const bool result = readContentForce((char*)value, 2);

		if (result)
		{
			*value = ntohs(*value);
		}

		return result;
	}

	template<>
	inline bool ReadPipe::readValue(sf::Int16* const value)
	{
		const bool result = readContentForce((char*)value, 2);

		if (result)
		{
			*value = ntohs(*value);
		}

		return result;
	}

	template<>
	inline bool ReadPipe::readValue(sf::Uint32* const value)
	{
		const bool result = readContentForce((char*)value, 4);

		if (result)
		{
			*value = ntohl(*value);
		}

		return result;
	}

	template<>
	inline bool ReadPipe::readValue(sf::Int32* const value)
	{
		const bool result = readContentForce((char*)value, 4);

		if (result)
		{
			*value = ntohl(*value);
		}

		return result;
	}

	template<>
	inline bool ReadPipe::readValue(sf::Uint64* const value)
	{
		const bool result = readContentForce((char*)value, 8);

		if (result)
		{
			*value = ntohll(*value);
		}

		return result;
	}

	template<>
	inline bool ReadPipe::readValue(sf::Int64* const value)
	{
		const bool result = readContentForce((char*)value, 8);

		if (result)
		{
			*value = ntohll(*value);
		}

		return result;
	}

	template<>
	inline bool ReadPipe::readValue(float* const value)
	{
		unsigned int temp;
		const bool result = readContentForce((char*) &temp, 4);

		if (result)
		{
			*value = ntohf(temp);
		}

		return result;
	}

	template<>
	inline bool ReadPipe::readValue(double* const value)
	{
		unsigned long long temp;
		const bool result = readContentForce((char*) &temp, 8);

		if (result)
		{
			*value = ntohd(temp);
		}

		return result;
	}

	template<>
	inline bool WritePipe::writeValue(const sf::Uint16* const value)
	{
		const sf::Uint16 netValue = htons(*value);
		return writeContentSafe((const char* const)&netValue, 2);
	}
	
	template<>
	inline bool WritePipe::writeValue(const sf::Int16* const value)
	{
		const sf::Int16 netValue = htons(*value);
		return writeContentSafe((const char* const)&netValue, 2);

	}
	
	template<>
	inline bool WritePipe::writeValue(const sf::Uint32* const value)
	{
		const sf::Uint32 netValue = htonl(*value);
		return writeContentSafe((const char* const)&netValue, 4);
	}
	
	template<>
	inline bool WritePipe::writeValue(const sf::Int32* const value)
	{
		const sf::Int32 netValue = htonl(*value);
		return writeContentSafe((const char* const)&netValue, 4);
	}
	
	template<>
	inline bool WritePipe::writeValue(const sf::Uint64* const value)
	{
		const sf::Uint64 netValue = htonll(*value);
		return writeContentSafe((const char* const)&netValue, 8);
	}
	
	template<>
	inline bool WritePipe::writeValue(const sf::Int64* const value)
	{
		const sf::Int64 netValue = htonll(*value);
		return writeContentSafe((const char* const)&netValue, 8);
	}
	
	template<>
	inline bool WritePipe::writeValue(const float* const value)
	{
		const unsigned int netValue = htonf(*value);
		return writeContentSafe((const char* const)&netValue, sizeof(netValue));
	}
	
	template<>
	inline bool WritePipe::writeValue(const double* const value)
	{
		const unsigned long long netValue = htond(*value);
		return writeContentSafe((const char* const) &netValue, sizeof(netValue));
	}
}

