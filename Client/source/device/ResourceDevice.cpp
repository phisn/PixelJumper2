#include "ResourceDevice.h"

#include <Client/source/Logger.h>
#include <fstream>

namespace Device
{
	bool Resource::initialize()
	{
		Log::Section section(L"Initializing ResourceDevice");

		const wchar_t** translations = RESOURCE::GetTranslations();
		for (int i = 0; i < RESOURCE::GetTranslationCount(); ++i)
			if (std::wifstream(translations[i]).fail())
			{
				section.error(
					std::wstring(L"File '")
						.append(translations[i])
						.append(L"' not found")
				);

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
