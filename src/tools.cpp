#include <usbtop/tools.h>
#include <sys/time.h>
#include <cstdlib>

double usbtop::tools::get_current_timestamp()
{
	struct timeval curt;
	gettimeofday(&curt, NULL);
	return (double)curt.tv_sec + ((double)curt.tv_usec)/1000000.0;
}
