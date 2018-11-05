#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/menu/element/LogicButton.h>

namespace Menu
{
	class SimpleButton
		:
		public LogicButton
	{
	public:
		typedef Properties Properties;

		struct Style
		{
			sf::Color
				default_fillColor,
				default_outlineColor,

				enter_fillColor,
				enter_outlineColor,

				click_fillColor,
				click_outlineColor;

			float
				default_outlineThickness,
				enter_outlineThickness,
				click_outlineThickness;
		};

		/*
		
			Policy -> (
				ElementBase* const
			)
		
		*/
		SimpleButton(
			ElementBase* const parent,
			const Style style)
			:
			LogicButton(parent),
			style(style)
		{
			setDefaultStyle();
		}

		virtual ~SimpleButton() {  }

		void initialize(
			ElementBase::Properties* const properties) override
		{
			this->properties = *(Properties*) properties;

			loadPosition();
		}

		virtual void onDraw() override
		{
			DEVICE::Interface::getScreen()->onDraw(&shape);
		}

		sf::Vector2f getPosition() const override
		{
			return shape.getPosition();
		}

		sf::Vector2f getSize() const override
		{
			return shape.getSize();
		}

		void loadPosition() override
		{
			shape.setSize(
				properties.size
			);
		
			shape.setPosition(convertPosition(
				properties.position
			));
		}

	protected:
		virtual void onMouseEnter() override;
		virtual void onMouseLeave() override;

		virtual void onMouseClickBegin() override;
		virtual void onMouseClickEnd() override;

	private:
		Properties properties;
		const Style style;

		void setEnterStyle();
		void setDefaultStyle();

		bool checkIsInside(
			const int x,
			const int y) const override;

		sf::RectangleShape shape;
	};

	inline void SimpleButton::onMouseEnter()
	{
		setEnterStyle();
	}

	inline void SimpleButton::onMouseLeave()
	{
		setDefaultStyle();
	}

	inline void SimpleButton::onMouseClickEnd()
	{
		setEnterStyle();
	}
}
