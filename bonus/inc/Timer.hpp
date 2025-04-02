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
		std::string	timer_tag_;
		std::string	user_name_;
		int			timer_time_;
		int			started_time_;

	public:
		Timer();
		~Timer();

		std::string	getTimerTag() const;
		std::string	getUserName() const;
		int 		getTimerTime() const;
		int			getStartedTime() const;

		void setTimerTag(std::string timer_tag);
		void setUserName(std::string user_name);
		void setTimerTime(int timer_time);
		void setStartedTime(int started_time);
};

#endif
