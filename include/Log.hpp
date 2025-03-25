#ifndef LOG_HPP
# define LOG_HPP

# include <ctime>
# include <iostream>
# include <sstream>

class Log
{
  public:
	Log(void);
	template <typename T> Log &operator<<(T const &value)
	{
		buffer_ << value;
		return (*this);
	}
	virtual ~Log(void);

	class Notice;
	class Info;
	class Err;

  private:
	std::time_t start_time_;
	std::ostringstream buffer_;
};

class Log::Notice : public Log
{
  public:
	Notice(void);
	~Notice(void);
};

class Log::Info : public Log
{
  public:
	Info(void);
	~Info(void);
};

class Log::Err : public Log
{
  public:
	Err(void);
	~Err(void);
};

#endif /* LOG_HPP */
