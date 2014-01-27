#ifndef USBTOP_SHOULD_STOP_H
#define USBTOP_SHOULD_STOP_H

namespace usbtop {

class ShouldStop
{
public:
	static void init()
	{
		_value = false;
	}

	static bool value()
	{
		return _value;
	}

	static void stop()
	{
		_value = true;
	}

private:
	static bool _value;
};

}

#endif
