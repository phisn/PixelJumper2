#pragma once

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/pipes/FilePipe.h>

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/ResourceType.h>
#include <Client/source/resource/StaticResource.h>

#include <SFML/Main.hpp>

namespace Resource
{
	namespace Interface
	{
		bool Initialize();
		void Uninitalize();

		void MapResources();
		void MapResourceType(const ResourceType type);

		bool SaveResource(
			ResourceBase* const resource,
			const ResourceType type,
			const std::wstring name);
		bool LoadResource(
			ResourceBase* const resource,
			const ResourceType type,
			const std::wstring name);

		bool SaveSettings(
			ResourceBase* const resource,
			const std::wstring name);
		bool LoadSettings(
			ResourceBase* const resource,
			const std::wstring name);

		Static::Resource GetStaticResource(
			const Static::ID type);
		Static::Resource GetStaticResource(
			const std::filesystem::path resource);

		const std::wstring GetResourcePath();
		const std::wstring MakeResourcePath(const ResourceType type);
		const std::wstring MakeFullResourcePath(
			const ResourceType type,
			const std::wstring name);
	}
	/*
	typedef std::map<std::wstring, FileDefinition> SubResources; // name, data
	typedef std::map<ResourceType, SubResources> MapedResources; // type, list

	namespace _N_Interface
	{
		bool Initialize();

		bool RemapAllFiles();
		bool RemapFiles(const ResourceType type);

		// TODO: rework internals

		bool WriteResource(
			ResourceBase* const resource,
			const ResourceType type,
			const std::wstring name);
		bool ReadResource(
			ResourceBase* const resource,
			const ResourceType type, 
			const std::wstring name);

		bool ReadRawResource(
			ResourceBase* const resource,
			const std::filesystem::path path);
		bool ReadRawResource(
			ResourceBase* const resource,
			const FileDefinition* const file);
		bool WriteRawResource(
			ResourceBase* const resource,
			const std::filesystem::path path);
		bool WriteRawResource(
			ResourceBase* const resource,
			FileDefinition* const file);
		
		Static::Resource GetStaticResource(
			const Static::ID type);
		Static::Resource GetStaticResource(
			const std::filesystem::path resource);

		const std::wstring GetResourcePath();
		const std::wstring MakeResourcePath(const ResourceType type);*/
}
