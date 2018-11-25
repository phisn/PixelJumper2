#pragma once

#include <Client/source/resource/StaticResource.h>

#include <utility>

namespace Device
{
	/*
	
		Handling resources and settings
		using files
	
	*/

	class Resource
	{
	public:
		typedef RESOURCE::Static::Resource Content;

		bool initialize();

		Content obtain(
			RESOURCE::Static::Type type);
		Content obtain(
			const std::wstring folderName,
			const std::wstring fileName);
		Content obtain(
			const std::wstring fileName);
	};
}
