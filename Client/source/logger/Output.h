#pragma once

#include <Client/source/logger/Level.h>

#include <string>
#include <vector>

namespace Logger
{
	class Output
	{
	public:
		typedef void (*Out)(const std::wstring);

		static void Add(
			const Out out,
			const Level importance)
		{
			outputs.emplace_back(
				out,
				importance);
		}

		static const std::vector<Output>* getOutputs()
		{
			return &outputs;
		}

		void write(
			const std::wstring message) const
		{
			out(message);
		}

		Level getImportance() const
		{
			return importance;
		}

		static const Out FILE_OUT;
		static const Out CONSOLE_OUT;

		static void setPath(
			std::wstring path)
		{
			Output::path = path;
		}
	private:
		static std::vector<Output> outputs;
		static std::wstring path;

		Output(
			const Out out,
			const Level importance)
			:
			out(out),
			importance(importance)
		{
		}

		const Out out;
		const Level importance;
	};
}
