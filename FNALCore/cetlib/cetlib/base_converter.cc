
#include "base_converter.h"
#include <stdexcept>
#include <algorithm>

using namespace cet;

const char* base_converter::binary_set_  = "01";
const char* base_converter::decimal_set_ = "0123456789";
const char* base_converter::hex_set_     = "0123456789ABCDEF";

base_converter::base_converter( const std::string& sourceBaseSet
                              , const std::string& targetBaseSet) 
: source_base_set_(sourceBaseSet)
, target_base_set_(targetBaseSet)
{
  if (sourceBaseSet.empty() || targetBaseSet.empty())
    throw std::invalid_argument("Invalid base character set");
}

std::string 
  base_converter::dec_to_bin(std::string value)
{
  static const base_converter dec2bin(decimal_set_, binary_set_);
  return dec2bin.convert(value);
}

std::string 
  base_converter::bin_to_dec(std::string value)
{
  static const base_converter bin2dec(binary_set_, decimal_set_);
  return bin2dec.convert(value);
}

std::string 
  base_converter::dec_to_hex(std::string value)
{
  static const base_converter dec2hex(decimal_set_, hex_set_);
  return dec2hex.convert(value);
}

std::string 
  base_converter::hex_to_dec(std::string value)
{
  static const base_converter hex2dec(hex_set_, decimal_set_);
  return hex2dec.convert(value);
}

std::string 
  base_converter::convert(std::string value) const
{
  unsigned int numberBase = get_target_base();
  std::string result;

  do
  {
    unsigned int remainder = divide(source_base_set_, value, numberBase);
    result.push_back(target_base_set_[remainder]);
  }
  while ( !value.empty() && 
          !(value.length() == 1 && value[0] == source_base_set_[0]));

  std::reverse(result.begin(), result.end());
  return result;
}

unsigned int 
  base_converter::divide( const std::string& baseDigits
                        , std::string & x
                        , unsigned int y )
{
  std::string quotient;

  size_t length = x.length();
  for (size_t i = 0; i < length; ++i)
  {
    size_t j = i + 1 + x.length() - length;
    if (x.length() < j)
      break;

    unsigned int value = base2dec(baseDigits, x.substr(0, j));

    quotient.push_back(baseDigits[value / y]);
    x = dec2base(baseDigits, value % y) + x.substr(j);
  }

  // calculate remainder
  unsigned int remainder = base2dec(baseDigits, x);

  // remove leading "zeros" from quotient and store in 'x'
  size_t n = quotient.find_first_not_of(baseDigits[0]);
  if (n != std::string::npos)
  {
    x = quotient.substr(n);
  }
  else
  {
    x.clear();
  }

  return remainder;
}

std::string 
  base_converter::dec2base( const std::string& baseDigits
                          , unsigned int value )
{
  unsigned int numberBase = (unsigned int)baseDigits.length();
  std::string result;
  do 
  {
    result.push_back(baseDigits[value % numberBase]);
    value /= numberBase;
  } 
  while (value > 0);

  std::reverse(result.begin(), result.end());
  return result;
}

unsigned int 
  base_converter::base2dec( const std::string& baseDigits
                          , const std::string& value )
{
  unsigned int numberBase = (unsigned int)baseDigits.length();
  unsigned int result = 0;
  for (size_t i = 0; i < value.length(); ++i)
  {
    result *= numberBase;
    size_t c = baseDigits.find(value[i]);
    if (c == std::string::npos)
      throw std::runtime_error("base_converter::Invalid character");

    result += (unsigned int)c;
  }

  return result;
}
