#include "stdafx.h"

#include "data_structure.h"

std::ostream& operator<<(std::ostream& os, const DataRecord& r)
{
  os << r.number_ << ". " << r.string_ << "\n";
  return os;
}

std::istream& operator>>(std::istream& is, DataRecord& r)
{
  is >> r.number_;
  is.ignore(2, ' ');
  std::getline( is, r.string_);
  if (!r.string_.empty() && r.string_.back() == '\r')
  {
    r.string_.pop_back();
  }
  return is;
}

bool operator<(const DataRecord& left, const DataRecord& right)
{
  auto str_comparison = left.string_.compare(right.string_);
  return str_comparison != 0 ? str_comparison < 0 : left.number_ < right.number_;
}
