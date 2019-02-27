#include "StaticResource.h"
#include <Client/source/resource/ResourceInterface.h>

namespace
{
	const wchar_t* STATIC_RES_FOLDER = L"static";
	const wchar_t* STATIC_ID_TRANSLATION =
	{
		L"font.ttf"
	};
}

namespace Resource
{
	std::vector<std::wstring> Static::GetTranslations()
	{
		std::vector<std::wstring> result;

		for (int i = 0; i < (int) ID::_Length; ++i)
		{
			result.push_back(
				Translate((ID) i)
			);
		}
		
		return std::move(result);
	}

	std::wstring Static::GetFolderName()
	{
		return STATIC_RES_FOLDER;
	}

	std::wstring Static::GetPath()
	{
		return Interface::GetResourcePath() + L"/" + GetFolderName();
	}

	std::wstring Static::Translate(const ID type)
	{
		return GetPath() + STATIC_ID_TRANSLATION[(int) type];
	}
}
