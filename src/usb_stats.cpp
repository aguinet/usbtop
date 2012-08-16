#include <usbtop/usb_stats.h>
#include <usbtop/tools.h>

#include <sys/time.h>

// TODO: 
//  * we need to do some real tests & math here to have accurate values.. !
//  * that lock should disappear
//  * for now, every GUI check values at a periodic time. We should have a signal/slot
//    system (and check its performance...).

double usbtop::Stats::_t0 = 0;

usbtop::Stats::Stats():
	_nbytes(0),
	_tN(0),
	_nsamples(0),
	_inst_data(LIVE_SAMPLE_COUNT)
{
}

void usbtop::Stats::init()
{
	_t0 = usbtop::tools::get_current_timestamp();
}

void usbtop::Stats::push(double timestamp, size_t spacket)
{
	_tN = timestamp;
	_nsamples++;
	_nbytes += spacket;

	{
		boost::unique_lock<boost::shared_mutex> lock(_access);
		if (_inst_data.size() == LIVE_SAMPLE_COUNT) {
			_inst_data.pop_front();
		}
		_inst_data.push_back(sample_t(timestamp, spacket));
	}
}

double usbtop::Stats::bw_instant() const
{
	if (_inst_data.size() < 2) {
		return 0.0;
	}

	// Get current timestamp
	double cur_ts = tools::get_current_timestamp();
	const double last_ts = _inst_data.back().first;
	if (last_ts + 0.1 >= cur_ts) {
		cur_ts = last_ts;
	}
	const double lower_ts = cur_ts-1.0;
	double first_ts = 0.0;

	size_t tsize = 0.0;
	boost::circular_buffer<sample_t>::const_reverse_iterator it;
	{
		boost::shared_lock<boost::shared_mutex> lock(_access);
		for (it = _inst_data.rbegin(); it != _inst_data.rend(); it++) {
			if (it->first < lower_ts) {
				break;
			}
			first_ts = it->first;
			tsize += it->second;
		}
	}

	// Our data is too old !
	if (first_ts == 0.0) {
		return 0.0;
	}

	return (double)tsize/(cur_ts - first_ts);
}

double usbtop::Stats::bw_mean() const
{
	return (double)_nbytes/(_tN-_t0);
}
