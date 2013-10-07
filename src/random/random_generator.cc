/*
 * scrm is an implementation of the Sequential-Coalescent-with-Recombination Model.
 * 
 * Copyright (C) 2013 Paul R. Staab, Sha (Joe) Zhu and Gerton Lunter
 * 
 * This file is part of scrm.
 * 
 * scrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "random_generator.h"
#include <iostream>

// Sample from a unit exponential distribution
// Unit tested
double RandomGenerator::sampleUnitExponential(void) {
  double exposample = -ff.fastlog( sample() );
  return exposample;
}

// Sets new seed
void RandomGenerator::set_seed(const size_t &seed){
  this->seed_ = seed;
}

// Initialize unit_exponential; must be called when the random generator is up and running
void RandomGenerator::initializeUnitExponential() {
  this->unit_exponential_ = sampleUnitExponential();
}

// Samples from an exponential distribution 
// Unit tested 
double RandomGenerator::sampleExpo(double lambda){
  return sampleUnitExponential() / lambda;
}

// Samples from an exponential distribution; return -1 if beyond limit
// If a limit is known, this version is faster than the standard one
// Unit tested
double RandomGenerator::sampleExpoLimit(double lambda, double limit){
  return sampleExpoExpoLimit(lambda, 0, limit);
}

// Samples waiting time, with limit, for a process with an exponentially changing rate:
//  rate(t) = b exp( c t )
// This code allows c=0, and falls back to a standard exponential if so
// For a p ~ unif(0,1), a waiting time sample is (1/c) log[ 1 - (c/b) log p ]
// It returns -1 if no event occurred; this can happen even if limit == +infinity (if c<0)
double RandomGenerator::sampleExpoExpoLimit(double b, double c, double limit){
  assert (b>0);
  assert (limit>=0);
  // for any c, the no-event condition (t=maximum waiting time) is
  //  (b/c) (exp(c t)-1) < -log p
  // For c<0 and c>0 respectively this becomes
  //  -c log p < b (exp(c t)-1)
  //  -c log p > b (exp(c t)-1)  resp.
  // These are implied by the conditions
  //  -c log p < b (exp_up(c t)-1)    [c<0]
  //  -c log p > b (exp_lo(c t)-1)    [c>0]
  // where exp_up and exp_lo are upper and lower bounds for the exp(x) function resp.
  if (c < 0) {
    double c_logp_limit = b*(ff.fastexp_lo(c*limit)-1);  // negative
    if (c*unit_exponential_ < c_logp_limit) {
      unit_exponential_ -= c_logp_limit / c;
      return -1;
    }
    double y = 1.0 + c*unit_exponential_ / b;
    unit_exponential_ = sampleUnitExponential();
    if (y <= 0.0) return -1; // no event at all
    y = ff.fastlog( y )/c;       
    if (y > limit) return -1;  // the event time; can still be beyond limit
    return y;
  } else if (c > 0) {
    double c_logp_limit = b*(ff.fastexp_up( c*limit )-1);  // positive
    if (c*unit_exponential_ > c_logp_limit) {
      unit_exponential_ -= c_logp_limit / c;
      return -1;
    }
    double y = ff.fastlog( 1.0 + c*unit_exponential_ / b ) / c;
    unit_exponential_ = sampleUnitExponential();
    if (y > limit) return -1;
    return y;
  } else {
    if (unit_exponential_ >= limit * b) {
      unit_exponential_ -= limit * b;
      return -1;
    } else {
      double result = unit_exponential_ / b;
      unit_exponential_ = sampleUnitExponential();
      return result;
    }
  }
}

// Uniformly samples a number out of 0, ..., range-1
// Unit tested 
int RandomGenerator::sampleInt(int range) {
  return(static_cast<int>(this->sample()*range));
}
