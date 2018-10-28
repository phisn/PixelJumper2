#include "TemplateManager.h"

namespace
{
	std::map<
		Game::Tile::Id,
		Editor::TileTemplate*> templates;
}

namespace Editor
{
	TileTemplate* TemplateManager::getTemplate(
		const Game::Tile::Id tileId)
	{
		decltype(templates)::iterator iterator
			= templates.find(tileId);
		if (iterator == templates.end())
		{
			return NULL;
		}
		else
		{
			return iterator->second;
		}
	}

	void TemplateManager::registerTemplate(
		const Game::Tile::Id tileId,
		Editor::TileTemplate* templ)
	{
		templates[tileId] = templ;
	}

	void TemplateManager::clearTemplates()
	{
		templates.clear();
	}
}
