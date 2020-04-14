#include "ResourceInterface.h"
#include "pipes/FilePipe.h"

#include <cassert>
#include <map>
#include <string>

namespace Resource
{
	std::wstring GetResourceErrorMessage();

	const ResourceTypeDefinition WorldResourceDefinition
	{
		"world resources",
		"world",
		"wrld"
	};

	std::map<std::filesystem::path, std::vector<char>> cachedResources;

	bool Interface::SaveResource(
		std::wstring filename, 
		ResourceBase* resource,
		const ResourceTypeDefinition& type)
	{
		std::filesystem::path path = GetResourcePath(filename, type);

		Log::Section section(L"saving file",
			path.c_str(), L"path",
			type.name, L"type");

		FileWritePipe pipe{ path };

		if (!pipe.isValid())
		{
			Log::Error(L"failed to open file for save",
				GetResourceErrorMessage(), L"error");

			return false;
		}

		if (!resource->save(&pipe))
		{
			Log::Error(L"failed to save resource",
				pipe.isValid(), L"file_valid",
				GetResourceErrorMessage(), L"error");

			return false;
		}

		pipe.close();

		if (!pipe.isValid())
		{
			Log::Error(L"failed to close resource after save",
				GetResourceErrorMessage(), L"error");

			return false;
		}


		//////////////////////////////////////////////////////////////
		// update cache if cached

		return true;
	}

	bool Interface::LoadResource(
		std::wstring filename, 
		ResourceBase* resource,
		const ResourceTypeDefinition& type)
	{
		//////////////////////////////////////////////////////////////
		// check for chached file

		std::filesystem::path path = GetResourcePath(filename, type);

		Log::Section section(L"loading file",
			path.c_str(), L"path",
			type.name, L"type");

		if (!std::filesystem::exists(path))
		{
			Log::Error(L"failed to open file, does not exist");

			return false;
		}

		FileReadPipe pipe{ path };

		if (!pipe.isValid())
		{
			Log::Error(L"failed to open file",
				GetResourceErrorMessage(), L"error");

			return false;
		}

		if (!resource->make(&pipe))
		{
			Log::Error(L"failed to save resource",
				pipe.isValid(), L"file_valid",
				GetResourceErrorMessage(), L"error");

			return false;
		}

		pipe.close();

		// we ignore weather this works or not
		// we just assume the resource is loaded correctly
		// and everything else can be ignored
		if (!pipe.isValid())
		{
			Log::Error(L"failed to close resource after load",
				GetResourceErrorMessage(), L"error");
		}

		return true;
	}

	bool Interface::CacheResource(
		std::wstring filename,
		const ResourceTypeDefinition& type)
	{
		std::filesystem::path path = GetResourcePath(filename, type);

		Log::Section section(L"caching file",
			path.c_str(), L"path",
			type.name, L"type");

		if (cachedResources.find(path) != cachedResources.end())
		{
			Log::Information(L"resource already cached, skipping process");
			return true;
		}

		if (!std::filesystem::exists(path))
		{
			Log::Error(L"failed to open file, does not exist");
			return false;
		}

		FileReadPipe pipe{ path };

		if (!pipe.isValid())
		{
			Log::Error(L"failed to open file for caching",
				GetResourceErrorMessage(), L"error");

			return false;
		}

		std::vector<char>& cachedResource = cachedResources[path];
		cachedResource.reserve(pipe.getSize());

		if (!pipe.readContentForce(&cachedResource[0], pipe.getSize()))
		{
			Log::Error(L"failed to read file for caching");
			return false;
		}

		return true;
	}

	std::wstring GetResourceErrorMessage()
	{
		return std::wstring(_wcserror(errno));
	}
}

namespace
{
	const wchar_t* RESOURCE_PATH = L"resource";

	typedef std::wstring ResourceName;
	typedef std::map<ResourceName, Resource::FileDefinition> ResourceMap;
	typedef std::map<Resource::ResourceType, ResourceMap> ResourceTypeMap;

	ResourceTypeMap resourceTypes;
}

namespace _Resource
{
	std::wstring MakeDefaultResourceTypeFileError(
		const ResourceType type,
		const std::filesystem::path path)
	{
		return L"'" + path.filename().wstring() + L"' (resource path: '" + Interface::MakeResourceTypePath(type) + L"')";
	}

