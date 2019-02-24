#include "ResourceInterface.h"

#include <Client/source/resource/ResourceDefinition.h>

namespace
{
	const wchar_t* const GENERAL_RESOURCE_PATH = L"resource";

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
				+ filename.filename().wstring()
				+ L"\')"
			);

			return false;
		}

		/*
		Resource::Magic magic;
		file.read(
			(char*) &magic,
			sizeof(magic)
		);

		if (!file || file.eof())
		{
			Log::Warning(
				L"Invalid file size ('"
				+ filename.filename().wstring()
				+ L"\')"
			);

			return false;
		}

		if (Resource::Interface::GetDefinition(type)->magic != magic) // TODO: is valid magic
		{
			Log::Warning(
				L"Invalid file format ('"
				+ filename.filename().wstring()
				+ L"\')"
			);

			return false;
		}*/

		Resource::FileDefinition* resource = &mappedResources[type][filename.filename().wstring()];
		// resource->file = std::move(file); | do not store handle
		resource->path = filename.wstring();
		resource->size = std::filesystem::file_size( resource->path );

		return true;
	}

	bool mapResource(
		const Resource::ResourceType type,
		const std::wstring name)
	{
		return mapResource(
			type, 
			std::filesystem::path(
				Resource::Definition::Get(type)->path + name
			)
		);
	}

	bool mapResourceFolder(
		const Resource::ResourceType type)
	{
		try {
			mappedResources[type].clear();

			for (const std::filesystem::directory_entry& entry
				: std::filesystem::directory_iterator(
					Resource::Definition::Get(type)->path
				))
			{
				const std::filesystem::path path = entry.path();

				if (Resource::Definition::Get(type)->hasExtension && !(
						path.has_extension() &&
						path.extension() == Resource::Definition::Get(type)->extension
					))
				{
					continue;
				}

				mapResource(type, path);
			}

			Log::Information(
				L"Total mapped '" 
				+ std::wstring(Resource::Definition::Get(type)->name)
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
				+ std::to_wstring(error.code().value())
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
		Log::Section section(L"Initializing resources");

		// create missing folder
		for (int i = 0; i < (int)Resource::ResourceType::_Length; ++i)
		{
			const std::filesystem::path& path(
				std::wstring(Resource::Definition::Get((Resource::ResourceType) i)->path)
			);

			if (!std::filesystem::exists(path))
			{
				Log::Warning(L"Path '" + path.wstring() + L"' did not exists. Creating new one");
				std::filesystem::create_directory(path);
			}
		}

		// check that all static resource exist
		const wchar_t** translations = Static::GetTranslations();
		for (int i = 0; i < Static::GetTranslationCount(); ++i)
			if (!std::filesystem::exists(translations[i]))
			{
				section.error(
					std::wstring(L"Static resource '")
					.append(translations[i])
					.append(L"' not found")
				);

				return false;
			}

		return mapAllResources();
	}

	bool Interface::RemapAllFiles()
	{
		return mapAllResources();
	}

	bool Interface::RemapFiles(
		const ResourceType type)
	{
		Log::Section section(L"Mapping all '" + std::wstring(Resource::Definition::Get(type)->name) + L"' resources");

		return mapResourceFolder(type);
	}

	bool Interface::WriteResource(
		ResourceBase* const resource, 
		const ResourceType type, 
		const std::wstring name)
	{
		SubResources* const subResources = &mappedResources[type];
		SubResources::iterator it = subResources->find(name);

		FileDefinition* destination;

		if (it == subResources->cend())
		{
			destination = &subResources->emplace(
				name, 
				Resource::Definition::Get(type)->path + name
			).first->second;
		}
		else
		{
			destination = &it->second;
		}

		if (WriteRawResource(resource, destination))
		{
			return true;
		}
		else
		{
			Log::Error(L"Failed write resource (type: '" 
				+ std::wstring(Resource::Definition::Get(type)->name)
				+ L"', name: '"
				+ name
				+ L"')");

			return false;
		}
	}

	bool Interface::ReadResource(
		ResourceBase* const resource,
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

		if (ReadRawResource(resource, &it->second))
		{
			return true;
		}
		else
		{
			Log::Error(L"Failed read resource (type: '"
				+ std::wstring(Resource::Definition::Get(type)->name)
				+ L"', name: '"
				+ name
				+ L"')");

			return false;
		}
	}

	bool Interface::ReadRawResource(
		ResourceBase* const resource, 
		const std::filesystem::path path)
	{
		FileDefinition fd(path);
		return ReadRawResource(resource, &fd);
	}

	bool Interface::ReadRawResource(
		ResourceBase* const resource, 
		const FileDefinition* const file)
	{
		FileReadPipe fwp(file);

		if (!fwp.isValid())
		{
			Log::Error(L"Failed to open file '" + file->path.filename().wstring() + L"'");
			Log::Error(std::wstring(L"Error message: '") + _wcserror(errno) + L"'");

			return false;
		}

		if (!resource->make(&fwp))
		{
			if (!fwp.isValid())
			{
				Log::Error(L"Failed to read file '" + file->path.filename().wstring() + L"'");
				Log::Error(std::wstring(L"Error message: '") + _wcserror(errno) + L"'");
			}

			return false;
		}

		return true;
	}

	bool Interface::WriteRawResource(
		ResourceBase* const resource, 
		const std::filesystem::path path)
	{
		FileDefinition fd(path);
		return WriteRawResource(resource, &fd);
	}

	bool Interface::WriteRawResource(
		ResourceBase* const resource, 
		FileDefinition* const file)
	{
		FileWritePipe fwp(file);

		if (!fwp.isValid())
		{
			Log::Error(L"Failed to open file '" + file->path.filename().wstring() + L"'");
			Log::Error(std::wstring(L"Error message: '") + _wcserror(errno) + L"'");

			return false;
		}

		if (!resource->save(&fwp))
		{
			if (!fwp.isValid())
			{
				Log::Error(L"Failed to write file '" + file->path.filename().wstring() + L"'");
				Log::Error(std::wstring(L"Error message: '") + _wcserror(errno) + L"'");
			}

			return false;
		}

		return true;
	}

	Static::Resource Interface::GetStaticResource(
		const Static::ID type)
	{
		GetStaticResource( Static::Translate(type) );
	}

	Static::Resource Interface::GetStaticResource(
		const std::filesystem::path resource)
	{
		Static::Resource result;

		FileDefinition fileDefinition(resource);

		if (fileDefinition.size == 0)
		{
			Log::Error(L"Static resource '" +
				resource.wstring() +
				L"' does not exist or is empty");

			return { };
		}

		FileReadPipe filePipe(&fileDefinition);

		result.first = new char[fileDefinition.size];
		result.second = fileDefinition.size;

		if (!filePipe.readContentForce(
				result.first, 
				result.second))
		{
			Log::Error(L"Unable to read static resource '"
				+ resource.wstring() + L"'");

			if (!filePipe.isValid())
			{
				Log::Error(std::wstring(L"Error message: '") + _wcserror(errno) + L"'");
			}

			delete[] result.first;
			return { };
		}

		return result;
	}

	const std::wstring Interface::GetResourcePath()
	{
		return GENERAL_RESOURCE_PATH;
	}

	const std::wstring Interface::MakeResourcePath(const ResourceType type)
	{
		return GetResourcePath() + L"/" + Definition::Get(type)->path;
	}
}
