#pragma once

#include <Client/source/game/GameProperty.h>
#include <Client/source/game/GameState.h>

namespace Game
{
	class PlayerRepresentation
	{
	public:
		virtual void update(const sf::Time time) = 0;
		virtual void draw(sf::RenderTarget& target) const = 0;

		virtual void setPosition(const sf::Vector2f position) = 0;
		virtual void setSize(const sf::Vector2f size) = 0;
	};

	class CommonPlayerRepresentation
		:
		public PlayerRepresentation
	{
	public:
		CommonPlayerRepresentation(
			const sf::Color outerColor = sf::Color::Color::Red,
			const sf::Color innerColor = sf::Color::Color::Green)
		{
			shape.setOutlineColor(outerColor);
			shape.setFillColor(innerColor);
		}

		void update(const sf::Time time) override
		{
		}

		void draw(sf::RenderTarget& target) const override
		{
			target.draw(shape);
		}

		virtual void setPosition(const sf::Vector2f position) override
		{
			shape.setPosition(position);
		}

		virtual void setSize(const sf::Vector2f size) override
		{
			const float outerFactor = 0.1f;

			shape.setSize(size * (1 - outerFactor * 2));
			shape.setOutlineThickness(
				(size.x < size.y ? size.x : size.y)
				* outerFactor);
		}

	private:
		sf::RectangleShape shape;
	};

	struct RawPlayerProperties
	{
		sf::Vector2f position, movement;
	};

	class TileDataStorage
		:
		public GameState
	{
	public:
		void update()
		{
			for (decltype(tileData)::const_reference data : tileData)
				data.second.update();
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			for (decltype(tileData)::const_reference data : tileData)
				if (!data.second->writeState(writePipe))
				{
					return false;
				}

			return true;
		}

		bool readState(Resource::ReadPipe * const readPipe) override
		{
			for (decltype(tileData)::const_reference data : tileData)
				if (!data.second->readState(readPipe))
				{
					return false;
				}

			return true;
		}

		template <typename T>
		T* readAs(const TileIdentity identity) const
		{
			return dynamic_cast<T*>(tileData[identity].getValue());
		}

		std::map<TileIdentity, Property<GameState*>> tileData;
	};

	class PlayerProperties
		:
		public GameState
	{
	public:
		Property<sf::Vector2f> position, movement;

		TileDataStorage tileDataStorage;

		void update()
		{
			position.update();
			movement.update();

			tileDataStorage.update();
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			RawPlayerProperties rpp = createRaw();
			return writePipe->writeValue(&rpp)
				&& tileDataStorage.writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			RawPlayerProperties rpp;

			if (!readPipe->readValue(&rpp) &&
				!tileDataStorage.readState(readPipe))
			{
				return false;
			}

			position.setValue(rpp.position);
			movement.setValue(rpp.movement);

			update(); // TODO: ?
			
			return true;
		}

		RawPlayerProperties createRaw() const
		{
			RawPlayerProperties rpp;

			rpp.position = position;
			rpp.movement = movement;

			return rpp;
		}
	};

	class TruePlayer
	{
	public:	
		TruePlayer(PlayerRepresentation* const representation)
			:
			representation(representation)
		{
		}

		~TruePlayer()
		{
			delete representation;
		}

		PlayerProperties& getProperties()
		{
			return properties;
		}

	private:
		PlayerProperties properties;
		PlayerRepresentation* const representation;
	};
}