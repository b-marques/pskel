//-------------------------------------------------------------------------------
// Copyright (c) 2015, Márcio B. Castro <marcio.castro@ufsc.br>
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-------------------------------------------------------------------------------

#include <mppa/osconfig.h>
#include <HAL/hal/core/mp.h>
#include "interface_mppa.h"
#include <time.h>

void set_path_name(char *path, char *template_path, int rx, int tag) {
  sprintf(path, template_path, rx, tag);
}

/**************************************
 * TIME
 **************************************/

//static uint64_t residual_error = 0;

struct timeval mppa_master_get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //std::cout << tv.tv_sec << "|" << tv.tv_usec << "|" << tv.tv_sec+((double)(tv.tv_usec/1000000)) << std::endl;
  return tv; //tv.tv_sec+((double)(tv.tv_usec/1000000));
  // return tv;
  // get_time_of_day();
  // residual_error = t2 - t1;

}

struct timespec mppa_slave_get_time() {
  struct timespec request;

  clock_gettime(CLOCK_REALTIME, &request);
  return request;
  // uint64_t t1, t2;
  // t1 = clock_gettime();
  // t2 = clock_gettime();
  // residual_error = t2 - t1;
}

double mppa_master_diff_time(struct timeval begin, struct timeval end) {
    std::cout << begin.tv_sec << ", " << begin.tv_usec << std::endl;
    std::cout << end.tv_sec << ", " << end.tv_usec << std::endl;
    std::cout << (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) << std::endl;
  return (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
}
double mppa_slave_diff_time(struct timespec begin, struct timespec end) {
  return (end.tv_sec - begin.tv_sec) + ((end.tv_nsec - begin.tv_nsec)/BILLION);
}
