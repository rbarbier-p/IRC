#include "Timer.hpp"

Timer::Timer(std::string timer_tag, std::string user_name, int delay_time, int started_time) 
	: timer_tag_(timer_tag), user_name_(user_name), timer_time_(timer_time), started_time_(started_time)
{
}

Timer::~Timer()
{
}

std::string Timer::getTimerTag() const
{
	return this->timer_tag_;
}

std::string Timer::getUserName() const
{
	return this->user_name_;
}

int Timer::getTimerTime() const
{
	return this->timer_time_;
}

int Timer::getStartedTime() const
{
	return this->started_time_;
}
