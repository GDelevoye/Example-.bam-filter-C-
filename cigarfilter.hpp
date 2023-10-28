#ifndef CIAGRFILTER_HPP
#define CIAGRFILTER_HPP

#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <string>

#include <stddef.h> // size_t

namespace cf{

struct CIGAR {
    size_t M = 0; /**< Number of Matched or mismatched nucleotides */
    size_t eq = 0; /**< Number of Matched nucleotides */
    size_t I = 0; /**< Number of Inserted nucleotides */
    size_t D = 0; /**< Number of Deleted nucleotides */
    size_t X = 0; /**< Number of Substituated nucleotides */
    size_t H = 0; /**< Number of Hard-clipped nucleotides */
    size_t S = 0; /**< Number of Soft-clipped nucleotides */
    size_t P = 0; /**< Number of Padded nucleotides */
    size_t N = 0; /**< Number of Skipped (or "Spliced") nucleotides
*
*       In cDNA-to-genome alignment, we may want to distinguish introns from deletions in exons.
*       We introduce openation 'N' to represent long skip on the reference sequence. Suppose the spliced alignment is:

        REF:AGCTAGCATCGTGTCGCCCGTCTAGCATACGCATGATCGACTGTCAGCTAGTCAGACTAGTC

        Read:         GTGTAACCC................................TCAGAATA

        where '...' on the read sequence indicates intron. The CIGAR for this alignment is : 9M32N8M. */

    bool empty = false; /**< Set to true if the CIGAR string is "*" */
    bool error = false; /**< @warning This catches only a very limited subset of errors. When true, the CIGAR is invalid.
                          * It is set to true if at least one group is not in {"X","M","N","P","H","S","=","D","I"} */

    std::vector<std::tuple<char,size_t>> groups; /**< Each tuple is made of (group_type,group_size) */

    std::string as_string = ""; /**< The original CIGAR string. */
};


/**
 * @brief Compilation-time optimization so that we don't compute pows of floats (or double) at runtime
 * @see "listdigits_to_number()"
 * @note Although it is hardcoded, it is technically impossible for a group to be >= 1000000000 nt long
 */
static size_t pow10[10 /**< Which exponent */] = {
        1, 10, 100, 1000, 10000,
        100000, 1000000, 10000000, 100000000, 1000000000,
};


inline size_t listdigits_to_number(std::vector<int> listdigits) {
  size_t final_digit = 0;
  size_t lsize = listdigits.size();

  for (size_t i = 0; i < lsize; i++) {
    final_digit += listdigits[i] * pow10[lsize - i - 1];
  }
  return final_digit;
};

CIGAR get_CIGAR(const std::string &inputstring) {
  int tab_count = 0;
  size_t i = 0;
  CIGAR cigar{};

  std::vector<int> listdigits;

  while (tab_count !=
         5) {  // Go until we've encountered the 5th tab character ...
    if (inputstring[i] == '\t') {
      tab_count++;
    }
    i++;
  }

  // ... After the 5th tab, that's the CIGAR ...

  // ... It can be empty ...
  char c = inputstring[i];

  if (c == '*') {  // ... in which case we can return ...
    cigar.empty = true;
    cigar.as_string = "*";
    return cigar;
  }

  // ... Most often however, it will rather contain one or more groups :

  while (c != '\t') {  // While a part of the CIGAR remains to be read
    cigar.as_string.push_back(c);
    if (isdigit(c)) {                  // If the character is a number :
      int this_integer = (int)c - 48;  // ASCII to digit
      listdigits.push_back(this_integer);
    }

    else {  // End of the CIGAR group
      int number = listdigits_to_number(listdigits);
      // Using a hashtable rather than a switch is preferable for maintenance
      // but not for performances
      switch (c) {
        case 'M':
          cigar.M += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('M', number));
          break;
        case '=':
          cigar.eq += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('=', number));
          break;
        case 'I':
          cigar.I += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('I', number));
          break;
        case 'D':
          cigar.D += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('D', number));
          break;
        case 'X':
          cigar.X += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('X', number));
          break;
        case 'H':
          cigar.H += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('H', number));
          break;
        case 'S':
          cigar.S += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('S', number));
          break;
        case 'P':
          cigar.P += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('P', number));
          break;
        case 'N':
          cigar.N += number;
          cigar.groups.push_back(std::make_tuple<char, size_t>('N', number));
          break;
        default:
          // WARNING : This does not catch all errors at all !!
          cigar.error = true;
          break;
      }
      listdigits.clear();
    }
    i++;
    c = inputstring[i];
    // end of while loop
  }  // We have reached the end of the CIGAR
  return cigar;
};  // end of the function get_cigar()

}

#endif
