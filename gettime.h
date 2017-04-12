#ifndef GETTIME_H
#define GETTIME_H

#include <chrono>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> Time;

Time getCurrentTime();
int getDuration(Time begin, Time end);


#endif
