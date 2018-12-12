#pragma once

#include <Client/source/resource/Common.h>

#include <string>

namespace Resource
{
	struct Definition
	{
		Definition(
			const Magic magic,
			const std::wstring name)
			:
			magic(magic),
			name(name),
			path(DEFAULT_RES_PATH + name + L"/")
		{
		}

		const Magic magic;
		const std::wstring name, path
	};

	class Base
	{
	public:
	};
}
