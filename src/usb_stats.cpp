/*
 * Copyright (c) 2012 Adrien Guinet <adrien@guinet.me>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 	Redistributions of source code must retain the above copyright notice, this
 * 	list of conditions and the following disclaimer.
 * 
 * 	Redistributions in binary form must reproduce the above copyright notice,
 * 	this list of conditions and the following disclaimer in the documentation
 * 	and/or other materials provided with the distribution.
 * 
 * 	Neither the name of HPCProject, Serge Guelton nor the names of its
 * 	contributors may be used to endorse or promote products derived from this
 * 	software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <usbtop/usb_stats.h>
#include <usbtop/tools.h>

#include <sys/time.h>

// TODO: 
//  * we need to do some real tests & math here to have accurate values.. !

double usbtop::Stats::_t0 = 0;

usbtop::Stats::Stats():
	_nbytes(0),
	_tN(0),
	_nsamples(0),
	_inst_data(LIVE_SAMPLE_COUNT),
	_last_inst_bw(0.0),
	_stats_window(1.0)
{
}

void usbtop::Stats::init()
{
	_t0 = usbtop::tools::get_current_timestamp();
}

void usbtop::Stats::push(double timestamp, size_t spacket)
{
	_nsamples++;
	_nbytes += spacket;

	double first_ts = timestamp-_stats_window;

	// Remove oldest samples
	
	_inst_data.push_back(sample_t(timestamp, spacket));

	if (timestamp < _tN+0.2) {
		return;
	}

	_tN = timestamp;

	boost::circular_buffer<sample_t>::iterator it;
	boost::circular_buffer<sample_t>::iterator it_last_rem;
	bool to_rem = false;
	for (it = _inst_data.begin(); it != _inst_data.end(); it++) {
		if (it->first >= first_ts) {
			break;
		}
		to_rem = true;
		it_last_rem = it;
	}
	if (to_rem) {
		it_last_rem++;
		_inst_data.erase(_inst_data.begin(), it_last_rem);
	}
	// Compute instant bw at this instant
	size_t tsize = 0.0;
	{
		boost::circular_buffer<sample_t>::const_iterator it;
		for (it = _inst_data.begin(); it != _inst_data.end(); it++) {
			tsize += it->second;
		}
	}
	const double first_ts_buf = _inst_data.front().first;
	if (timestamp == first_ts_buf) {
		_last_inst_bw = 0.0;
	}
	else {
		_last_inst_bw = ((double)tsize)/(timestamp-first_ts_buf);
	}
}

double usbtop::Stats::bw_instant() const
{
	double last_ts_packet, last_inst_bw;
	last_ts_packet = _tN;
	last_inst_bw = _last_inst_bw;

	double cur_ts = tools::get_current_timestamp();
	if (cur_ts >= last_ts_packet+_stats_window) {
		// No packet in current window. Returns 0
		return 0.0;
	}

	return last_inst_bw;
}

double usbtop::Stats::bw_mean() const
{
	return (double)_nbytes/(_tN-_t0);
}