	std::wstring MakeErrorMessageError()
	{
		return L"(error message: '" + std::wstring( _wcserror(errno) ) + L"')";
	}

	bool SaveResourceFile(
		ResourceBase* const resource,
		const ResourceType type,
		FileDefinition* const file)
	{
		FileWritePipe fwp(file);

		if (!fwp.isValid())
		{
			Log::Error(L"Failed to open resource "
				+ MakeDefaultResourceTypeFileError(type, file->path)
				+ L" "
				+ MakeErrorMessageError()
			);

			return false;
		}

		if (!fwp.writeValue(&ResourceDefinition::Get(type)->magic))
		{
			Log::Error(L"Failed to write resource "
				+ MakeDefaultResourceTypeFileError(type, file->path)
				+ L" "
				+ MakeErrorMessageError()
			);

			return false;
		}

		if (!resource->save(&fwp))
		{
			Log::Error(L"Failed to save resource "
				+ MakeDefaultResourceTypeFileError(type, file->path)
				+ L" [maybe invalid] "
				+ MakeErrorMessageError()
			);

			return false;
		}

		Log::Information(L"Resource '"
			+ file->path.filename().wstring()
			+ L"' saved (resource path: '"
			+ ResourceDefinition::Get(type)->path
			+ L"')");

		return true;
	}

	bool LoadResourceFile(
		ResourceBase* const resource,
		const ResourceType type,
		FileDefinition* const file)
	{
		if (!file->doesExists())
		{
			Log::Error(L"Resource file not found "
				+ MakeDefaultResourceTypeFileError(type, file->path)
			);

			return false;
		}

		FileReadPipe frp(file);

		if (!frp.isValid())
		{
			Log::Error(L"Failed to open resource "
				+ MakeDefaultResourceTypeFileError(type, file->path)
				+ L" "
				+ MakeErrorMessageError()
			);

			return false;
		}

		{ // ensure magic
			ResourceTypeMagic::Type magic;
			if (!frp.readValue(&magic) || magic != ResourceDefinition::Get(type)->magic)
			{
				Log::Error(L"Failed to write resource "
					+ MakeDefaultResourceTypeFileError(type, file->path)
					+ L" "
					+ MakeErrorMessageError()
				);

				return false;
			}
		}

		if (!resource->make(&frp))
		{
			Log::Error(L"Failed to make resource "
				+ MakeDefaultResourceTypeFileError(type, file->path)
				+ L" [maybe invalid] "
				+ MakeErrorMessageError()
			);

			return false;
		}

		Log::Information(L"Resource '"
			+ file->path.filename().wstring()
			+ L"' made (resource path: '"
			+ ResourceDefinition::Get(type)->path
			+ L"')");

		return true;
	}

	bool MapResourceFile(
		const ResourceType type,
		const std::filesystem::path path)
	{
		FileDefinition fileDefinition(path);
		
		if (!fileDefinition.resetSize())
		{
			Log::Error(L"Failed to open file [definition] " 
				+ MakeDefaultResourceTypeFileError(type, path)
			);

			return false;
		}

		FileReadPipe frp(&fileDefinition);

		if (!frp.isValid())
		{
			Log::Error(L"Failed to open file [pipe] " 
				+ MakeDefaultResourceTypeFileError(type, path)
				+ L" "
				+ MakeErrorMessageError()
			);

			return false;
		}

		ResourceTypeMagic::Type magic;

		if (!frp.readValue(&magic))
		{
			Log::Error(L"Failed to read file [pipe] "
				+ MakeDefaultResourceTypeFileError(type, path)
				+ L" "
				+ MakeErrorMessageError()
			);

			return false;
		}

		if (magic != ResourceDefinition::Get(type)->magic)
		{
			Log::Error(L"File with invalid content [magic] "
				+ MakeDefaultResourceTypeFileError(type, path)
			);

			return false;
		}

		resourceTypes[type][path.stem().wstring()] = std::move(fileDefinition);

		return true;
	}

