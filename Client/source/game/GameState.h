#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

#include <cassert>
#include <functional>

namespace Game
{
	struct GameState
	{
		virtual bool writeState(Resource::WritePipe* const writePipe) = 0;
		virtual bool readState(Resource::ReadPipe* const readPipe) = 0;
	};

	template <typename T, typename Special>
	class StateProperty
	{
	public:
		typedef void(__thiscall Special::*OnChange)(const T oldValue);

		StateProperty(const OnChange onChange)
			:
			onChange(onChange)
		{
		}

		StateProperty& operator=(const T value)
		{
			const T oldValue = this->value;

			this->value = value;
			onChange(oldValue);

			return *this;
		}

		operator const T() const
		{
			return value;
		}

		T getValue() const
		{
			return value;
		}

	private:
		const OnChange onChange;
		T value;
	};
}
