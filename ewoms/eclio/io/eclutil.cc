/*

   This file is part of the eWoms project.

   eWoms is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eWoms is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
   */
#include "config.h"

#include <ewoms/eclio/io/eclutil.hh>

#include <ewoms/eclio/errormacros.hh>

#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <fstream>

int Ewoms::EclIO::flipEndianInt(int num)
{
    unsigned int tmp = __builtin_bswap32(num);
    return static_cast<int>(tmp);
}

int64_t Ewoms::EclIO::flipEndianLongInt(int64_t num)
{
    uint64_t tmp = __builtin_bswap64(num);
    return static_cast<int64_t>(tmp);
}

float Ewoms::EclIO::flipEndianFloat(float num)
{
    float value = num;

    char* floatToConvert = reinterpret_cast<char*>(&value);
    std::reverse(floatToConvert, floatToConvert+4);

    return value;
}

double Ewoms::EclIO::flipEndianDouble(double num)
{
    double value = num;

    char* doubleToConvert = reinterpret_cast<char*>(&value);
    std::reverse(doubleToConvert, doubleToConvert+8);

    return value;
}

bool Ewoms::EclIO::isEOF(std::fstream* fileH)
{
    int num;
    int64_t pos = fileH->tellg();
    fileH->read(reinterpret_cast<char*>(&num), sizeof(num));

    if (fileH->eof()) {
        return true;
    } else {
        fileH->seekg (pos);
        return false;
    }
}

std::tuple<int, int> Ewoms::EclIO::block_size_data_binary(eclArrType arrType)
{
    using BlockSizeTuple = std::tuple<int, int>;

    switch (arrType) {
    case INTE:
        return BlockSizeTuple{sizeOfInte, MaxBlockSizeInte};
        break;
    case REAL:
        return BlockSizeTuple{sizeOfReal, MaxBlockSizeReal};
        break;
    case DOUB:
        return BlockSizeTuple{sizeOfDoub, MaxBlockSizeDoub};
        break;
    case LOGI:
        return BlockSizeTuple{sizeOfLogi, MaxBlockSizeLogi};
        break;
    case CHAR:
        return BlockSizeTuple{sizeOfChar, MaxBlockSizeChar};
        break;
    case MESS:
        EWOMS_THROW(std::invalid_argument, "Type 'MESS' have no associated data");
        break;
    default:
        EWOMS_THROW(std::invalid_argument, "Unknown field type");
        break;
    }
}

std::tuple<int, int, int> Ewoms::EclIO::block_size_data_formatted(eclArrType arrType)
{
    using BlockSizeTuple = std::tuple<int, int, int>;

    switch (arrType) {
    case INTE:
        return BlockSizeTuple{MaxNumBlockInte, numColumnsInte, columnWidthInte};
        break;
    case REAL:
        return BlockSizeTuple{MaxNumBlockReal,numColumnsReal, columnWidthReal};
        break;
    case DOUB:
        return BlockSizeTuple{MaxNumBlockDoub,numColumnsDoub, columnWidthDoub};
        break;
    case LOGI:
        return BlockSizeTuple{MaxNumBlockLogi,numColumnsLogi, columnWidthLogi};
        break;
    case CHAR:
        return BlockSizeTuple{MaxNumBlockChar,numColumnsChar, columnWidthChar};
        break;
    case MESS:
        EWOMS_THROW(std::invalid_argument, "Type 'MESS' have no associated data") ;
        break;
    default:
        EWOMS_THROW(std::invalid_argument, "Unknown field type");
        break;
    }
}

std::string Ewoms::EclIO::trimr(const std::string &str1)
{
    if (str1 == "        ") {
        return "";
    } else {
        int p = str1.find_last_not_of(" ");

        return str1.substr(0,p+1);
    }
}

uint64_t Ewoms::EclIO::sizeOnDiskBinary(int64_t num, Ewoms::EclIO::eclArrType arrType)
{
    uint64_t size = 0;

    if (arrType == Ewoms::EclIO::MESS) {
        if (num > 0) {
            std::string message = "In routine calcSizeOfArray, type MESS can not have size > 0";
            EWOMS_THROW(std::invalid_argument, message);
        }
    } else {
        if (num > 0) {
            auto sizeData = Ewoms::EclIO::block_size_data_binary(arrType);

            int sizeOfElement = std::get<0>(sizeData);
            int maxBlockSize = std::get<1>(sizeData);
            int maxNumberOfElements = maxBlockSize / sizeOfElement;

            auto numBlocks = static_cast<uint64_t>(num)/static_cast<uint64_t>(maxNumberOfElements);
            auto rest = static_cast<uint64_t>(num) - numBlocks*static_cast<uint64_t>(maxNumberOfElements);

            auto size2Inte = static_cast<uint64_t>(Ewoms::EclIO::sizeOfInte) * 2;
            auto sizeFullBlocks = numBlocks * (static_cast<uint64_t>(maxBlockSize) + size2Inte);

            uint64_t sizeLastBlock = 0;

            if (rest > 0)
                sizeLastBlock = rest * static_cast<uint64_t>(sizeOfElement) + size2Inte;

            size = sizeFullBlocks + sizeLastBlock;
        }
    }

    return size;
}

