#pragma once


#include <sstream>
namespace SE
{
	namespace Debug
	{
		class CDebugConsole
		{
		public:
			enum class EConsoleStream
			{
				EndStream
			};

			void Render();
			void Clear();

			// printf alike
			CDebugConsole& operator()(const char* aFormat, ...);

			// std::cout alike
			template <class T>
			CDebugConsole& operator<<(const T& aType);
			// std::endl like
			CDebugConsole& operator<<(const EConsoleStream& aConsoleStreamEnum);
			// std::endl like
			CDebugConsole& operator<<(CDebugConsole& aDebugConsole);

		private:
			std::stringstream myStream;
			std::vector<char*> myStoredText;
		};

		// std::cout
		template<class T>
		inline CDebugConsole& CDebugConsole::operator<<(const T& aType)
		{
			myStream << aType;
			return *this;
		}

		template <class T>
		void Track(T*, const char* fileName)
		{
			Singleton<CDebugConsole>()("%s Allocated %u bytes for %s", fileName, sizeof(T), typeid(T).name());
		}
	}
}
