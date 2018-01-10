#ifndef __MPPA_UTILS_H
#define __MPPA_UTILS_H

#include <iostream>
#include <chrono>


#define BILLION 1E9

std::chrono::time_point<std::chrono::steady_clock> mppa_master_get_time(void);
std::chrono::time_point<std::chrono::steady_clock> mppa_slave_get_time(void);
double mppa_master_diff_time(std::chrono::time_point<std::chrono::steady_clock> begin, std::chrono::time_point<std::chrono::steady_clock> end);
double mppa_slave_diff_time(std::chrono::time_point<std::chrono::steady_clock> begin, std::chrono::time_point<std::chrono::steady_clock> end);

#endif // __MPPA_UTILS_H