	bool VerifyDirectories()
	{
		try 
		{
			if (!std::filesystem::exists(Interface::GetResourcePath()))
			{
				Log::Warning(L"central resource directory did not exists. creating new one");
				std::filesystem::create_directory(Interface::GetResourcePath());
			}

			for (int i = 0; i < (int)ResourceType::_Length; ++i)
			{
				const std::filesystem::path path = Interface::MakeResourceTypePath((ResourceType) i);

				if (!std::filesystem::exists(path))
				{
					Log::Warning(std::wstring(L"Resource directory did not exists, creating new one '")
						+ ResourceDefinition::Get((ResourceType) i)->name
						+ L"' (resource path: '"
						+ path.wstring() + L"')");

					std::filesystem::create_directory(path);
				}
			}

			return true;
		}
		catch (const std::filesystem::filesystem_error error)
		{
			Log::Error(std::wstring(L"Failed to verify resource directories (error message: '")
				+ carrtowstr(error.what()) + L"')");

			return false;
		}
	}

	bool VerifyStaticResources()
	{
		Log::Information(Static::GetTranslations()[0]);

		for (const std::filesystem::path staticResource : Static::GetTranslations())
			if (!std::filesystem::exists(staticResource))
			{
				Log::Error(L"Unable to find static resource '"
					+ staticResource.filename().wstring()
					+ L"' (resource path: '"
					+ Static::GetPath() + L"')");

				return false;
			}

		// fix by somehow downloading content?

		return true;
	}

	bool Interface::Initialize()
	{
		Log::Section section(L"Initializing resource interface");

		if (!VerifyDirectories())
		{
			return false;
		}

		if (!VerifyStaticResources())
		{
			Log::Error(L"Static resources are corrupted, reinstall the game");

			return false;
		}
		
		if (!MapResources())
		{
			return false;
		}

		return true;
	}

	void Interface::Uninitalize()
	{
		resourceTypes.clear();
	}

	bool Interface::MapResources()
	{
		Log::Section section(L"Mapping all resource types");

		for (int i = (int) ResourceType::_Begin; i < (int) ResourceType::_Length; ++i)
			if (!MapResourceType((ResourceType)i))
			{
				return false;
			}

		return true;
	}

	bool Interface::MapResourceType(const ResourceType type)
	{
		Log::Section section(std::wstring(L"Mapping resource type '")
			+ ResourceDefinition::Get(type)->name + L"'");

		resourceTypes[type].clear();

		try {
			for (std::filesystem::directory_entry directoryEntry
				: std::filesystem::directory_iterator(MakeResourceTypePath(type))
				)
			{
				if (directoryEntry.status().permissions() != std::filesystem::perms::all)
				{
					Log::Warning(L"Not enough permissions to open file "
						+ MakeDefaultResourceTypeFileError(type, directoryEntry.path())
					);

					continue;
				}

				if (!directoryEntry.is_regular_file())
				{
					Log::Warning(L"File with irregular type "
						+ MakeDefaultResourceTypeFileError(type, directoryEntry.path())
					);

					continue;
				}

				if (directoryEntry.file_size() < 4)
				{
					Log::Warning(L"Too small or empty file "
						+ MakeDefaultResourceTypeFileError(type, directoryEntry.path())
					);

					continue;
				}

				if (ResourceDefinition::Get(type)->hasExtension && (!directoryEntry.path().has_extension()
					|| ResourceDefinition::Get(type)->extension != directoryEntry.path().extension()))
				{
					Log::Warning(L"File with invalid extension "
						+ MakeDefaultResourceTypeFileError(type, directoryEntry.path())
						+ L" (extension: '"
						+ directoryEntry.path().extension().wstring()
						+ L"')");

					continue;
				}

				// TODO: return currently unused
				MapResourceFile(type, directoryEntry.path());
			}
		}
		catch (const std::filesystem::filesystem_error error)
		{
			Log::Error(std::wstring(L"Failed to map resource type '")
				+ ResourceDefinition::Get(type)->name
				+ L"' (error message: '"
				+ _wcserror(error.code().value()) + L"')");

			return false;
		}

		Log::Information(L"Mapped '"
			+ std::to_wstring( resourceTypes.size() )
			+ L"' files (resource type: '"
			+ MakeResourceTypePath(type) + L"')");

		return true;
	}

