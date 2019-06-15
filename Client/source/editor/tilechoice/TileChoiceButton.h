#pragma once

#include <Client/source/menu/MenuButton.h>
#include <Client/source/menu/MenuButtonMaterial.h>

namespace Editor
{
	enum class TileChoiceButtonEffect
	{
		Selected,
		SelectedHover,
		SelectedPressed
	};

	class TileChoiceButtonMaterial
		:
		public Menu::ButtonMaterial::DefaultStaticRectangleBase
	{
	public:
		TileChoiceButtonMaterial()
		{
			initialize();
		}

		void applyEffect(const Menu::CommonControlEffect effect) override
		{
			DefaultStaticRectangleBase::applyEffect(effect);
			currentlySelected = false;
		}

		void applyAdditionalStyle(const TileChoiceButtonEffect effect)
		{
			applyStyle(additionalStyles[(int) effect]);
			currentlySelected = true;
		}

	private:
		TileChoiceButtonEffect currentAdditionalEffect;
		bool currentlySelected = false;

		void updateEffect() override
		{
			if (currentlySelected)
			{
				applyAdditionalStyle(currentAdditionalEffect);
			}
			else
			{
				DefaultStaticRectangleBase::updateEffect();
			}
		}

		const Menu::CommonShapeStyle& findStyle(const Menu::CommonControlEffect effect) const override
		{
			return styles[(int) effect];
		}

		const Menu::CommonShapeStyle styles[3] =
		{
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(40, 40, 40), 0.5f },
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3.f },
			{ sf::Color::Color(80, 80, 80), sf::Color::Color(), 0.f }
		};

		const Menu::CommonShapeStyle additionalStyles[3] =
		{ 
			{ sf::Color::Color(80, 80, 80), sf::Color::Color(50, 50, 50), 3.f },
			{ sf::Color::Color(80, 80, 80), sf::Color::Color(110, 110, 110), 3.f },
			{ sf::Color::Color(110, 110, 110), sf::Color::Color(0, 0, 0), 0.f }
		};
	};

	class TileChoiceButton
		:
		public Menu::Button<TileChoiceButtonMaterial>
	{
	public:
		TileChoiceButton()
		{
		}

		Menu::MenuNotifier<TileChoiceButton> buttonSelectedEvent;

		void unselect()
		{
			selected = false;
		}

		bool isSelected() const
		{
			return selected;
		}

	protected:
		virtual void onButtonPressed() override
		{
			if (!selected)
			{
				selected = true;
				buttonSelectedEvent.notify();
			}
			else
			{
				Button::onButtonPressed();
			}
		}

		virtual void onGraphicsDefault()
		{
			if (selected)
			{
				material.applyAdditionalStyle(TileChoiceButtonEffect::Selected);
			}
			else
			{
				Button::onGraphicsDefault();
			}
		}

		virtual void onGraphicsHovered()
		{
			if (selected)
			{
				material.applyAdditionalStyle(TileChoiceButtonEffect::SelectedHover);
			}
			else
			{
				Button::onGraphicsHovered();
			}
		}

		virtual void onGraphicsPressed()
		{
			if (selected)
			{
				material.applyAdditionalStyle(TileChoiceButtonEffect::SelectedPressed);
			}
			else
			{
				Button::onGraphicsPressed();
			}
		}

	private:
		bool selected;
	};
}