uint64_t Ewoms::EclIO::sizeOnDiskFormatted(const int64_t num, Ewoms::EclIO::eclArrType arrType)
{
    uint64_t size = 0;

    if (arrType == Ewoms::EclIO::MESS) {
        if (num > 0) {
            EWOMS_THROW(std::invalid_argument, "In routine calcSizeOfArray, type MESS can not have size > 0");
        }
    } else {
        auto sizeData = block_size_data_formatted(arrType);

        int maxBlockSize = std::get<0>(sizeData);
        int nColumns = std::get<1>(sizeData);
        int columnWidth = std::get<2>(sizeData);

        int nBlocks = num /maxBlockSize;
        int sizeOfLastBlock = num %  maxBlockSize;

        size = 0;

        if (nBlocks > 0) {
            int nLinesBlock = maxBlockSize / nColumns;
            int rest = maxBlockSize % nColumns;

            if (rest > 0) {
                nLinesBlock++;
            }

            int64_t blockSize = maxBlockSize * columnWidth + nLinesBlock;
            size = nBlocks * blockSize;
        }

        int nLines = sizeOfLastBlock / nColumns;
        int rest = sizeOfLastBlock % nColumns;

        size = size + sizeOfLastBlock * columnWidth + nLines;

        if (rest > 0) {
            size++;
        }
    }

    return size;
}

void Ewoms::EclIO::readBinaryHeader(std::fstream& fileH, std::string& tmpStrName,
                      int& tmpSize, std::string& tmpStrType)
{
    int bhead;

    fileH.read(reinterpret_cast<char*>(&bhead), sizeof(bhead));
    bhead = Ewoms::EclIO::flipEndianInt(bhead);

    if (bhead != 16){
        std::string message="Error reading binary header. Expected 16 bytes of header data, found " + std::to_string(bhead);
        EWOMS_THROW(std::runtime_error, message);
    }

    fileH.read(&tmpStrName[0], 8);

    fileH.read(reinterpret_cast<char*>(&tmpSize), sizeof(tmpSize));
    tmpSize = Ewoms::EclIO::flipEndianInt(tmpSize);

    fileH.read(&tmpStrType[0], 4);

    fileH.read(reinterpret_cast<char*>(&bhead), sizeof(bhead));
    bhead = Ewoms::EclIO::flipEndianInt(bhead);

    if (bhead != 16){
        std::string message="Error reading binary header. Expected 16 bytes of header data, found " + std::to_string(bhead);
        EWOMS_THROW(std::runtime_error, message);
    }
}

void Ewoms::EclIO::readBinaryHeader(std::fstream& fileH, std::string& arrName,
                      int64_t& size, Ewoms::EclIO::eclArrType &arrType)
{
    std::string tmpStrName(8,' ');
    std::string tmpStrType(4,' ');
    int tmpSize;

    readBinaryHeader(fileH, tmpStrName, tmpSize, tmpStrType);

    if (tmpStrType == "X231"){
        std::string x231ArrayName = tmpStrName;
        int x231exp = tmpSize * (-1);

        readBinaryHeader(fileH, tmpStrName, tmpSize, tmpStrType);

        if (x231ArrayName != tmpStrName)
            EWOMS_THROW(std::runtime_error, "Invalied X231 header, name should be same in both headers'");

        if (x231exp < 0)
            EWOMS_THROW(std::runtime_error, "Invalied X231 header, size of array should be negative'");

        size = static_cast<int64_t>(tmpSize) + static_cast<int64_t>(x231exp) * pow(2,31);
    } else {
        size = static_cast<int64_t>(tmpSize);
    }

    arrName = tmpStrName;
    if (tmpStrType == "INTE")
        arrType = Ewoms::EclIO::INTE;
    else if (tmpStrType == "REAL")
        arrType = Ewoms::EclIO::REAL;
    else if (tmpStrType == "DOUB")
        arrType = Ewoms::EclIO::DOUB;
    else if (tmpStrType == "CHAR")
        arrType = Ewoms::EclIO::CHAR;
    else if (tmpStrType =="LOGI")
        arrType = Ewoms::EclIO::LOGI;
    else if (tmpStrType == "MESS")
        arrType = Ewoms::EclIO::MESS;
    else
        EWOMS_THROW(std::runtime_error, "Error, unknown array type '" + tmpStrType +"'");
}

void Ewoms::EclIO::readFormattedHeader(std::fstream& fileH, std::string& arrName,
                         int64_t &num, Ewoms::EclIO::eclArrType &arrType)
{
    std::string line;
    std::getline(fileH,line);

    int p1 = line.find_first_of("'");
    int p2 = line.find_first_of("'",p1+1);
    int p3 = line.find_first_of("'",p2+1);
    int p4 = line.find_first_of("'",p3+1);

    if (p1 == -1 || p2 == -1 || p3 == -1 || p4 == -1) {
        EWOMS_THROW(std::runtime_error, "Header name and type should be enclosed with '");
    }

    arrName = line.substr(p1 + 1, p2 - p1 - 1);
    std::string antStr = line.substr(p2 + 1, p3 - p2 - 1);
    std::string arrTypeStr = line.substr(p3 + 1, p4 - p3 - 1);

    num = std::stol(antStr);

    if (arrTypeStr == "INTE")
        arrType = Ewoms::EclIO::INTE;
    else if (arrTypeStr == "REAL")
        arrType = Ewoms::EclIO::REAL;
    else if (arrTypeStr == "DOUB")
        arrType = Ewoms::EclIO::DOUB;
    else if (arrTypeStr == "CHAR")
        arrType = Ewoms::EclIO::CHAR;
    else if (arrTypeStr == "LOGI")
        arrType = Ewoms::EclIO::LOGI;
    else if (arrTypeStr == "MESS")
        arrType = Ewoms::EclIO::MESS;
    else
        EWOMS_THROW(std::runtime_error, "Error, unknown array type '" + arrTypeStr +"'");

    if (arrName.size() != 8) {
        EWOMS_THROW(std::runtime_error, "Header name should be 8 characters");
    }
}
