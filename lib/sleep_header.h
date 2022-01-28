#ifndef SLEEP_HEADER_H
#define SLEEP_HEADER_H

//includes for the sleep
#include <thread>
#include <chrono>

#define sleep_for_millis(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));


#endif // SLEEP_HEADER_H
