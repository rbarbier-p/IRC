#include "Timer.hpp"

Timer::Timer()
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

void Timer::setTimerTag(std::string timer_tag)
{
	this->timer_tag_ = timer_tag;
}

void Timer::setUserName(std::string user_name)
{
	this->user_name_ = user_name;
}

void Timer::setTimerTime(int timer_time)
{
	this->timer_time_ = timer_time;
}

void Timer::setStartedTime(int started_time)
{
	this->started_time_ = started_time;
}
