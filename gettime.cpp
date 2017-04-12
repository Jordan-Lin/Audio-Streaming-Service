/*------------------------------------------------------------------------------
-- SOURCE FILE: gettime.cpp
--
-- PROGRAM: Comm Audio
--
-- FUNCTIONS:   getCurretnTime();
--              getDuration(Time begin, Time end);
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--           Justen DePourcq
--
-- PROGRAMMER: Brody McCrone
--
-- NOTES:
--  get time functions are used for finding the lengths of a song in a Time object
------------------------------------------------------------------------------*/
#include "gettime.h"

/*------------------------------------------------------------------------------
-- FUNCTION: getCurrentTime
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--           Justen DePourcq
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: Time getCurrentTime()
--
-- PARAMETERS: N/A
--
-- RETURNS: Time
--
-- NOTES:
--  get the current time.
------------------------------------------------------------------------------*/
Time getCurrentTime() {
    return std::chrono::high_resolution_clock::now();
}

/*------------------------------------------------------------------------------
-- FUNCTION: getDuration
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--           Justen DePourcq
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: int getDuration()
--
-- PARAMETERS: N/A
--
-- RETURNS: int
--
-- NOTES:
--  gets the duration of time based on an start and end time
------------------------------------------------------------------------------*/
int getDuration(Time begin, Time end) {
    std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - begin);
    return timeSpan.count() / 1000 / 1000 / 1000;
}
