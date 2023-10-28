#ifndef FILTER_HPP
#define FILTER_HPP

#include "cigarfilter.hpp"

namespace cf{

bool filter(CIGAR &cigar, std::string &samline) {
  if (!(cigar.error) && !(cigar.empty)) {
	if(cigar.N <= 1000){
		return true;
	}
	else{
		return false;
	}
  }
  return false;
}

}

#endif
