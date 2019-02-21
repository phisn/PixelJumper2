#include "StaticResource.h"

static const wchar_t* static_resource_names[] =
{
	L"resource/font.ttf"
};

const int Resource::Static::GetTranslationCount()
{
	return sizeof(static_resource_names) / sizeof(*static_resource_names);
}

const wchar_t** Resource::Static::GetTranslations()
{
	return static_resource_names;
}

std::wstring Resource::Static::Translate(const Type type)
{
	return static_resource_names[(int) type];
}
