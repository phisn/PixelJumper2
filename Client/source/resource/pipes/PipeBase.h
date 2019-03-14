#pragma once

#include <SFML/Main.hpp>

#include <string>

namespace Resource
{
	class BasePipe // never used as plmp
	{
	public:
		sf::Uint64 getPosition() const
		{
			return position;
		}
		
		virtual void setPosition(
			const sf::Uint64 position)
		{
			this->position = position;
		}

		virtual sf::Uint64 getSize() const = 0;

		void goToEnd()
		{
			setPosition( getSize() );
		}

		void goToBegin()
		{
			setPosition(0);
		}

		virtual bool isValid() const = 0;
	protected:
		sf::Uint64 position = 0;
	};

	class WritePipe
		:
		public BasePipe
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
			if ( !isValid() )
			{
				return false;
			}

			writeContent(buffer, size);

			return isValid();
		}

		template <typename T>
		bool writeValue(const T* const value)
		{
			return writeContentSafe((const char* const) value, sizeof(T));
		}

		template <typename Size>
		bool writeString(
			const std::wstring* const str)
		{
			Size size = (Size) str->size();

			if (size <= 0)
			{
				return false;
			}

			if ( !writeValue(&size) )
			{
				return false;
			}

			return writeContentSafe((char * const) str->c_str(), size * sizeof(wchar_t));
		}
	};

	class ReadPipe
		:
		public BasePipe
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
			return readContent(buffer, size) == size && isValid();
		}
		
		template <typename T>
		bool readValue(T* const value)
		{
			return readContentForce((char*) value, sizeof(T));
		}

		template <typename Size>
		bool readString(
			std::wstring* const str)
		{
			Size size;

			if ( !readValue(&size) )
			{
				return false;
			}

			if (size <= 0)
			{
				return false;
			}

			str->resize(size);

			return readContentForce(
				(char*) str->c_str(), size * sizeof(wchar_t)
			);
		}
	};
}
