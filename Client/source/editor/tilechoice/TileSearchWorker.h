#pragma once

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/tile/TileTemplateFactory.h>

#include <Client/source/shared/tiles/TileDescription.h>

namespace Editor
{
	class TileSearchWorker
	{
	public:
		bool operator()(
			const TileSearchInput* const input,
			TileSearchOutput* const output)
		{
			this->input = input;
			this->output = output;

			output->tiles.clear();

			for (int i = 0; i < (int)Shared::TileID::_Length; ++i)
			{
				const Shared::TileDescription* const description = Shared::TileDescription::Find(
					(Shared::TileID) i
				);

				// TODO: find in name or in info?
				if (input->search.empty() || matchTileName(description->name))
				{
					output->tiles.push_back(
						TileTemplateFactory::GetTileTemplate((Shared::TileID) i)
					);
				}
			}

			return true;
		}

	private:
		bool matchTileName(const std::wstring& name)
		{
			return std::search(
				name.cbegin(), name.cend(),
				input->search.cbegin(), input->search.cend(),
				[](const wchar_t w1, const wchar_t w2) -> bool
				{
					return towlower(w1) == towlower(w2);
				}
			) != name.cend();
		}

		const TileSearchInput* input;
		TileSearchOutput* output;
	};
}
