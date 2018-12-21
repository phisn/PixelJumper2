#pragma once

#include <SFML/Main.hpp>

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
	private:
		int position;
	};

	class WritePipe
		:
		public BasePipe
	{
	public:
		virtual ~WritePipe() = 0 { }

		virtual void reserveSize(const sf::Uint64) = 0;
		virtual void writeContent(
			char* buffer,
			const sf::Uint64 size) = 0;
	};

	class ReadPipe
		:
		public BasePipe
	{
	public:
		virtual ~ReadPipe() = 0 { }

		virtual void readContent(
			char* buffer,
			const sf::Uint64 size) = 0;
	};
}
