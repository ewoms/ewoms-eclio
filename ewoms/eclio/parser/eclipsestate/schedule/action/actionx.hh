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

#ifndef ActionX_HH_
#define ActionX_HH_

#include <string>
#include <vector>
#include <ctime>

#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionast.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionresult.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/condition.hh>

namespace Ewoms {
class DeckKeyword;

namespace Action {

/*
  The ActionX class internalizes the ACTIONX keyword. This keyword represents a
  small in-deck programming language for the SCHEDULE section. In the deck the
  ACTIONX keyword comes together with a 'ENDACTIO' kewyord and then a list of
  regular keywords in the between. The principle is then that ACTIONX represents
  a condition, and when that condition is satisfied the keywords are applied. In
  the example below the ACTIONX keyword defines a condition whether well OPX has
  watercut above 0.75, when the condition is met the WELOPEN keyword is applied
  - and the well is shut.

  ACTIONX
     'NAME'  /
     WWCT OPX > 0.50 /
  /

  WELOPEN
     'OPX'  OPEN /
  /

  ENDACTION

*/

class ActionX {
public:
    ActionX();
    ActionX(const std::string& name, size_t max_run, double max_wait, std::time_t start_time);
    ActionX(const DeckKeyword& kw, std::time_t start_time);
    ActionX(const DeckRecord& record, std::time_t start_time);
    ActionX(const std::string& nam,
            size_t maxRun,
            double minWait,
            std::time_t startTime,
            const std::vector<DeckKeyword>& keyword,
            const AST& cond,
            const std::vector<Condition>& conditions,
            size_t runCount,
            std::time_t lastRun);

    void addKeyword(const DeckKeyword& kw);
    bool ready(std::time_t sim_time) const;
    Action::Result eval(std::time_t sim_time, const Action::Context& context) const;

    std::string name() const { return this->m_name; }
    size_t max_run() const { return this->m_max_run; }
    double min_wait() const { return this->m_min_wait; }
    std::time_t start_time() const { return this->m_start_time; }
    std::vector<DeckKeyword>::const_iterator begin() const;
    std::vector<DeckKeyword>::const_iterator end() const;
    static bool valid_keyword(const std::string& keyword);

    /*
      The conditions() and keyword_strings() methods, and their underlying data
      members are only present to support writing formatted restart files.
    */
    const std::vector<DeckKeyword>& getKeywords() const;
    const std::vector<Condition>& conditions() const;
    const Action::AST& getCondition() const;
    std::vector<std::string> keyword_strings() const;
    size_t getRunCount() const;
    std::time_t getLastRun() const;

    bool operator==(const ActionX& data) const;

private:
    std::string m_name;
    size_t m_max_run = 0;
    double m_min_wait = 0.0;
    std::time_t m_start_time;

    std::vector<DeckKeyword> keywords;
    Action::AST condition;
    std::vector<Condition> m_conditions;
    mutable size_t run_count = 0;
    mutable std::time_t last_run = 0;
};

}
}
#endif /* WELL_HH_ */
