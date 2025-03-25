#ifndef SIG_HPP
# define SIG_HPP

# include "Log.hpp"
# include <signal.h>
# include <string.h>
# include <string>
# include <unistd.h>

class Sig
{
  public:
	static bool quit;
	static void SigHandler(int sig);
	static void Init(void);
	static void Exit(void);
};

#endif /* SIG_HPP */
