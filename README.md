# Minimal example for to filter only the reads with less than 1000N

Just adapt the "filter" function in ["filter.hpp"](https://github.com/GDelevoye/Example-.bam-filter-C-/blob/main/filter.hpp) to make other kind of filtering :

```C++
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
```

# Compilation

## Ubuntu

I compiled it with g++11 under Ubuntu 22.04 LTS

```
g++ main.cpp filter.hpp lyra.hpp cigarfilter.hpp -o cigarfilter
```

## Others

The code should in theory work with any other decently-modern C++ compiler too

# Usage (.sam file)

```console
delevoye@Dell-G15-5530:~/GitHub/minimal_1000N$ g++ main.cpp filter.hpp lyra.hpp cigarfilter.hpp -o max1000N
main.cpp: In function ‘int main(int, char**)’:
main.cpp:33:66: warning: ‘std::string lyra::detail::result_base::errorMessage() const’ is deprecated [-Wdeprecated-declarations]
   33 |     std::cerr << "Error in command line: " << result.errorMessage()
      |                                               ~~~~~~~~~~~~~~~~~~~^~
In file included from main.cpp:7:
lyra.hpp:446:36: note: declared here
  446 |         [[deprecated]] std::string errorMessage() const { return message(); }
      |                                    ^~~~~~~~~~~~
delevoye@Dell-G15-5530:~/GitHub/minimal_1000N$ # Let's run it with '-p true', which will output the header lines 
delevoye@Dell-G15-5530:~/GitHub/minimal_1000N$ cat mocked.sam | ./max1000N -p true
@header
@ header ...
@  Some other header
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	55000	60	134S169M	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	1000N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	999N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	988N100M1N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	988N100M2N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	1N100M2N	otherfields...
delevoye@Dell-G15-5530:~/GitHub/minimal_1000N$ # Let's run it with '-p false', which will hide the header lines
delevoye@Dell-G15-5530:~/GitHub/minimal_1000N$ cat mocked.sam | ./max1000N -p false
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	55000	60	134S169M	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	1000N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	999N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	988N100M1N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	988N100M2N	otherfields...
9ae8a1cd-1a71-40d1-ab06-f1567ec79f9a	0	chr7	53644	60	1N100M2N	otherfields...
delevoye@Dell-G15-5530:~/GitHub/minimal_1000N$ 
```

# Usage (.bam files input and .bam files output)

```bash
samtools view -h ./input.bam | ./max1000N | samtools view -bS > filtered_output.bam
```bash

# Important notes

- The .bam file is assumed to be correct, and have more than 5 fields. No checking is made
- If your .bam is incorrect, you can have "core dump (segmentation fault)" errors, because it counts the number of tabs (i.e '\n') in each line to take the right CIGAR field
