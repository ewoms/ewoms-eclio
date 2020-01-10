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

#ifndef EWOMS_SUMMARY_CONFIG_H
#define EWOMS_SUMMARY_CONFIG_H

#include <array>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include <ewoms/eclio/opmlog/location.hh>

namespace Ewoms {

    /*
      Very small utility class to get value semantics on the smspec_node
      pointers. This should die as soon as the smspec_node class proper gets
      value semantics.
    */

    class SummaryNode {
    public:
        enum class Category {
            Well, Group, Field,
            Region, Block,
            Connection, Segment,
            Miscellaneous,
        };

        enum class Type {
            Rate, Total, Ratio, Pressure, Count,
            Undefined,
        };

        SummaryNode() = default;
        explicit SummaryNode(std::string keyword, const Category cat, Location loc_arg);

        SummaryNode& parameterType(const Type type);
        SummaryNode& namedEntity(std::string name);
        SummaryNode& number(const int num);
        SummaryNode& isUserDefined(const bool userDefined);

        const std::string& keyword() const { return this->keyword_; }
        Category category() const { return this->category_; }
        Type type() const { return this->type_; }
        const std::string& namedEntity() const { return this->name_; }
        int number() const { return this->number_; }
        bool isUserDefined() const { return this->userDefined_; }

        std::string uniqueNodeKey() const;
        const Location& location( ) const { return this->loc; }
    private:
        std::string keyword_;
        Category    category_;
        Location    loc;
        Type        type_{ Type::Undefined };
        std::string name_{};
        int         number_{std::numeric_limits<int>::min()};
        bool        userDefined_{false};
    };

    SummaryNode::Category parseKeywordCategory(const std::string& keyword);

    bool operator==(const SummaryNode& lhs, const SummaryNode& rhs);
    bool operator<(const SummaryNode& lhs, const SummaryNode& rhs);

    inline bool operator!=(const SummaryNode& lhs, const SummaryNode& rhs)
    {
        return ! (lhs == rhs);
    }

    inline bool operator<=(const SummaryNode& lhs, const SummaryNode& rhs)
    {
        return ! (rhs < lhs);
    }

    inline bool operator>(const SummaryNode& lhs, const SummaryNode& rhs)
    {
        return rhs < lhs;
    }

    inline bool operator>=(const SummaryNode& lhs, const SummaryNode& rhs)
    {
        return ! (lhs < rhs);
    }

    class Deck;
    class TableManager;
    class EclipseState;
    class ParserKeyword;
    class Schedule;
    class ErrorGuard;
    class ParseContext;
    class GridDims;

    class SummaryConfig {
        public:
            typedef SummaryNode keyword_type;
            typedef std::vector< keyword_type > keyword_list;
            typedef keyword_list::const_iterator const_iterator;

            SummaryConfig() = default;
            SummaryConfig( const Deck&,
                           const Schedule&,
                           const TableManager&,
                           const ParseContext&,
                           ErrorGuard&);

            template <typename T>
            SummaryConfig( const Deck&,
                           const Schedule&,
                           const TableManager&,
                           const ParseContext&,
                           T&&);

            SummaryConfig( const Deck&,
                           const Schedule&,
                           const TableManager&);

            SummaryConfig(const keyword_list& kwds,
                          const std::set<std::string>& shortKwds,
                          const std::set<std::string>& smryKwds);

            const_iterator begin() const;
            const_iterator end() const;
            size_t size() const;
            SummaryConfig& merge( const SummaryConfig& );
            SummaryConfig& merge( SummaryConfig&& );

            /*
              The hasKeyword() method will consult the internal set
              'short_keywords', i.e. the query should be based on pure
              keywords like 'WWCT' and 'BPR' - and *not* fully
              identifiers like 'WWCT:OPX' and 'BPR:10,12,3'.
            */
            bool hasKeyword( const std::string& keyword ) const;

            /*
               The hasSummaryKey() method will look for fully
               qualified keys like 'RPR:3' and 'BPR:10,15,20.
            */
            bool hasSummaryKey(const std::string& keyword ) const;
            /*
              Can be used to query if a certain 3D field, e.g. PRESSURE,
              is required to calculate the summary variables.
            */
            bool require3DField( const std::string& keyword) const;
            bool requireFIPNUM( ) const;

            const keyword_list& getKwds() const;
            const std::set<std::string>& getShortKwds() const;
            const std::set<std::string>& getSmryKwds() const;

            bool operator==(const SummaryConfig& data) const;

        private:
            SummaryConfig( const Deck& deck,
                           const Schedule& schedule,
                           const TableManager& tables,
                           const ParseContext& parseContext,
                           ErrorGuard& errors,
                           const GridDims& dims);

            /*
              The short_keywords set contains only the pure keyword
              part, e.g. "WWCT", and not the qualification with
              well/group name or a numerical value.
            */
            keyword_list keywords;
            std::set<std::string> short_keywords;
            std::set<std::string> summary_keywords;
    };

} //namespace Ewoms

#endif
