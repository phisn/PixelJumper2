#include "ResourceInterface.h"

namespace
{
	Resource::MapedResources mappedResources;

	bool mapResource(
		const Resource::ResourceType type,
		const std::filesystem::path filename)
	{
		Log::Information(
			L"Mapping resource '" 
			+ filename.filename().wstring() 
			+ L"' as " 
			+ std::to_wstring((int) type)
		);

		std::ifstream file(
			filename,
			std::ios::binary | std::ios::in);

		if (!file)
		{
			Log::Warning(
				L"Unable to open file ('"
				+ filename.filename.wstring()
				+ L'\')'
			);

			return false;
		}

		Resource::Magic magic;
		file.read(
			(char*) &magic,
			sizeof(magic)
		);

		if (!file || file.eof())
		{
			Log::Warning(
				L"Invalid file size ('"
				+ filename.filename.wstring()
				+ L'\')'
			);

			return false;
		}

		if (Resource::Interface::GetDefinition(type)->magic != magic) // TODO: is valid magic
		{
			Log::Warning(
				L"Invalid file format ('"
				+ filename.filename.wstring()
				+ L'\')'
			);

			return false;
		}

		Resource::MappedResource* resource = &mappedResources[type][filename.filename.wstring()];
		resource->file = std::move(file);
		resource->path = filename.wstring();
		resource->size = std::filesystem::file_size( resource->path );

		return true;
	}

	bool mapResource(
		const Resource::ResourceType type,
		const std::wstring name)
	{
		mapResource(
			type, 
			Resource::Interface::GetDefinition(type)->path + name
		);
	}

	bool mapResourceFolder(
		const Resource::ResourceType type)
	{
		try {
			mappedResources[type].clear();

			for (const std::filesystem::directory_entry& entry
				: std::filesystem::directory_iterator(
					resourceDefinitions[(int) type].path
				))
			{
				const std::filesystem::path path = entry.path();

				if (path.has_extension() &&
					path.extension() == DEFAULT_RES_EXTENSION)
				{
					mapResource(type, path);
				}
			}

			Log::Information(
				L"Total mapped '" 
				+ Resource::Interface::Translate(type)
				+ L"' resources: '"
				+ std::to_wstring( mappedResources[type].size() )
				+ L'\''
			);

			return true;
		}
		catch (const std::filesystem::filesystem_error error)
		{
			Log::Error(
				L"Failed to map files, Error Code: "
				+ std::to_wstring(error.code)
			);

			return false;
		}
	}

	bool mapAllResources()
	{
		Log::Section section(L"Mapping all resources");

		for (int i = 0; i < (int) Resource::ResourceType::_Length; ++i)
			if (!mapResourceFolder((Resource::ResourceType) i))
			{
				return false;
			}

		return true;
	}

	bool isResourcesMapped(
		const Resource::ResourceType type,
		const std::wstring name)
	{
		Resource::SubResources* subResources = &mappedResources[type];
		Resource::SubResources::const_iterator it = subResources->find(name);

		return it != subResources->cend();
	}
}

namespace Resource
{
	bool Interface::Initialize()
	{
		return mapAllResources();

		// ...
	}

	bool Interface::RemapAllFiles()
	{
		return mapAllResources();
	}

	bool Interface::RemapFiles(
		const ResourceType type)
	{
		Log::Section section(L"Mapping all '" + Translate(type) + L"' resources");

		return mapResourceFolder;
	}

	bool Interface::WriteResource(
		const Base* const resource, 
		const ResourceType type, 
		const std::wstring name)
	{

		return true;
	}

	bool Interface::ReadResource(
		Base* resource,
		const ResourceType type,
		const std::wstring name)
	{
		SubResources* const subResources = &mappedResources[type];
		SubResources::iterator it = subResources->find(name);

		if (it == subResources->cend())
		{
			if (mapResource(type, name))
			{
				it = mappedResources[type].find(name);
			}
			else
			{
				Log::Error(
					L"Resource '"
					+ name 
					+ L"' not found"
				);

				return NULL;
			}
		}

		FileReadPipe pipe = it->second;

		if ( !pipe.isValid() )
		{
			return false;
		}

	}

	const Definition* Interface::GetDefinition(
		const ResourceType resource)
	{
		return &resourceDefinitions[(int) resource];
	}

	std::wstring Interface::Translate(
		const ResourceType resource)
	{
		return resourceDefinitions[(int) resource].name;
	}
}
