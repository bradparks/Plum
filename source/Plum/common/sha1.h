#pragma once
namespace Plum
{
	class SHA1
	{
		public:
			static void digestString(const std::string& str, std::string& hash);
			static bool digestFile(const std::string& str, std::string& hash);
	};
}