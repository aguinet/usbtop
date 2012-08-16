#ifndef USBTOP_USB_STATS_H
#define USBTOP_USB_STATS_H

#include <cassert>
#include <cstddef>

#include <boost/circular_buffer.hpp>
#include <boost/thread/shared_mutex.hpp>

#define LIVE_SAMPLE_COUNT 128

namespace usbtop {

class Stats: boost::noncopyable
{
	typedef std::pair<double, size_t> sample_t;
public:
	Stats();

public:
	static void init();
	void push(double timestamp, size_t spacket);

public:
	double bw_instant() const;
	double bw_mean() const;

private:
	// Global stats
	size_t _nbytes;
	double _tN;
	size_t _nsamples;

	// "Instantaneous" stats
	boost::circular_buffer<sample_t> _inst_data;
	mutable boost::shared_mutex _access;

	// Timestamp when the application is launched. Used as t0
	static double _t0;
};

class UsbStats: boost::noncopyable
{
public:
	// The order of this enum is important !! (cf. UsbBus::push)
	typedef enum {
		from_device = 0,
		to_device
	} direction_type;

public:
	inline void push(const double ts, const size_t size, const direction_type dir)
	{
		assert((dir == direction_type::from_device) || (dir == direction_type::to_device));
		if (dir == direction_type::from_device) {
			_from_device.push(ts, size);
		}
		else {
			_to_device.push(ts, size);
		}
	}

public:
	Stats const& stats_to_device() const { return _to_device; }
	Stats const& stats_from_device() const { return _from_device; }

private:
	Stats _from_device;
	Stats _to_device;
};

}

#endif
