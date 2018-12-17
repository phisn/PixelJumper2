#pragma once

#include <Client/source/resource/Common.h>

#include <string>

namespace Resource
{
	class Base
	{
	public:
	};

	struct Definition
	{
		typedef Base* (*Creator)(); // argument == some_cool_buffer

		Definition(
			const Magic magic,
			const std::wstring name,
			const Creator create)
			:
			magic(magic),
			name(name),
			path(DEFAULT_RES_PATH + name + L"/"),
			create(create)
		{
		}

		const Magic magic;
		const std::wstring name, path;

		const Creator create;
	};
}
