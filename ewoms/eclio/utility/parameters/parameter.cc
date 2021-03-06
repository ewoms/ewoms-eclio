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
#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <string>
#include <ewoms/eclio/utility/parameters/parameter.hh>

namespace Ewoms {
	std::string
        correct_parameter_tag(const ParameterMapItem& item)
        {
	    std::string tag = item.getTag();
	    if (tag != ID_xmltag__param) {
		std::string error = "The XML tag was '" +
                                    tag + "' but should be '" +
                                    ID_xmltag__param + "'.\n";
		return error;
	    } else {
		return "";
	    }
	}

	std::string
        correct_type(const Parameter& parameter,
                     const std::string& param_type)
        {
	    std::string type = parameter.getType();
	    if ( (type != param_type) &&
                 (type != ID_param_type__cmdline) ) {
		std::string error = "The data was of type '" + type +
                                    "' but should be of type '" +
                                    param_type + "'.\n";
		return error;
	    } else {
		return "";
	    }
	}
} // namespace Ewoms
