#pragma once

#include <string>
#include <iostream>

struct DataRecord
{
  std::uint32_t number_;
  std::string string_;
};

std::ostream& operator<<(std::ostream& os, const DataRecord& r);
std::istream& operator>>(std::istream& is, DataRecord& r);
bool operator<(const DataRecord& left, const DataRecord& right);