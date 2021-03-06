// TestDataGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "data_structure.h"
#include "data_generator.h"
#include <fstream>
#include <iterator>
#include <vector>

int main(int argc, char** argv)
{
  RandomDataGenerator generator;
  std::ofstream fout{ argv[1] };
  if (!fout.is_open())
  {
    std::cerr << "Cannot open output file.\n";
    exit(1);
  }
  size_t limit = strtoull(argv[2], nullptr, 0) * 1000 * 1000;
  unsigned reseed_counter{1};
  unsigned reseed_mask = 255;
  unsigned store_copy_mask = 127;
  DataRecord dup_string;
  while( fout.tellp() < limit )
  {
    if ((++reseed_counter & reseed_mask) == 0)
    {
      generator.Reseed();
      auto tmp = generator.RandomData();
      tmp.string_ = dup_string.string_;
      fout << dup_string;
      continue;
    }
    else if ((reseed_counter & store_copy_mask) == 0)
    {
      dup_string = generator.RandomData();
      fout << dup_string;
      continue;
    }
    fout << generator.RandomData();
  }
  fout.close();
  return 0;
}

