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
#ifndef EWOMS_IO_ERSM_H
#define EWOMS_IO_ERSM_H

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

#include <ewoms/eclio/io/summarynode.hh>
#include <ewoms/eclio/utility/timeservice.hh>
#include <ewoms/common/variant.hh>

namespace Ewoms { namespace EclIO {

/*
  Small class to load RSM files. The RSM file is a text based version of the
  information found in the summary file. The format seems quite fragile - with
  significant whitespace all over the place.The ambition of this ERsm clas is to
  be able to do regression testing of the RSM files we export from the ESmry
  class - it is not meant to be a rock-solid-works-in-every-lunar phase RSM
  loader.
*/
class ESmry;

class ERsm
{

struct Vector{
    SummaryNode header;
    std::vector<double> data;

    Vector(SummaryNode header_, std::size_t size_advice) :
        header(std::move(header_))
    {
        this->data.reserve(size_advice);
    }
};

public:
    ERsm(const std::string& fname);

    const std::vector<TimeStampUTC>& dates() const;
    const std::vector<double>& days() const;
    bool has_dates() const;

    const std::vector<double>& get(const std::string& key) const;
    bool has(const std::string& key) const;
private:
    void load_block(std::deque<std::string>& lines , std::size_t& vector_length);

    std::unordered_map<std::string, Vector> vectors;
    Ewoms::variant<std::vector<double>, std::vector<TimeStampUTC>> time;
};

bool cmp(const ESmry& esmr, const ERsm& ersm);

}
}

#endif
