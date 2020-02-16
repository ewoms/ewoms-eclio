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

// NOTE: This file is inteded to be copy-pasted into user code
// through an #include statement.

#include <string>

#include <ewoms/common/filesystem.hh>

namespace {

    class WorkArea
    {
    public:
        explicit WorkArea(const std::string& subdir = "")
            : root_(Ewoms::filesystem::temp_directory_path() /
                    Ewoms::unique_path("wrk-%%%%"))
            , area_(root_)
            , orig_(Ewoms::filesystem::current_path())
        {
            if (! subdir.empty())
                this->area_ /= subdir;

            Ewoms::filesystem::create_directories(this->area_);
            Ewoms::filesystem::current_path(this->area_);
        }

        void copyIn(const std::string& filename) const
        {
            Ewoms::filesystem::copy_file(this->orig_ / filename,
                                       this->area_ / filename);
        }

        std::string currentWorkingDirectory() const
        {
            return this->area_.generic_string();
        }

        void makeSubDir(const std::string& dirname)
        {
            Ewoms::filesystem::create_directories(this->area_ / dirname);
        }

        ~WorkArea()
        {
            Ewoms::filesystem::current_path(this->orig_);
            Ewoms::filesystem::remove_all(this->root_);
        }

    private:
        Ewoms::filesystem::path root_;
        Ewoms::filesystem::path area_;
        Ewoms::filesystem::path orig_;
    };
} // Anonymous
