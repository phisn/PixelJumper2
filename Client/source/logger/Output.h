#pragma once

#include <Client/source/logger/LoggerLevel.h>

#include <string>
#include <vector>

namespace Log
{
	class Output
	{
	public:
		typedef void (*Out)(const std::wstring);

		Output(
			const Out out,
			const Level importance)
			:
			out(out),
			importance(importance)
		{
		}

		static void Add(
			const Out out,
			const Level importance = Log::Level::Error)
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
	public:
		static std::vector<Output> outputs;
		static std::wstring path;

		const Out out;
		const Level importance;
	};
}
