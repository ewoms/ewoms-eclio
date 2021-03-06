// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef EWOMS_IO_ECLUTIL_H
#define EWOMS_IO_ECLUTIL_H

#include <ewoms/eclio/io/ecliodata.hh>

#include <string>
#include <tuple>
#include <vector>
#include <functional>

namespace Ewoms { namespace EclIO {

    int flipEndianInt(int num);
    int64_t flipEndianLongInt(int64_t num);
    float flipEndianFloat(float num);
    double flipEndianDouble(double num);
    bool isEOF(std::fstream* fileH);
    bool fileExists(const std::string& filename);
    bool isFormatted(const std::string& filename);

    std::tuple<int, int> block_size_data_binary(eclArrType arrType);
    std::tuple<int, int, int> block_size_data_formatted(eclArrType arrType);

    std::string trimr(const std::string &str1);

    uint64_t sizeOnDiskBinary(int64_t num, Ewoms::EclIO::eclArrType arrType, int elementSize);
    uint64_t sizeOnDiskFormatted(const int64_t num, Ewoms::EclIO::eclArrType arrType, int elementSize);

    void readBinaryHeader(std::fstream& fileH, std::string& tmpStrName,
                      int& tmpSize, std::string& tmpStrType);

    void readBinaryHeader(std::fstream& fileH, std::string& arrName,
                      int64_t& size, Ewoms::EclIO::eclArrType &arrType, int& elementSize);

    void readFormattedHeader(std::fstream& fileH, std::string& arrName,
                      int64_t &num, Ewoms::EclIO::eclArrType &arrType, int& elementSize);

    template<typename T, typename T2>
    std::vector<T> readBinaryArray(std::fstream& fileH, const int64_t size, Ewoms::EclIO::eclArrType type,
                               std::function<T(T2)>& flip, int elementSize);

    std::vector<int> readBinaryInteArray(std::fstream &fileH, const int64_t size);
    std::vector<float> readBinaryRealArray(std::fstream& fileH, const int64_t size);
    std::vector<double> readBinaryDoubArray(std::fstream& fileH, const int64_t size);
    std::vector<bool> readBinaryLogiArray(std::fstream &fileH, const int64_t size);
    std::vector<unsigned int> readBinaryRawLogiArray(std::fstream &fileH, const int64_t size);
    std::vector<std::string> readBinaryCharArray(std::fstream& fileH, const int64_t size);
    std::vector<std::string> readBinaryC0nnArray(std::fstream& fileH, const int64_t size, int elementSize);

    template<typename T>
    std::vector<T> readFormattedArray(const std::string& file_str, const int size, int64_t fromPos,
                                       std::function<T(const std::string&)>& process);

    std::vector<int> readFormattedInteArray(const std::string& file_str, const int64_t size, int64_t fromPos);

    std::vector<std::string> readFormattedCharArray(const std::string& file_str, const int64_t size,
                                                    int64_t fromPos, int elementSize);

    std::vector<float> readFormattedRealArray(const std::string& file_str, const int64_t size, int64_t fromPos);
    std::vector<std::string> readFormattedRealRawStrings(const std::string& file_str, const int64_t size, int64_t fromPos);

    std::vector<bool> readFormattedLogiArray(const std::string& file_str, const int64_t size, int64_t fromPos);
    std::vector<double> readFormattedDoubArray(const std::string& file_str, const int64_t size, int64_t fromPos);

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_ECLUTIL_H
