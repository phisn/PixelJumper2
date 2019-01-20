#include "ResourceDevice.h"

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/ResourceInterface.h>

#include <filesystem>

namespace Device
{
	bool Resource::initialize()
	{
		Log::Section section(L"Initializing ResourceDevice");
		section.information(
			L"Checking '" 
			+ std::to_wstring( RESOURCE::GetTranslationCount() )
			+ L"' resources");

		const wchar_t** translations = RESOURCE::GetTranslations();
		for (int i = 0; i < RESOURCE::GetTranslationCount(); ++i)
			if (!std::filesystem::exists(translations[i]))
			{
				section.error(
					std::wstring(L"File '")
						.append(translations[i])
						.append(L"' not found")
				);

				return false;
			}

		if (!::Resource::Interface::Initialize())
		{
			return false;
		}

		return true;
	}

	Resource::Content Resource::obtain(
		RESOURCE::Static::Type type)
	{
		return obtain( RESOURCE::Translate(type) );
	}
	
	Resource::Content Resource::obtain(
		const std::wstring folderName, 
		const std::wstring fileName)
	{
		return obtain(folderName + L"\\" + fileName);
	}

	Resource::Content Resource::obtain(
		const std::wstring fileName)
	{
		Content content = { };

		std::ifstream file(
			fileName,
			std::ios::binary | std::ios::in | std::ios::ate);

		if ( file.fail() )
		{
			return content;
		}

		content.second = file.tellg();
		file.clear();
		file.seekg(0, std::ios::beg);

		// released in 'ResourceContext' using RAII
		content.first = new char[content.second];
		file.read(
			content.first,
			content.second);

		if ( file.fail() )
		{
			delete[] content.first;

			content.first = NULL;
			content.second = NULL;
		}

		return content;
	}
}
