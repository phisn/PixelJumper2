#pragma once

#include <vector>

namespace Editor
{
	class ContextMenuResult
	{
	public:
		virtual void onTrigger() = 0;

		const std::wstring* getText() const
		{
			return &text;
		}
	protected:
		std::wstring text;
	};
	
	struct ContextMenuResults
	{
		std::vector<ContextMenuResult> results;
	};
}