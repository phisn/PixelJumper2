#include "StaticResource.h"

static const wchar_t* static_resource_names[] =
{
	L"resource/font.ttf"
};

const int Resource::GetTranslationCount()
{
	return sizeof(static_resource_names) / sizeof(*static_resource_names);
}

const wchar_t** Resource::GetTranslations()
{
	return static_resource_names;
}

const std::wstring Resource::Translate(
	RESOURCE::StaticResource::Type type)
{
	return static_resource_names[(int) type];
}
