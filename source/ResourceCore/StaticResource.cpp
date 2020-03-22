#include "StaticResource.h"
#include "ResourceInterface.h"

namespace
{
	const wchar_t* STATIC_RES_FOLDER = L"static";
	const wchar_t* STATIC_ID_TRANSLATION[] =
	{
		L"font.ttf"
	};
}

namespace Resource
{
	std::vector<std::wstring> Static::GetTranslations()
	{
		std::vector<std::wstring> result;

		for (int i = 0; i < (int) Id::_Length; ++i)
		{
			result.push_back(
				Translate((Id) i)
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

	std::wstring Static::Translate(const Id type)
	{
		return GetPath() + L"/" + STATIC_ID_TRANSLATION[(int) type];
	}
}
