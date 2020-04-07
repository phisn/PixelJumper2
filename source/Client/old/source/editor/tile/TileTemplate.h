#pragma once

#include <Client/source/Common.h>
#include <Client/source/editor/tile/EditorTileBase.h>
#include <Client/source/menu/MenuElementBase.h>

// only for new tile template
// #include <Client/source/shared/tiles/TileDescription.h>

#include <SFML/Graphics/Color.hpp>

#include <string>

namespace Editor
{
	/*class _TileTemplate
	{
		const Shared::TileID id;

	public:
		_TileTemplate(const Shared::TileID id)
			:
			id(id),
			info(Shared::TileDescription::Find(id))
		{
			representation = createRepresentation();
		}

		~_TileTemplate()
		{
			delete representation;
		}

		Menu::ElementBase* getRepresentation()
		{
			return representation;
		}

		const std::wstring& getName() const
		{
			return info->name;
		}

		const std::wstring& getDescription() const
		{
			return info->info;
		}

		Shared::TileID getId() const
		{
			return id;
		}

	protected:
		virtual Menu::ElementBase* createRepresentation()
		{
			TileChoiceButton<TileChoiceMaterial::Default>* const button = new TileChoiceButton<TileChoiceMaterial::Default>();

			button->sizePreferred = { 300, 100 };
			button->buttonSelectedEvent.addListener(
				[this]()
				{
					representationTileSelect();
				});
			button->buttonPressedEvent.addListener(
				[this]()
				{
					representationTileCreate();
				});

			button->value.text = info->name;

			return button;
		}

		virtual void representationTileSelect()
		{
			Manipulator::GetCache()->tileChoice.writeInput()->selection = this;

			if (!Manipulator::GetCache()->tileChoice.notify())
			{
				Log::Error(L"Failed to select template");
			}
		}

		virtual void representationTileCreate()
		{
			if (!Manipulator::GetExecutor()->execute<TilePlace>())
			{
				Log::Error(L"Failed to place tiles");
			}
		}

	private:
		const Shared::TileDescription* const info;

		Menu::ElementBase* representation;
	};*/

	class TileTemplate
	{
	public:
		virtual TileBase* create(const sf::Vector2f position) = 0;

		virtual const std::wstring& getName() const = 0;
		virtual const std::wstring& getDescription() const = 0;
		virtual Shared::TileID getId() const = 0;

		virtual bool isSelected() const = 0;

		virtual void select() = 0;
		virtual void unselect() = 0;

		// usally a button
		virtual Menu::ElementBase* createRepresentation() = 0;
	};
}