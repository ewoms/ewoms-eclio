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
#ifndef EWOMS_IO_EINIT_H
#define EWOMS_IO_EINIT_H

#include <ewoms/eclio/io/eclfile.hh>

#include <array>
#include <vector>
#include <map>

namespace Ewoms { namespace EclIO {

class EInit : public EclFile
{
public:
    explicit EInit(const std::string& filename);

    const std::vector<std::string>& list_of_lgrs() const { return lgr_names; }

    std::vector<EclFile::EclEntry> list_arrays() const;
    std::vector<EclFile::EclEntry> list_arrays(const std::string& grid_name) const;

    const std::array<int, 3>& grid_dimension(const std::string& grid_name = "global") const;
    int activeCells(const std::string& grid_name = "global") const;

    bool hasLGR(const std::string& name) const;

    template <typename T>
    const std::vector<T>& getInitData(const std::string& name, const std::string& grid_name = "global")
    {
        return this->ImplgetInitData(name, grid_name, T());
    }

protected:
    template <typename T>
    const std::vector<T>& ImplgetInitData(const std::string& name, const std::string& grid_name, T dummy)
    {
        EWOMS_THROW(std::logic_error, "Type not implemented");
    }

    const std::vector<int>& ImplgetInitData(const std::string& name, const std::string& grid_name, int dummy)
    {
        int arr_ind = get_array_index(name, grid_name);
        return getImpl(arr_ind, INTE, inte_array, "integer");
    }

    const std::vector<float>& ImplgetInitData(const std::string& name, const std::string& grid_name, float dummy)
    {
        int arr_ind = get_array_index(name, grid_name);
        return getImpl(arr_ind, REAL, real_array, "float");
    }

    const std::vector<double>& ImplgetInitData(const std::string& name, const std::string& grid_name, double dummy)
    {
        int arr_ind = get_array_index(name, grid_name);
        return getImpl(arr_ind, DOUB, doub_array, "double");
    }

    const std::vector<bool>& ImplgetInitData(const std::string& name, const std::string& grid_name, bool dummy)
    {
        int arr_ind = get_array_index(name, grid_name);
        return getImpl(arr_ind, LOGI, logi_array, "bool");
    }

    const std::vector<std::string>& ImplgetInitData(const std::string& name, const std::string& grid_name, std::string dummy)
    {
        int arr_ind = get_array_index(name, grid_name);
        if (array_type[arr_ind] == Ewoms::EclIO::CHAR)
            return getImpl(arr_ind, array_type[arr_ind], char_array, "char");

        if (array_type[arr_ind] == Ewoms::EclIO::C0NN)
            return getImpl(arr_ind, array_type[arr_ind], char_array, "c0nn");

        EWOMS_THROW(std::runtime_error, "Array not of type CHAR or C0nn");
    }

private:
    std::array<int, 3> global_nijk;
    std::vector<std::array<int, 3>> lgr_nijk;

    int global_nactive;
    std::vector<int> lgr_nactive;

    std::vector<std::string> lgr_names;

    std::map<std::string,int> global_array_index;
    std::vector<std::map<std::string,int>> lgr_array_index;

    int get_array_index(const std::string& name, const std::string& grid_name) const;
    int get_lgr_index(const std::string& grid_name) const;
};

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_EINIT_H
