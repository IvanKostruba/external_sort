#pragma once

#include "data_structure.h"

#include <random>
#include <limits>

class RandomDataGenerator
{
public:
  RandomDataGenerator();
  void Reseed();
  DataRecord RandomData();

private:
  static constexpr size_t CHAR_BUFFER_SIZE{ 1024 };
  char buffer_[CHAR_BUFFER_SIZE];

  std::mt19937 num_generator_;
  std::uniform_int_distribution<std::uint32_t> number_range{ 0, std::numeric_limits<std::uint32_t>::max() };
  std::uniform_int_distribution<unsigned> str_buffer_range{ 0, CHAR_BUFFER_SIZE - 1 };
};

