#pragma once

namespace ams
{
	class ArgManager
	{
		std::deque<std::string> args;
		std::map<char, bool> singleoptionsmap;
		std::map<char, std::string> pairoptionsmap;

		// Meyers singleton - hence private constructor:
		ArgManager(std::string singleOptions, std::string pairOptions, int argc, char**&argv)
		{
			// store the list of arguments from argv[1] onwards:
			while (--argc)
			{
				args.push_back(*++argv);
			}

			// initialise the "single options" - e.g. like "-n" without any parameters
			for (auto c : singleOptions)
			{
				singleoptionsmap.insert(std::make_pair(c, false));
			}
			// initialise the "pair options" - the ones with a single letter option followed by a parameter like "-f filename"
			for (auto c : pairOptions)
			{
				pairoptionsmap.insert(std::make_pair(c, ""));
			}

			bool expectingParam = false;
			char optionexpecting;
			for (auto p : args)
			{
				// if we have already had a pair options option character - like -f but we are expecting a parameter
				// then store the parameter here:
				if (expectingParam)
				{
					pairoptionsmap[optionexpecting] = p;
					expectingParam = false;
				}
				else if (p[0] == '-')
				{
					if (p.length() == 1)
					{
						// ignore a single minus
					}
					else if (p.length() == 2)
					{
						// a one character option immediately after a minus
						auto it = singleoptionsmap.find(p[1]);
						if (it != singleoptionsmap.end())
						{
							// we found a 'single' option - set it:
							it->second = true;
						}
						else
						{
							// if this is an option that needs a parameter, set the 
							// expecting param flag:
							auto it = pairoptionsmap.find(p[1]);
							if (it != pairoptionsmap.end())
							{
								expectingParam = true;
								optionexpecting = p[1];
							}
							else
							{
								throw QException("invalid option: '" + p[1] + std::string("'"));
							}
						}
					}
					else
					{
						// length is more than two so combined single options are allowed:
						for (auto q : p.substr(1))
						{
							auto r = singleoptionsmap.find(q);
							if (r == singleoptionsmap.end())
							{
								throw QException(std::string("") + "invalid option '" + q + std::string("'"));
							}
							singleoptionsmap[q] = true;
						}
					}
				}
				else
				{
					throw QException(p + ": unexpected argument. Option expected.");
				}
			}
			if (expectingParam)
			{
				throw QException(std::string("") + "parameter expected after option '" + optionexpecting + '\'');
			}
		}

	public:
		static ArgManager& GetInstance(std::string singleoptions, std::string pairoptions, int argc, char**&argv)
		{
			// static instance member - construct on first use. Further calls to get instance return this static
			// instance without constructing it. This is a Meyers singleton and thanks to "magic statics" is thread
			// safe from VS2015. 
			static ArgManager instance(singleoptions, pairoptions, argc, argv);
			return instance;
		}

		bool TestOpt(char c)
		{
			bool result;
			auto p = singleoptionsmap.find(c);
			if (p == singleoptionsmap.end())
			{
				throw QException(std::string("") + "invalid option '" + c + "' requested");
			}
			else
			{
				result = p->second;
			}
			return result;
		}

		std::string GetOpt(char c)
		{
			auto p = pairoptionsmap.find(c);
			if (p == pairoptionsmap.end())
			{
				throw QException(std::string("") + "invalid option '" + c + "' requested");
			}
			return p->second;
		}
	};
}