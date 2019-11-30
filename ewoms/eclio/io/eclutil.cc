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

#include <ewoms/eclio/io/eclutil.hh>

#include <ewoms/eclio/errormacros.hh>

#include <algorithm>
#include <stdexcept>

int Ewoms::EclIO::flipEndianInt(int num)
{
    unsigned int tmp = __builtin_bswap32(num);
    return static_cast<int>(tmp);
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
