#ifndef PLUM_ANIMATION_H
#define PLUM_ANIMATION_H

namespace Plum
{
	class AnimationParser
	{
		private:
			std::string script;
			bool visible;
			int frame;
			int wait;
			unsigned int position;
		public:
			AnimationParser();

			void reset();
			std::string getScript();
			void setScript(std::string script);

			int getFrame();
		private:
			int parseNumber();
		public:
			void update();
	};

	class AnimationInfo
	{	
		public:
			// Maps direction name to a script
			std::map<std::string, std::string> scripts;

			AnimationInfo()
			{
			}
	};
}
#endif