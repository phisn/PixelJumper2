#include "ResourceInterface.h"
#include "WorldResource.h"

#include "pipes/FilePipe.h"
#include "pipes/MemoryPipe.h"

#include <cassert>
#include <map>
#include <string>

namespace Resource
{
	const ResourceTypeDefinition WorldResourceDefinition
	{
		L"world resources",
		L"world",
		L"wrld"
	};
	
	const ResourceTypeDefinition SettingsResourceDefinition
	{
		L"app settings",
		L"settings",
		L"stg"
	};

	std::wstring primaryDirectory = L"resource";
	std::map<std::filesystem::path, std::vector<char>> cachedResources;

	std::wstring GetResourceErrorMessage();
	bool SaveResourceFromPipe(
		ResourceBase* resource,
		WritePipe* pipe);
	bool LoadResourceFromPipe(
		ResourceBase* resource,
		ReadPipe* pipe);
	bool EnforceDirectory(const std::filesystem::path& path);

	bool Interface::Initialize()
	{
		if (!EnforceDirectory(primaryDirectory))
		{
			Log::Error(L"failed to enforce primary directory");
			return false;
		}

		return true;
	}

	bool Interface::SaveResource(
		std::wstring filename, 
		ResourceBase* resource,
		const ResourceTypeDefinition& type)
	{
		std::filesystem::path path = Interface::GetResourcePath(filename, type);

		Log::Section section(L"saving file",
			path.c_str(), L"path",
			type.name, L"type");

		decltype(cachedResources)::iterator cachedResource = cachedResources.find(path);

		// update cache
		// cached resource wont be removed
		// if save failed
		if (cachedResource != cachedResources.end())
		{
			Log::Error(L"resource found in cache, updating");

			MemoryWritePipe pipe;
			
			if (!resource->save(&pipe))
			{
				Log::Error(L"failed to save resource");

				return false;
			}

			pipe.swap(cachedResource->second);

			std::ofstream file{ path };
			
			if (file.bad())
			{
				Log::Error(L"failed to open file after caching",
					GetResourceErrorMessage(), L"error");

				return false;
			}

			file.write(&cachedResource->second[0], cachedResource->second.size());
			file.close();

			if (file.bad())
			{
				Log::Error(L"failed to save file after caching",
					GetResourceErrorMessage(), L"error");

				return false;
			}

			return true;
		}

		FileWritePipe pipe{ path };

		if (!pipe.isValid())
		{
			Log::Error(L"failed to open file for save",
				GetResourceErrorMessage(), L"error");

			return false;
		}

		if (!SaveResourceFromPipe(
				resource,
				&pipe))
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
		std::filesystem::path path = Interface::GetResourcePath(filename, type);

		Log::Section section(L"loading file",
			path.c_str(), L"path",
			type.name, L"type");

		decltype(cachedResources)::iterator cachedResource = cachedResources.find(path);

		// load from cache
		if (cachedResource != cachedResources.end())
		{
			Log::Information(L"resource found in cache, loading");

			// do not have to check for validity because
			// memory pipes cant be invalid
			MemoryReadPipe pipe{ cachedResource->second };

			return LoadResourceFromPipe(
				resource,
				&pipe);
		}

		if (std::error_code error_code; !std::filesystem::exists(path, error_code))
		{
			if (error_code)
			{
				Log::Error(L"failed to check file existance",
					error_code.message(), L"error");
			}
			else
			{
				Log::Error(L"failed to open file, does not exist");
			}

			return false;
		}

		FileReadPipe pipe{ path };

		if (!pipe.isValid())
		{
			Log::Error(L"failed to open file",
				GetResourceErrorMessage(), L"error");

			return false;
		}

		if (!LoadResourceFromPipe(
				resource,
				&pipe))
		{
			Log::Error(L"load resource failed", 
				pipe.isValid(), L"valid",
				GetResourceErrorMessage(), L"error");
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
		std::filesystem::path path = Interface::GetResourcePath(filename, type);

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

	std::filesystem::path Interface::GetResourcePath(
		std::wstring filename,
		const ResourceTypeDefinition& type)
	{
		if (type.directory.size() != 0 && !EnforceDirectory(primaryDirectory + L"\\" + type.directory))
		{
			Log::Error(L"Failed to enforce directory",
				type.directory, L"type_directory",
				type.name, L"type_name",
				filename, L"filename",
				primaryDirectory, L"primary");
		}

		// primaryDirectory\[type_directory\]filename[.type_extension]
		return primaryDirectory + L"\\" 
			+ (type.directory.size() == 0
				? L""
				: type.directory + L"\\")
			+ filename
			+ (type.extension.size() == 0
				? L""
				: L"." + type.extension);
	}

	std::wstring GetResourceErrorMessage()
	{
		return std::wstring(_wcserror(errno));
	}

	bool SaveResourceFromPipe(
		ResourceBase* resource,
		WritePipe* pipe)
	{
		if (!resource->save(pipe))
		{
			Log::Error(L"failed to save resource");

			return false;
		}

		return false;
	}

	bool LoadResourceFromPipe(
		ResourceBase* resource,
		ReadPipe* pipe)
	{
		if (!resource->make(pipe))
		{
			Log::Error(L"failed to make resource");

			return false;
		}

		return true;
	}

	bool EnforceDirectory(const std::filesystem::path& path)
	{
		if (std::error_code error_code; !std::filesystem::exists(primaryDirectory, error_code))
		{
			Log::Section section{ L"creating missing directory", path, L"directory"};

			if (error_code)
			{
				Log::Error(L"Failed to check if directory exists",
					error_code.message(), L"message");

				return false;
			}
			else
			{
				if (!std::filesystem::create_directory(primaryDirectory, error_code))
				{
					Log::Error(L"Failed to create directory",
						error_code.message(), L"message");

					return false;
				}
			}
		}

		return true;
	}
}
