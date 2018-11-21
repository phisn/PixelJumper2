#include "ResourceDevice.h"

namespace
{
	std::map<::Resource::File, std::pair<void*, sf::Uint32> > fileStreams;

	bool createResource(
		::Resource::File resource)
	{
		std::ifstream file(
			Resource::Manager::translate(resource),
			std::ios::binary | std::ios::in | std::ios::ate
		);

		if ( file.fail() )
		{
			return false;
		}

		const sf::Uint32 size = file.tellg();
		file.clear();
		file.seekg(0, std::ios::beg);

		char* buffer = new char[size];
		file.read(
			buffer,
			size);

		if ( file.fail() )
		{
			return false;
		}

		fileStreams[resource] = std::pair<void*, sf::Uint32>(buffer, size);

		return true;
	}

	_Ret_maybenull_
	std::pair<void*, sf::Uint32>* acquireResource(
		::Resource::File resource)
	{
		decltype(fileStreams)::iterator it = fileStreams.find(resource);

		if (it == fileStreams.end())
		{
			if ( createResource(resource) )
			{
				return &fileStreams[resource];
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return &it->second;
		}
	}
}

namespace Device
{
	bool Resource::initialize()
	{
		for (int resource = 0; resource < (int) ::Resource::File::_Size; ++resource)
			if (!_EXISTS(
				::Resource::Manager::translate( (::Resource::File) resource ).c_str()
			))
			{
				return false;
			}

		return true;
	}

	sf::MemoryInputStream Resource::load(
		const ::Resource::File resource)
	{
		sf::MemoryInputStream mis;

		std::pair<void*, sf::Uint32>* data = acquireResource(resource);
		if (data == NULL)
		{
			MessageBox(NULL, L"Unable to load resource", L"Error", MB_OK);
		}

		mis.open(data->first, data->second);
		return mis;
	}
}
