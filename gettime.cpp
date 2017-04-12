#include "gettime.h"

Time getCurrentTime() {
    return std::chrono::high_resolution_clock::now();
}

int getDuration(Time begin, Time end) {
    std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - begin);
    return timeSpan.count() / 1000 / 1000 / 1000;
}
