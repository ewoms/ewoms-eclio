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
#ifndef EWOMS_IO_ECLOUTPUT_H
#define EWOMS_IO_ECLOUTPUT_H

#include <fstream>
#include <ios>
#include <string>
#include <typeinfo>
#include <vector>

#include <ewoms/eclio/io/ecliodata.hh>
#include <ewoms/eclio/io/paddedoutputstring.hh>
#include <iostream>

namespace Ewoms { namespace EclIO { namespace OutputStream {
    class Restart;
    class SummarySpecification;
}}}

namespace Ewoms { namespace EclIO {

class EclOutput
{
public:
    EclOutput(const std::string&            filename,
              const bool                    formatted,
              const std::ios_base::openmode mode = std::ios::out);

    template<typename T>
    void write(const std::string& name,
               const std::vector<T>& data)
    {
        eclArrType arrType = MESS;
        if (typeid(T) == typeid(int))
            arrType = INTE;
        else if (typeid(T) == typeid(float))
            arrType = REAL;
        else if (typeid(T) == typeid(double))
            arrType = DOUB;
        else if (typeid(T) == typeid(bool))
            arrType = LOGI;
        else if (typeid(T) == typeid(char))
            arrType = MESS;

        if (isFormatted)
        {
            writeFormattedHeader(name, data.size(), arrType);
            if (arrType != MESS)
                writeFormattedArray(data);
        }
        else
        {
            writeBinaryHeader(name, data.size(), arrType);
            if (arrType != MESS)
                writeBinaryArray(data);
        }
    }

    void message(const std::string& msg);
    void flushStream();

    friend class OutputStream::Restart;
    friend class OutputStream::SummarySpecification;

private:
    void writeBinaryHeader(const std::string& arrName, long int size, eclArrType arrType);

    template <typename T>
    void writeBinaryArray(const std::vector<T>& data);

    void writeBinaryCharArray(const std::vector<std::string>& data);
    void writeBinaryCharArray(const std::vector<PaddedOutputString<8>>& data);

    void writeFormattedHeader(const std::string& arrName, int size, eclArrType arrType);

    template <typename T>
    void writeFormattedArray(const std::vector<T>& data);

    void writeFormattedCharArray(const std::vector<std::string>& data);
    void writeFormattedCharArray(const std::vector<PaddedOutputString<8>>& data);

    void writeArrayType(const eclArrType arrType);
    std::string make_real_string(float value) const;
    std::string make_doub_string(double value) const;

    bool isFormatted;
    std::ofstream ofileH;
};

template<>
void EclOutput::write<std::string>(const std::string& name,
                                   const std::vector<std::string>& data);

template <>
void EclOutput::write<PaddedOutputString<8>>
    (const std::string&                        name,
     const std::vector<PaddedOutputString<8>>& data);

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_ECLOUTPUT_H
