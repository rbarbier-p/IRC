#ifndef TIMER_HPP
# define TIMER_HPP

# include <cstring>
# include <fstream>
# include <unistd.h>
# include <stdlib.h>
# include <iostream>

class Timer 
{
	private:
		const std::string	timer_tag_;
		const std::string	user_name_;
		const int			timer_time_;
		const int			started_time_;

	public:
		Timer(std::string timer_tag, std::string client_name, int delay_time, int started_time);
		~Timer();

		std::string	getTimerTag() const;
		std::string	getUserName() const;
		int 		getTimerTime() const;
		int			getStartedTime() const;
};

#endif