	bool Resource::Interface::SaveResource(
		ResourceBase* const resource, 
		const ResourceType type, 
		const std::wstring name)
	{
		ResourceMap& resourceMap = resourceTypes[type];
		ResourceMap::iterator mappedResource = resourceMap.find(name);

		FileDefinition file;

		if (mappedResource == resourceMap.end())
		{
			file.path = MakeResourceFilePath(type, name);
			file.size = 0;

			Log::Information(L"Resource '"
				+ name
				+ L"' not found, creating new one (resource path: '"
				+ ResourceDefinition::Get(type)->path
				+ L"')");
		}
		else
		{
			file = mappedResource->second;
		}

		if (SaveResourceFile(
				resource,
				type,
				&file))
		{
			file.resetSize();
			resourceMap[name] = std::move(file);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool Resource::Interface::LoadResource(
		ResourceBase* const resource, 
		const ResourceType type, 
		const std::wstring name)
	{
		ResourceMap resourceType = resourceTypes[type];
		ResourceMap::iterator resourceFile = resourceType.find(name);

		if (resourceFile == resourceType.end())
		{
			Log::Warning(L"Resource '"
				+ name
				+ L"' (resouce path: '"
				+ ResourceDefinition::Get(type)->path
				+ L"') not found mapping resource type (resource type: '"
				+ ResourceDefinition::Get(type)->name
				+ L"')");

			if (!MapResourceType(type))
			{
				return false;
			}

			resourceFile = resourceType.find(name);
			 
			if (resourceFile == resourceType.end())
			{
				Log::Error(L"Resource '"
					+ name
					+ L"' (resouce path: '"
					+ ResourceDefinition::Get(type)->path
					+ L"') not found (again)");

				return false;
			}
		}

		FileDefinition* file = &resourceFile->second;
		
		return LoadResourceFile(
			resource,
			type,
			&resourceFile->second);
	}

	bool Resource::Interface::SaveSettings(
		ResourceBase* const resource, 
		const std::wstring name)
	{
		FileDefinition fileDefinition(
			MakeResourceFilePath(ResourceType::Settings, name)
		);

		return SaveResourceFile(
			resource,
			ResourceType::Settings,
			&fileDefinition);
	}

	bool Resource::Interface::LoadSettings(
		ResourceBase* const resource, 
		const std::wstring name)
	{
		FileDefinition fileDefinition(
			MakeResourceFilePath(ResourceType::Settings, name)
		);

		return LoadResourceFile(
			resource,
			ResourceType::Settings,
			&fileDefinition);
	}

	Static::Resource Resource::Interface::GetStaticResource(
		const Static::Id type)
	{
		return GetStaticResource(
			Static::Translate(type)
		);
	}

	Static::Resource Resource::Interface::GetStaticResource(
		const std::filesystem::path resource)
	{
		Static::Resource result;
		FileDefinition file(resource);

		if (!file.doesExists() || !file.resetSize() || file.size == 0)
		{
			Log::Error(L"Static resource file not found or is empty '"
				+ resource.filename().wstring()
				+ L"' (resource path: '"
				+ Static::GetPath() + L"')");

			return { };
		}

		FileReadPipe frp(&file);

		if (!frp.isValid())
		{
			Log::Error(L"Failed to open static resource '"
				+ resource.filename().wstring()
				+ L"' (resource path: '"
				+ Static::GetPath()
				+ L"') "
				+ MakeErrorMessageError()
			);

			return { };
		}

		result.first = new char[file.size];
		result.second = file.size;

		if (!frp.readContentForce(
				result.first,
				result.second))
		{
			Log::Error(L"Failed to read static resource '"
				+ resource.filename().wstring()
				+ L"' (resource path: '"
				+ Static::GetPath()
				+ L"') [maybe invalid] "
				+ MakeErrorMessageError()
			);

			delete[] result.first;
			return { };
		}

		Log::Information(L"Resource '"
			+ resource.filename().wstring()
			+ L"' read (resource path: '"
			+ Static::GetPath() + L"')");

		return result;
	}

	const std::wstring Interface::GetResourcePath()
	{
		return RESOURCE_PATH;
	}

	const std::wstring Interface::MakeResourceTypePath(
		const ResourceType type)
	{
		return GetResourcePath() + L"/" + ResourceDefinition::Get(type)->path;
	}
	
	const std::wstring Interface::MakeResourceFilePath(
		const ResourceType type, 
		const std::wstring name)
	{
		if (ResourceDefinition::Get(type)->hasExtension)
		{
			return MakeResourceTypePath(type) + L"/" + name + ResourceDefinition::Get(type)->extension;
		}
		else
		{
			return MakeResourceTypePath(type) + L"/" + name; // add extension
		}
	}
}
