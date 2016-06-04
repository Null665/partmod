/**
 * Covert string to number and number to string
 * Normally std::to_string could be used but it doesn't work on mingw (g++ 4.9.2)
 * also, sometimes we need to convert between different number bases
*/
#ifndef NUMSTR_H
#define NUMSTR_H

#include <string>
#include <sstream>
#include <iomanip>

/// \defgroup numbase Number base to use when converting from/to string
//@{
#define STR_OCT                   8 //!< Octal string
#define STR_DEC                  10 //!< Decimal string
#define STR_HEX                  16 //!< Hex string
//@}


/** \brief convert number to string
 *
 * \param number     number to_convert
 * \param base       Number base to use. See numbase
*/
template<typename T> std::string to_string (T number, unsigned base=10)
{
  std::stringstream ss;

  switch (base)
  {
    case STR_OCT:
      ss<<std::oct;
      break;
    case STR_HEX:
      ss<<std::hex;
      break;
  }
  ss <<number;
  return ss.str();
}


//
// HACK: If the bug is present, use the above implementation for to_string with base 10
// else, the to_string is defined in std namespace, so do nothing.
//
#if defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF) && _GLIBCXX_HAVE_BROKEN_VSWPRINTF==1
  template<typename T> std::string to_string (T numebr)
  {
      return to_string (numebr,STR_DEC);
  }
#else
 using std::to_string;
#endif


#endif // NUMSTR_H
