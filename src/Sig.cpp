#include "Sig.hpp"

bool Sig::quit(false);

void Sig::SigHandler(int sig)
{
	Log::Info() << "Got signal \"" << strsignal(sig) << "\" ...";
	quit = true;
}

void Sig::Init(void)
{
	signal(SIGINT, SigHandler);
	signal(SIGQUIT, SigHandler);
	signal(SIGTERM, SigHandler);
}

void Sig::Exit(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
}
