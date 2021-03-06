// ExternalSort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "data_structure.h"
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <tuple>
#include <memory>
#include <cstdio>

/// Read chunk of file.
/**
* \param file_name Input file.
* \param offset Start point at the input file.
* \param buffer Externaly provided buffer for read operations.
* \param limit How much data to load from file, bytes.
* \param out Container for parsed data.
*
* \return Amount of bytes of valid data actually read.
*/
size_t load_file_chunk(const char* file_name, size_t offset, std::vector<char>& buffer, size_t limit, std::vector<DataRecord>& out)
{
  size_t bytes_to_read{ limit };
  // Opening file in binary mode to correctly count read bytes, and manage offset.
  std::ifstream fin{ file_name, std::ios::binary };
  if (!fin.is_open())
  {
    std::cerr << "Cannot open input file.\n";
    exit(1);
  }
  fin.seekg(offset);
  while (fin && bytes_to_read > 0)
  {
    fin.read(&buffer.front(), std::min(buffer.size(), bytes_to_read));
    size_t bytes_read = fin.gcount();
    size_t valid_data_end{ std::min(bytes_read, std::min(buffer.size() - 1, limit - 1)) };
    while (buffer[valid_data_end] != '\n'
        && valid_data_end != 0)
    {
      --valid_data_end;
    }
    auto test = buffer[valid_data_end];
    if (valid_data_end == 0) // Not even one line or null-terminated string detected.
    {
      break;
    }
    auto istream = std::istringstream{ std::string{&buffer.front(), valid_data_end} };
    std::copy(std::istream_iterator<DataRecord>{istream},
              std::istream_iterator<DataRecord>(),
              std::back_inserter(out) );
    bytes_to_read -= valid_data_end + 1;
    if (bytes_to_read == bytes_read - valid_data_end - 1)
    {
      // Limit remainder does not contain valid data record.
      break;
    }
    // Move offset to the beginning of last incomplete record
    size_t new_offset = fin.tellg();
    new_offset -= bytes_read - valid_data_end - 1;
    fin.seekg(new_offset);
  }
  return limit - bytes_to_read;
}

// Actually, maximum size data record should be slightly smaller, but absolute presision is not needed here
static constexpr size_t DATA_RECORD_MAX_SIZE = 1100;
static constexpr size_t BUFFER_SIZE = 100000000;

/// Split input file into parts fitting into memory and sort them separately
/**
* \param input_file Input file.
* \param memory_limit Size limit of file chunk loaded in main memory.
* \param out_file_prefix Prefix for file chunks with sorted data.
*
* \return Amount of bytes of valid data actually read.
*/
std::tuple<size_t, size_t> sort_file_by_chunks(const char* input_file, size_t memory_limit, const std::string& out_file_prefix)
{
  std::ifstream fin{ input_file, std::ios::binary | std::ios::ate };
  if (!fin.is_open())
  {
    std::cerr << "Cannot open input file.\n";
    exit(1);
  }
  const size_t file_size = fin.tellg();
  fin.close();
  auto expected_parts = 1u;
  while (file_size / expected_parts + DATA_RECORD_MAX_SIZE > memory_limit)
  {
    ++expected_parts;
  }
  // Adding size of DataRecord to part size, so that incomplete records
  // would not mess with expected parts count leading to creation of small
  // extra part. We need parts of more or less equal size for our sorting.
  auto part_size = file_size / expected_parts + DATA_RECORD_MAX_SIZE;
  size_t processed{ 0 };
  auto chunk_counter = 0;
  std::vector<char> buffer;
  buffer.resize(BUFFER_SIZE);
  while (processed < file_size)
  {
    std::vector<DataRecord> data{};
    auto bytes_read = load_file_chunk(input_file, processed, buffer, std::min(part_size, file_size - processed), data);
    std::sort(data.begin(), data.end());
    auto out_file_name = out_file_prefix + std::to_string(chunk_counter);
    std::ofstream fout{ out_file_name };
    for (auto& item : data)
    {
      fout << item;
    }
    ++chunk_counter;
    processed += bytes_read;
  }
  return std::make_tuple(chunk_counter, part_size);
}

/// Read already sorted files part by part and combine that data into final output
/**
* \param chunk_prefix Prefix for file chunks with sorted data.
* \param chunk_count Number of sorted chunks.
* \param part_size Upper limit for chunk size.
* \param output_file Name of the final output file.
*
* \return -
*/
void compose_output_file(const std::string& chunk_prefix, size_t chunk_count, size_t part_size, const char* output_file)
{
  auto parts_offset = std::unique_ptr< size_t[] >(new size_t[chunk_count]);
  for (int i = 0; i < chunk_count; ++i)
  {
    parts_offset[i] = 0;
  }
  std::ofstream fout{ output_file };
  if (!fout.is_open())
  {
    std::cerr << "Cannot open output file.\n";
    exit(1);
  }
  const auto chunk_part_size = part_size / chunk_count + DATA_RECORD_MAX_SIZE;
  std::vector<char> buffer;
  buffer.resize(BUFFER_SIZE);
  for (int i = 0; i < chunk_count; ++i)
  {
    std::vector<DataRecord> data{};
    for (int j = 0; j < chunk_count; ++j)
    {
      auto chunk_name = chunk_prefix + std::to_string(j);
      parts_offset[j] += load_file_chunk(chunk_name.c_str(), parts_offset[j], buffer, chunk_part_size, data);
    }
    std::sort(data.begin(), data.end());
    for (auto& item : data)
    {
      fout << item;
    }
  }
}

/// Remove created earlier temporary files
void clean_tmp_files(const std::string& chunk_prefix, size_t chunk_count)
{
  for (int i = 0; i < chunk_count; ++i)
  {
    auto chunk_name = chunk_prefix + std::to_string(i);
    std::remove(chunk_name.c_str());
  }
}

/// Entry point
int main(int argc, char** argv)
{
  const size_t chunk_size = strtoull(argv[2], nullptr, 0) * 1000 * 1000;
  const std::string tmp_file_prefix = "tmp_";
  size_t chunk_count, part_size;
  std::tie(chunk_count, part_size) = sort_file_by_chunks(argv[1], chunk_size, tmp_file_prefix);
  compose_output_file(tmp_file_prefix, chunk_count, part_size, argv[3]);
  clean_tmp_files(tmp_file_prefix, chunk_count);
  return 0;
}
