#ifndef __MPPA_UTILS_H
#define __MPPA_UTILS_H

#include <iostream>
#include <chrono>

struct work_area_t {
	int x_init,
		y_init,
		x_final,
		y_final;
	int dist_to_border[4];
};

std::chrono::time_point<std::chrono::steady_clock> mppa_master_get_time(void);
std::chrono::time_point<std::chrono::steady_clock> mppa_slave_get_time(void);
double mppa_master_diff_time(std::chrono::time_point<std::chrono::steady_clock> begin, std::chrono::time_point<std::chrono::steady_clock> end);
double mppa_slave_diff_time(std::chrono::time_point<std::chrono::steady_clock> begin, std::chrono::time_point<std::chrono::steady_clock> end);

#endif // __MPPA_UTILS_H