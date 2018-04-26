#include "stdafx.h"

#include "data_generator.h"

#include <algorithm>
#include <cctype>

RandomDataGenerator::RandomDataGenerator()
{
  Reseed();
}

void RandomDataGenerator::Reseed()
{
  std::random_device dev;
  num_generator_.seed(dev());
  // This leaves tabs and newlines out, because, in my uderstanding, newline is the line separator.
  // So there is no need to support all ascii values conforming to std::isspace().
  std::uniform_int_distribution<> ascii_range{ 32, 126 };
  char tmp;
  for (int i = 0; i < CHAR_BUFFER_SIZE;)
  {
    tmp = static_cast<char>(ascii_range(num_generator_));
    if (!std::isalnum(tmp) && !std::ispunct(tmp) && !std::isspace(tmp))
    {
      continue;
    }
    buffer_[i] = tmp;
    ++i;
  }
}

DataRecord RandomDataGenerator::RandomData()
{
  auto number = number_range(num_generator_);
  auto str_begin = str_buffer_range(num_generator_);
  auto str_len = std::min<size_t>(str_buffer_range(num_generator_), CHAR_BUFFER_SIZE - str_begin);
  return DataRecord{ number, std::string{ &buffer_[str_begin], str_len } };
}
