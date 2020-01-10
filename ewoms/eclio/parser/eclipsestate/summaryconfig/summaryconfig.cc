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
#include <iostream>
#include <algorithm>
#include <array>
#include <stdexcept>

#include <ewoms/eclio/parser/parsecontext.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/connection.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellconnections.hh>
#include <ewoms/eclio/parser/eclipsestate/summaryconfig/summaryconfig.hh>

namespace Ewoms {

namespace {

    const std::vector<std::string> ALL_keywords = {
        "FAQR",  "FAQRG", "FAQT", "FAQTG", "FGIP", "FGIPG", "FGIPL",
        "FGIR",  "FGIT",  "FGOR", "FGPR",  "FGPT", "FOIP",  "FOIPG",
        "FOIPL", "FOIR",  "FOIT", "FOPR",  "FOPT", "FPR",   "FVIR",
        "FVIT",  "FVPR",  "FVPT", "FWCT",  "FWGR", "FWIP",  "FWIR",
        "FWIT",  "FWPR",  "FWPT",
        "GGIR",  "GGIT",  "GGOR", "GGPR",  "GGPT", "GOIR",  "GOIT",
        "GOPR",  "GOPT",  "GVIR", "GVIT",  "GVPR", "GVPT",  "GWCT",
        "GWGR",  "GWIR",  "GWIT", "GWPR",  "GWPT",
        "WBHP",  "WGIR",  "WGIT", "WGOR",  "WGPR", "WGPT",  "WOIR",
        "WOIT",  "WOPR",  "WOPT", "WPI",   "WTHP", "WVIR",  "WVIT",
        "WVPR",  "WVPT",  "WWCT", "WWGR",  "WWIR", "WWIT",  "WWPR",
        "WWPT",
        // ALL will not expand to these keywords yet
        "AAQR",  "AAQRG", "AAQT", "AAQTG"
    };

    const std::vector<std::string> GMWSET_keywords = {
        "GMCTG", "GMWPT", "GMWPR", "GMWPA", "GMWPU", "GMWPG", "GMWPO", "GMWPS",
        "GMWPV", "GMWPP", "GMWPL", "GMWIT", "GMWIN", "GMWIA", "GMWIU", "GMWIG",
        "GMWIS", "GMWIV", "GMWIP", "GMWDR", "GMWDT", "GMWWO", "GMWWT"
    };

    const std::vector<std::string> FMWSET_keywords = {
        "FMCTF", "FMWPT", "FMWPR", "FMWPA", "FMWPU", "FMWPF", "FMWPO", "FMWPS",
        "FMWPV", "FMWPP", "FMWPL", "FMWIT", "FMWIN", "FMWIA", "FMWIU", "FMWIF",
        "FMWIS", "FMWIV", "FMWIP", "FMWDR", "FMWDT", "FMWWO", "FMWWT"
    };

    const std::vector<std::string> PERFORMA_keywords = {
        "TCPU", "ELAPSED","NEWTON","NLINEARS","NLINSMIN", "NLINSMAX","MLINEARS",
        "MSUMLINS","MSUMNEWT","TIMESTEP","TCPUTS","TCPUDAY","STEPTYPE","TELAPLIN"
    };

    /*
      The variable type 'ECL_SMSPEC_MISC_TYPE' is a catch-all variable
      type, and will by default internalize keywords like 'ALL' and
      'PERFORMA', where only the keywords in the expanded list should
      be included.
    */
    const std::map<std::string, std::vector<std::string>> meta_keywords = {{"PERFORMA", PERFORMA_keywords},
                                                                           {"ALL", ALL_keywords},
                                                                           {"FMWSET", FMWSET_keywords},
                                                                           {"GMWSET", GMWSET_keywords}};

    /*
      This is a hardcoded mapping between 3D field keywords,
      e.g. 'PRESSURE' and 'SWAT' and summary keywords like 'RPR' and
      'BPR'. The purpose of this mapping is to maintain an overview of
      which 3D field keywords are needed by the Summary calculation
      machinery, based on which summary keywords are requested. The
      Summary calculations are implemented in the ewoms-eclio
      repository.
    */
    const std::map<std::string , std::set<std::string>> required_fields =  {
         {"PRESSURE", {"FPR" , "RPR" , "BPR"}},
         {"OIP"  , {"ROIP" , "FOIP" , "FOE"}},
         {"OIPL" , {"ROIPL" ,"FOIPL" }},
         {"OIPG" , {"ROIPG" ,"FOIPG"}},
         {"GIP"  , {"RGIP" , "FGIP"}},
         {"GIPL" , {"RGIPL" , "FGIPL"}},
         {"GIPG" , {"RGIPG", "FGIPG"}},
         {"WIP"  , {"RWIP" , "FWIP"}},
         {"SWAT" , {"BSWAT"}},
         {"SGAS" , {"BSGAS"}}
    };

    bool is_special(const std::string& keyword) {
        using set = std::unordered_set<std::string>;
        static const auto specialkw = set {
            "NEWTON", "NAIMFRAC", "NLINEARS", "NLINSMIN", "NLINSMAX",
            "ELAPSED", "MAXDPR", "MAXDSO", "MAXDSG", "MAXDSW", "STEPTYPE",
            "WNEWTON",
        };

        return specialkw.count(keyword) > set::size_type{0};
    }

    bool is_udq_blacklist(const std::string& keyword) {
        return (keyword == "SUMTHIN") || (keyword == "RUNSUM");
    }

    bool is_udq(const std::string& keyword) {
        // Does 'keyword' match one of the patterns
        //   AU*, BU*, CU*, FU*, GU*, RU*, SU*, or WU*?
        using sz_t = std::string::size_type;
        return (keyword.size() > sz_t{1})
            && (keyword[1] == 'U')
            && !is_udq_blacklist(keyword)
            && (keyword.find_first_of("WGFCRBSA") == sz_t{0});
    }

    bool is_pressure(const std::string& keyword) {
        using set = std::unordered_set<std::string>;
        static const auto presskw = set {
            "BHP", "BHPH", "THP", "THPH", "PR"
        };

        return presskw.count(&keyword[1]) > set::size_type{0};
    }

    bool is_rate(const std::string& keyword) {
        using set = std::unordered_set<std::string>;
        static const auto ratekw = set {
            "OPR", "GPR", "WPR", "LPR", "NPR", "VPR",
            "OPRH", "GPRH", "WPRH", "LPRH",
            "OVPR", "GVPR", "WVPR",
            "OPRS", "GPRS", "OPRF", "GPRF",

            "OIR", "GIR", "WIR", "LIR", "NIR", "VIR",
            "OIRH", "GIRH", "WIRH",
            "OVIR", "GVIR", "WVIR",

            "OPI", "OPP", "GPI", "GPP", "WPI", "WPP",
        };

        return ratekw.count(&keyword[1]) > set::size_type{0};
    }

    bool is_ratio(const std::string& keyword) {
        using set = std::unordered_set<std::string>;
        static const auto ratiokw = set {
            "GLR", "GOR", "WCT",
            "GLRH", "GORH", "WCTH",
        };

        return ratiokw.count(&keyword[1]) > set::size_type{0};
    }

    bool is_total(const std::string& keyword) {
        using set = std::unordered_set<std::string>;
        static const auto totalkw = set {
            "OPT", "GPT", "WPT", "LPT", "NPT",
            "VPT", "OVPT", "GVPT", "WVPT",
            "WPTH", "OPTH", "GPTH", "LPTH",
            "GPTS", "OPTS", "GPTF", "OPTF",

            "WIT", "OIT", "GIT", "LIT", "NIT", "CIT", "VIT",
            "WITH", "OITH", "GITH", "WVIT", "OVIT", "GVIT",
        };

        return totalkw.count(&keyword[1]) > set::size_type{0};
    }

    bool is_count(const std::string& keyword) {
        using set = std::unordered_set<std::string>;
        static const auto countkw = set {
            "MWIN", "MWIT", "MWPR", "MWPT"
        };

        return countkw.count(keyword) > set::size_type{0};
    }

    bool is_region_to_region(const std::string& keyword) {
        using sz_t = std::string::size_type;
        if ((keyword.size() == sz_t{3}) && keyword[2] == 'F') return true;
        if ((keyword == "RNLF") || (keyword == "RORFR")) return true;
        if ((keyword.size() >= sz_t{4}) && ((keyword[2] == 'F') && ((keyword[3] == 'T') || (keyword[3] == 'R')))) return true;
        if ((keyword.size() >= sz_t{5}) && ((keyword[3] == 'F') && ((keyword[4] == 'T') || (keyword[4] == 'R')))) return true;

        return false;
    }

    bool is_aquifer(const std::string& keyword) {
        return keyword[0] == 'A';
    }

    SummaryNode::Type parseKeywordType(const std::string& keyword) {
        if (is_rate(keyword)) return SummaryNode::Type::Rate;
        if (is_total(keyword)) return SummaryNode::Type::Total;
        if (is_ratio(keyword)) return SummaryNode::Type::Ratio;
        if (is_pressure(keyword)) return SummaryNode::Type::Pressure;
        if (is_count(keyword)) return SummaryNode::Type::Count;

        return SummaryNode::Type::Undefined;
    }

void handleMissingWell( const ParseContext& parseContext, ErrorGuard& errors, const std::string& keyword, const std::string& well) {
    std::string msg = std::string("Error in keyword:") + keyword + std::string(" No such well: ") + well;
    if (parseContext.get( ParseContext::SUMMARY_UNKNOWN_WELL) == InputError::WARN)
        std::cerr << "ERROR: " << msg << std::endl;

    parseContext.handleError( ParseContext::SUMMARY_UNKNOWN_WELL , msg, errors );
}

void handleMissingGroup( const ParseContext& parseContext , ErrorGuard& errors, const std::string& keyword, const std::string& group) {
    std::string msg = std::string("Error in keyword:") + keyword + std::string(" No such group: ") + group;
    if (parseContext.get( ParseContext::SUMMARY_UNKNOWN_GROUP) == InputError::WARN)
        std::cerr << "ERROR: " << msg << std::endl;

    parseContext.handleError( ParseContext::SUMMARY_UNKNOWN_GROUP , msg, errors );
}

inline void keywordW( SummaryConfig::keyword_list& list,
                      const std::vector<std::string>& well_names,
                      SummaryNode baseWellParam) {
    for (const auto& wname : well_names)
        list.push_back( baseWellParam.namedEntity(wname) );
}

inline void keywordW( SummaryConfig::keyword_list& list,
                      const std::string& keyword,
                      Location loc,
                      const Schedule& schedule) {
    auto param = SummaryNode {
        keyword, SummaryNode::Category::Well , std::move(loc)
    }
    .parameterType( parseKeywordType(keyword) )
    .isUserDefined( is_udq(keyword) );

    keywordW( list, schedule.wellNames(), param );
}

inline void keywordW( SummaryConfig::keyword_list& list,
                      const ParseContext& parseContext,
                      ErrorGuard& errors,
                      const DeckKeyword& keyword,
                      const Schedule& schedule ) {
    /*
      Here is a two step check whether this keyword should be discarded as not
      supported:

      1. Well keywords ending with 'L' represent completions, they are not
      supported.

      2. If the keyword is a UDQ keyword there is no convention enforced to
      the last character, and in that case it is treated as a normal well
      keyword anyways.
    */
    if (keyword.name().back() == 'L') {
        if (!is_udq(keyword.name())) {
            const auto& location = keyword.location();
            std::string msg = std::string("The completion keywords like: " + keyword.name() + " are not supported at: " + location.filename + ", line " + std::to_string(location.lineno));
            parseContext.handleError( ParseContext::SUMMARY_UNHANDLED_KEYWORD, msg, errors);
            return;
        }
    }

    auto param = SummaryNode {
        keyword.name(), SummaryNode::Category::Well, keyword.location()
    }
    .parameterType( parseKeywordType(keyword.name()) )
    .isUserDefined( is_udq(keyword.name()) );

    if (keyword.size() && keyword.getDataRecord().getDataItem().hasValue(0)) {
        for( const std::string& pattern : keyword.getStringData()) {
          auto well_names = schedule.wellNames( pattern, schedule.size() - 1 );

            if( well_names.empty() )
                handleMissingWell( parseContext, errors, keyword.name(), pattern );

            keywordW( list, well_names, param );
        }
    } else
        keywordW( list, schedule.wellNames(), param );
}

inline void keywordG( SummaryConfig::keyword_list& list,
                      const std::string& keyword,
                      Location loc,
                      const Schedule& schedule ) {
    auto param = SummaryNode {
        keyword, SummaryNode::Category::Group, std::move(loc)
    }
    .parameterType( parseKeywordType(keyword) )
    .isUserDefined( is_udq(keyword) );

    for( const auto& group : schedule.groupNames() ) {
        if( group == "FIELD" ) continue;
        list.push_back( param.namedEntity(group) );
    }
}

inline void keywordG( SummaryConfig::keyword_list& list,
                      const ParseContext& parseContext,
                      ErrorGuard& errors,
                      const DeckKeyword& keyword,
                      const Schedule& schedule ) {

    if( keyword.name() == "GMWSET" ) return;

    auto param = SummaryNode {
        keyword.name(), SummaryNode::Category::Group, keyword.location()
    }
    .parameterType( parseKeywordType(keyword.name()) )
    .isUserDefined( is_udq(keyword.name()) );

    if( keyword.size() == 0 ||
        !keyword.getDataRecord().getDataItem().hasValue( 0 ) ) {

        for( const auto& group : schedule.groupNames() ) {
            if( group == "FIELD" ) continue;
            list.push_back( param.namedEntity(group) );
        }
        return;
    }

    const auto& item = keyword.getDataRecord().getDataItem();

    for( const std::string& group : item.getData< std::string >() ) {
        if( schedule.hasGroup( group ) )
            list.push_back( param.namedEntity(group) );
        else
            handleMissingGroup( parseContext, errors, keyword.name(), group );
    }
}

inline void keywordF( SummaryConfig::keyword_list& list,
                      const std::string& keyword,
                      Location loc) {
    auto param = SummaryNode {
        keyword, SummaryNode::Category::Field, std::move(loc)
    }
    .parameterType( parseKeywordType(keyword) )
    .isUserDefined( is_udq(keyword) );

    list.push_back( std::move(param) );
}

inline void keywordF( SummaryConfig::keyword_list& list,
                      const DeckKeyword& keyword ) {
    if( keyword.name() == "FMWSET" ) return;
    keywordF( list, keyword.name(), keyword.location() );
}

inline std::array< int, 3 > getijk( const DeckRecord& record,
                                    int offset = 0 ) {
    return {{
        record.getItem( offset + 0 ).get< int >( 0 ) - 1,
        record.getItem( offset + 1 ).get< int >( 0 ) - 1,
        record.getItem( offset + 2 ).get< int >( 0 ) - 1
    }};
}

inline std::array< int, 3 > getijk( const Connection& completion ) {
    return { { completion.getI(), completion.getJ(), completion.getK() }};
}

inline void keywordB( SummaryConfig::keyword_list& list,
                      const DeckKeyword& keyword,
                      const GridDims& dims) {
    auto param = SummaryNode {
        keyword.name(), SummaryNode::Category::Block, keyword.location()
    }
    .parameterType( parseKeywordType(keyword.name()) )
    .isUserDefined( is_udq(keyword.name()) );

  for( const auto& record : keyword ) {
      auto ijk = getijk( record );
      int global_index = 1 + dims.getGlobalIndex(ijk[0], ijk[1], ijk[2]);
      list.push_back( param.number(global_index) );
  }
}

inline void keywordR2R( SummaryConfig::keyword_list& /* list */,
                        const ParseContext& parseContext,
                        ErrorGuard& errors,
                        const DeckKeyword& keyword)
{
    const auto& location = keyword.location();
    std::string msg = "Region to region summary keyword: " + keyword.name() + " at " + location.filename + ", line " + std::to_string(location.lineno) + " is ignored";
    parseContext.handleError(ParseContext::SUMMARY_UNHANDLED_KEYWORD, msg, errors);
}

  inline void keywordR( SummaryConfig::keyword_list& list,
                        const DeckKeyword& keyword,
                        const TableManager& tables,
                        const ParseContext& parseContext,
                        ErrorGuard& errors ) {

    /* RUNSUM is not a region keyword but a directive for how to format and
     * print output. Unfortunately its *recognised* as a region keyword
     * because of its structure and position. Hence the special handling of ignoring it.
     */
    if( keyword.name() == "RUNSUM" ) return;
    if( keyword.name() == "RPTONLY" ) return;

    if( is_region_to_region(keyword.name()) ) {
        keywordR2R( list, parseContext, errors, keyword );
        return;
    }

    const size_t numfip = tables.numFIPRegions( );
    const auto& item = keyword.getDataRecord().getDataItem();
    std::vector<int> regions;

    if (item.data_size() > 0)
        regions = item.getData< int >();
    else {
        for (size_t region=1; region <= numfip; region++)
            regions.push_back( region );
    }

    // Don't (currently) need parameter type for region keywords
    auto param = SummaryNode {
        keyword.name(), SummaryNode::Category::Region, keyword.location()
    }
    .isUserDefined( is_udq(keyword.name()) );

    for( const int region : regions ) {
        if (region >= 1 && region <= static_cast<int>(numfip))
            list.push_back( param.number( region ) );
        else
            throw std::invalid_argument("Illegal region value: " + std::to_string( region ));
    }
}

inline void keywordMISC( SummaryConfig::keyword_list& list,
                         const std::string& keyword,
                         Location loc)
{
    if (meta_keywords.count(keyword) == 0)
        list.emplace_back( keyword, SummaryNode::Category::Miscellaneous , std::move(loc));
}

inline void keywordMISC( SummaryConfig::keyword_list& list,
                         const DeckKeyword& keyword)
{
    keywordMISC(list, keyword.name(), keyword.location());
}

  inline void keywordC( SummaryConfig::keyword_list& list,
                        const ParseContext& parseContext,
                        ErrorGuard& errors,
                        const DeckKeyword& keyword,
                        const Schedule& schedule,
                        const GridDims& dims) {

    auto param = SummaryNode {
        keyword.name(), SummaryNode::Category::Connection, keyword.location()
    }
    .parameterType( parseKeywordType( keyword.name()) )
    .isUserDefined( is_udq(keyword.name()) );

    for( const auto& record : keyword ) {

        const auto& wellitem = record.getItem( 0 );

        const auto well_names = wellitem.defaultApplied( 0 )
                              ? schedule.wellNames()
                              : schedule.wellNames( wellitem.getTrimmedString( 0 ) );
        const auto ijk_defaulted = record.getItem( 1 ).defaultApplied( 0 );

        if( well_names.empty() )
            handleMissingWell( parseContext, errors, keyword.name(), wellitem.getTrimmedString( 0 ) );

        for(const auto& name : well_names) {
            param.namedEntity(name);
            const auto& well = schedule.getWellatEnd(name);
            /*
             * we don't want to add completions that don't exist, so we iterate
             * over a well's completions regardless of the desired block is
             * defaulted or not
             */
            for( const auto& connection : well.getConnections() ) {
                auto cijk = getijk( connection );
                int global_index = 1 + dims.getGlobalIndex(cijk[0], cijk[1], cijk[2]);

                if( ijk_defaulted || ( cijk == getijk(record, 1) ) )
                    list.push_back( param.number(global_index) );
            }
        }
    }
}

    bool isKnownSegmentKeyword(const DeckKeyword& keyword)
    {
        const auto& kw = keyword.name();

        if (kw.size() > 4) {
            // Easy check first--handles SUMMARY and SUMTHIN &c.
            return false;
        }

        const auto kw_whitelist = std::vector<const char*> {
            "SOFR", "SGFR", "SWFR", "SPR",
        };

        return std::any_of(kw_whitelist.begin(), kw_whitelist.end(),
                           [&kw](const char* known) -> bool
                           {
                               return kw == known;
                           });
    }

    int maxNumWellSegments(const std::size_t /* last_timestep */,
                           const Well&       well)
    {
        return well.isMultiSegment()
            ? well.getSegments().size() : 0;
    }

    void makeSegmentNodes(const std::size_t               last_timestep,
                          const int                       segID,
                          const DeckKeyword&              keyword,
                          const Well&                    well,
                          SummaryConfig::keyword_list&    list)
    {
        if (!well.isMultiSegment())
            // Not an MSW.  Don't create summary vectors for segments.
            return;

        auto param = SummaryNode {
            keyword.name(), SummaryNode::Category::Segment, keyword.location()
        }
        .namedEntity( well.name() )
        .isUserDefined( is_udq(keyword.name()) );

        if (segID < 1) {
            // Segment number defaulted.  Allocate a summary
            // vector for each segment.
            const auto nSeg = maxNumWellSegments(last_timestep, well);

            for (auto segNumber = 0*nSeg; segNumber < nSeg; ++segNumber)
                list.push_back( param.number(segNumber + 1) );
        }
        else
            // Segment number specified.  Allocate single
            // summary vector for that segment number.
            list.push_back( param.number(segID) );
    }

    void keywordSNoRecords(const std::size_t            last_timestep,
                           const DeckKeyword&           keyword,
                           const Schedule&              schedule,
                           SummaryConfig::keyword_list& list)
    {
        // No keyword records.  Allocate summary vectors for all
        // segments in all wells at all times.
        //
        // Expected format:
        //
        //   SGFR
        //   / -- All segments in all MS wells at all times.

        const auto segID = -1;

        for (const auto& well : schedule.getWellsatEnd())
            makeSegmentNodes(last_timestep, segID, keyword,
                             well, list);
    }

    void keywordSWithRecords(const std::size_t            last_timestep,
                             const ParseContext&          parseContext,
                             ErrorGuard& errors,
                             const DeckKeyword&           keyword,
                             const Schedule&              schedule,
                             SummaryConfig::keyword_list& list)
    {
        // Keyword has explicit records.  Process those and create
        // segment-related summary vectors for those wells/segments
        // that match the description.
        //
        // Expected formats:
        //
        //   SOFR
        //     'W1'   1 /
        //     'W1'  10 /
        //     'W3'     / -- All segments
        //   /
        //
        //   SPR
        //     1*   2 / -- Segment 2 in all multi-segmented wells
        //   /

        for (const auto& record : keyword) {
            const auto& wellitem = record.getItem(0);
            const auto& well_names = wellitem.defaultApplied(0)
                ? schedule.wellNames()
                : schedule.wellNames(wellitem.getTrimmedString(0));

            if (well_names.empty())
                handleMissingWell(parseContext, errors, keyword.name(),
                                  wellitem.getTrimmedString(0));

            // Negative 1 (< 0) if segment ID defaulted.  Defaulted
            // segment number in record implies all segments.
            const auto segID = record.getItem(1).defaultApplied(0)
                ? -1 : record.getItem(1).get<int>(0);

            for (const auto& well_name : well_names)
                makeSegmentNodes(last_timestep, segID, keyword, schedule.getWellatEnd(well_name), list);
        }
    }

    inline void keywordS(SummaryConfig::keyword_list& list,
                         const ParseContext&          parseContext,
                         ErrorGuard& errors,
                         const DeckKeyword&           keyword,
                         const Schedule&              schedule)
    {
        // Generate SMSPEC nodes for SUMMARY keywords of the form
        //
        //   SOFR
        //     'W1'   1 /
        //     'W1'  10 /
        //     'W3'     / -- All segments
        //   /
        //
        //   SPR
        //     1*   2 / -- Segment 2 in all multi-segmented wells
        //   /
        //
        //   SGFR
        //   / -- All segments in all MS wells at all times.

        if (! isKnownSegmentKeyword(keyword)) {
            // Ignore keywords that have not been explicitly white-listed
            // for treatment as segment summary vectors.
            return;
        }

        const auto last_timestep = schedule.getTimeMap().last();

        if (keyword.size() > 0) {
            // Keyword with explicit records.
            // Handle as alternatives SOFR and SPR above
            keywordSWithRecords(last_timestep, parseContext, errors,
                                keyword, schedule, list);
        }
        else {
            // Keyword with no explicit records.
            // Handle as alternative SGFR above.
            keywordSNoRecords(last_timestep, keyword, schedule, list);
        }
    }

    std::string to_string(const SummaryNode::Category cat) {
        switch( cat ) {
            case SummaryNode::Category::Well: return "Well";
            case SummaryNode::Category::Group: return "Group";
            case SummaryNode::Category::Field: return "Field";
            case SummaryNode::Category::Region: return "Region";
            case SummaryNode::Category::Block: return "Block";
            case SummaryNode::Category::Connection: return "Connection";
            case SummaryNode::Category::Segment: return "Segment";
            case SummaryNode::Category::Miscellaneous: return "Miscellaneous";
        }

        throw std::invalid_argument {
            "Unhandled Summary Parameter Category '"
            + std::to_string(static_cast<int>(cat)) + '\''
        };
    }

    void check_udq( const std::string& name,
                    const Schedule& schedule,
                    const ParseContext& parseContext,
                    ErrorGuard& errors ) {
        if (! is_udq(name))
            // Nothing to do
            return;

        const auto& udq = schedule.getUDQConfig(schedule.size() - 1);

        if (!udq.has_keyword(name)) {
            std::string msg{"Summary output has been requested for UDQ keyword: " + name + " but it has not been configured"};
            parseContext.handleError(ParseContext::SUMMARY_UNDEFINED_UDQ, msg, errors);
            return;
        }

        if (!udq.has_unit(name)) {
            std::string msg{"Summary output has been requested for UDQ keyword: " + name + " but no unit has not been configured"};
            parseContext.handleError(ParseContext::SUMMARY_UDQ_MISSING_UNIT, msg, errors);
        }
    }

  inline void handleKW( SummaryConfig::keyword_list& list,
                        const DeckKeyword& keyword,
                        const Schedule& schedule,
                        const TableManager& tables,
                        const ParseContext& parseContext,
                        ErrorGuard& errors,
                        const GridDims& dims) {
    using Cat = SummaryNode::Category;

    const auto& name = keyword.name();
    check_udq( name, schedule, parseContext, errors );

    const auto cat = parseKeywordCategory( name );
    switch( cat ) {
        case Cat::Well: return keywordW( list, parseContext, errors, keyword, schedule );
        case Cat::Group: return keywordG( list, parseContext, errors, keyword, schedule );
        case Cat::Field: return keywordF( list, keyword );
        case Cat::Block: return keywordB( list, keyword, dims );
        case Cat::Region: return keywordR( list, keyword, tables, parseContext, errors );
        case Cat::Connection: return keywordC( list, parseContext, errors, keyword, schedule, dims);
        case Cat::Segment: return keywordS( list, parseContext, errors, keyword, schedule );
        case Cat::Miscellaneous: return keywordMISC( list, keyword );

        default:
            std::string msg = "Summary keywords of type: " + to_string( cat ) + " is not supported. Keyword: " + name + " is ignored";
            parseContext.handleError(ParseContext::SUMMARY_UNHANDLED_KEYWORD, msg, errors);
            return;
    }
}

inline void handleKW( SummaryConfig::keyword_list& list,
                      const std::string& keyword,
                      Location loc,
                      const Schedule& schedule,
                      const ParseContext& parseContext,
                      ErrorGuard& errors) {

    if (is_udq(keyword))
        throw std::logic_error("UDQ keywords not handleded when expanding alias list");

    if (is_aquifer( keyword )) {
        std::string msg = "Summary keywords of type: Aquifer is not supported. Keyword: " + keyword + " is ignored";
        parseContext.handleError(ParseContext::SUMMARY_UNHANDLED_KEYWORD, msg, errors);
        return;
    }

    using Cat = SummaryNode::Category;
    const auto cat = parseKeywordCategory( keyword );

    switch( cat ) {
        case Cat::Well: return keywordW( list, keyword, std::move(loc), schedule );
        case Cat::Group: return keywordG( list, keyword, std::move(loc), schedule );
        case Cat::Field: return keywordF( list, keyword, std::move(loc) );
        case Cat::Miscellaneous: return keywordMISC( list, keyword, std::move(loc));

        default:
            throw std::logic_error("Keyword type: " + to_string( cat ) + " is not supported in alias lists. Internal error handling: " + keyword);
    }
}

  inline void uniq( SummaryConfig::keyword_list& vec ) {
    std::sort( vec.begin(), vec.end() );
    auto logical_end = std::unique( vec.begin(), vec.end() );
    vec.erase( logical_end, vec.end() );
  }
}

// =====================================================================

SummaryNode::Category parseKeywordCategory(const std::string& keyword) {
    using Cat = SummaryNode::Category;

    if (is_special(keyword)) { return Cat::Miscellaneous; }

    switch (keyword[0]) {
        case 'W': return Cat::Well;
        case 'G': return Cat::Group;
        case 'F': return Cat::Field;
        case 'C': return Cat::Connection;
        case 'R': return Cat::Region;
        case 'B': return Cat::Block;
        case 'S': return Cat::Segment;
    }

    // TCPU, MLINEARS, NEWTON, &c
    return Cat::Miscellaneous;
}

SummaryNode::SummaryNode(std::string keyword, const Category cat, Location loc_arg) :
    keyword_(std::move(keyword)),
    category_(cat),
    loc(std::move(loc_arg))
{}

SummaryNode& SummaryNode::parameterType(const Type type)
{
    this->type_ = type;
    return *this;
}

SummaryNode& SummaryNode::namedEntity(std::string name)
{
    this->name_ = std::move(name);
    return *this;
}

SummaryNode& SummaryNode::number(const int num)
{
    this->number_ = num;
    return *this;
}

SummaryNode& SummaryNode::isUserDefined(const bool userDefined)
{
    this->userDefined_ = userDefined;
    return *this;
}

std::string SummaryNode::uniqueNodeKey() const
{
    switch (this->category()) {
    case SummaryNode::Category::Well: // fall-through
    case SummaryNode::Category::Group:
        return this->keyword() + ':' + this->namedEntity();

    case SummaryNode::Category::Field: // fall-through
    case SummaryNode::Category::Miscellaneous:
        return this->keyword();

    case SummaryNode::Category::Region: // fall-through
    case SummaryNode::Category::Block:
        return this->keyword() + ':' + std::to_string(this->number());

    case SummaryNode::Category::Connection: // fall-through
    case SummaryNode::Category::Segment:
        return this->keyword() + ':' + this->namedEntity() + ':' + std::to_string(this->number());
    }

    throw std::invalid_argument {
        "Unhandled Summary Parameter Category '"
        + to_string(this->category()) + '\''
    };
}

bool operator==(const SummaryNode& lhs, const SummaryNode& rhs)
{
    if (lhs.keyword() != rhs.keyword()) return false;

    assert (lhs.category() == rhs.category());

    switch( lhs.category() ) {
        case SummaryNode::Category::Field: // fall-through
        case SummaryNode::Category::Miscellaneous:
            // Fully identified by keyword
            return true;

        case SummaryNode::Category::Well:  // fall-through
        case SummaryNode::Category::Group:
            // Equal if associated to same named entity
            return lhs.namedEntity() == rhs.namedEntity();

        case SummaryNode::Category::Region:  // fall-through
        case SummaryNode::Category::Block:
            // Equal if associated to same numeric entity
            return lhs.number() == rhs.number();

        case SummaryNode::Category::Connection:  // fall-through
        case SummaryNode::Category::Segment:
            // Equal if associated to same numeric
            // sub-entity of same named entity
            return (lhs.namedEntity() == rhs.namedEntity())
                && (lhs.number()      == rhs.number());
    }

    return false;
}

bool operator<(const SummaryNode& lhs, const SummaryNode& rhs)
{
    if (lhs.keyword() < rhs.keyword()) return true;
    if (rhs.keyword() < lhs.keyword()) return false;

    // If we get here, the keyword are equal.

    switch( lhs.category() ) {
        case SummaryNode::Category::Field:  // fall-through
        case SummaryNode::Category::Miscellaneous:
            // Fully identified by keyword.
            // Return false for equal keywords.
            return false;

        case SummaryNode::Category::Well:  // fall-through
        case SummaryNode::Category::Group:
            // Ordering determined by namedEntityd entity
            return lhs.namedEntity() < rhs.namedEntity();

        case SummaryNode::Category::Region:  // fall-through
        case SummaryNode::Category::Block:
            // Ordering determined by numeric entity
            return lhs.number() < rhs.number();

        case SummaryNode::Category::Connection:  // fall-through
        case SummaryNode::Category::Segment:
        {
            // Ordering determined by pair of namedEntity and numeric ID.
            //
            // Would ideally implement this in terms of operator< for
            // std::tuple<std::string,int>, with objects generated by std::tie().
            const auto& lnm = lhs.namedEntity();
            const auto& rnm = rhs.namedEntity();

            return ( lnm <  rnm)
                || ((lnm == rnm) && (lhs.number() < rhs.number()));
        }
    }

    throw std::invalid_argument {
        "Unhandled Summary Parameter Category '" + to_string(lhs.category()) + '\''
    };
}

// =====================================================================

SummaryConfig::SummaryConfig( const Deck& deck,
                              const Schedule& schedule,
                              const TableManager& tables,
                              const ParseContext& parseContext,
                              ErrorGuard& errors,
                              const GridDims& dims) {
    SUMMARYSection section( deck );

    // The kw_iter++ hoops is to skip the initial 'SUMMARY' keyword.
    auto kw_iter = section.begin();
    if (kw_iter != section.end())
        kw_iter++;

    for(; kw_iter != section.end(); ++kw_iter) {
        const auto& kw = *kw_iter;
        handleKW( this->keywords, kw, schedule, tables, parseContext, errors, dims);
    }

    for (const auto& meta_pair : meta_keywords) {
        if( section.hasKeyword( meta_pair.first ) ) {
            const auto& deck_keyword = section.getKeyword(meta_pair.first);
            for (const auto& kw : meta_pair.second) {
                if (!this->hasKeyword(kw))
                    handleKW(this->keywords, kw, deck_keyword.location(), schedule, parseContext, errors);
            }
        }
    }

    uniq( this->keywords );
    for (const auto& kw: this->keywords) {
        this->short_keywords.insert( kw.keyword() );
        this->summary_keywords.insert( kw.uniqueNodeKey() );
    }
}

SummaryConfig::SummaryConfig( const Deck& deck,
                              const Schedule& schedule,
                              const TableManager& tables,
                              const ParseContext& parseContext,
                              ErrorGuard& errors) :
    SummaryConfig( deck , schedule, tables, parseContext, errors, GridDims( deck ))
{ }

template <typename T>
SummaryConfig::SummaryConfig( const Deck& deck,
                              const Schedule& schedule,
                              const TableManager& tables,
                              const ParseContext& parseContext,
                              T&& errors) :
    SummaryConfig(deck, schedule, tables, parseContext, errors)
{}

SummaryConfig::SummaryConfig( const Deck& deck,
               const Schedule& schedule,
               const TableManager& tables) :
    SummaryConfig(deck, schedule, tables, ParseContext(), ErrorGuard())
{}

SummaryConfig::SummaryConfig(const keyword_list& kwds,
                             const std::set<std::string>& shortKwds,
                             const std::set<std::string>& smryKwds) :
    keywords(kwds), short_keywords(shortKwds), summary_keywords(smryKwds)
{}

SummaryConfig::const_iterator SummaryConfig::begin() const {
    return this->keywords.cbegin();
}

SummaryConfig::const_iterator SummaryConfig::end() const {
    return this->keywords.cend();
}

SummaryConfig& SummaryConfig::merge( const SummaryConfig& other ) {
    this->keywords.insert( this->keywords.end(),
                            other.keywords.begin(),
                            other.keywords.end() );

    uniq( this->keywords );
    return *this;
}

SummaryConfig& SummaryConfig::merge( SummaryConfig&& other ) {
    auto fst = std::make_move_iterator( other.keywords.begin() );
    auto lst = std::make_move_iterator( other.keywords.end() );
    this->keywords.insert( this->keywords.end(), fst, lst );
    other.keywords.clear();

    uniq( this->keywords );
    return *this;
}

bool SummaryConfig::hasKeyword( const std::string& keyword ) const {
    return (this->short_keywords.count( keyword ) == 1);
}

bool SummaryConfig::hasSummaryKey(const std::string& keyword ) const {
    return (this->summary_keywords.count( keyword ) == 1);
}

size_t SummaryConfig::size() const {
    return this->keywords.size();
}

/*
  Can be used to query if a certain 3D field, e.g. PRESSURE, is
  required to calculate the summary variables.

  The implementation is based on the hardcoded datastructure
  required_fields defined in a anonymous namespaces at the top of this
  file; the content of this datastructure again is based on the
  implementation of the Summary calculations in the ewoms-eclio
  repository: ewoms/eclio/output/Summary.cpp.
*/

bool SummaryConfig::require3DField( const std::string& keyword ) const {
    const auto iter = required_fields.find( keyword );
    if (iter == required_fields.end())
        return false;

    for (const auto& kw : iter->second) {
        if (this->hasKeyword( kw ))
            return true;
    }

    return false;
}

bool SummaryConfig::requireFIPNUM( ) const {
    return this->hasKeyword("ROIP")  ||
           this->hasKeyword("ROIPL") ||
           this->hasKeyword("RGIP")  ||
           this->hasKeyword("RGIPL") ||
           this->hasKeyword("RGIPG") ||
           this->hasKeyword("RWIP")  ||
           this->hasKeyword("RPR");
}

const SummaryConfig::keyword_list& SummaryConfig::getKwds() const {
    return keywords;
}

const std::set<std::string>& SummaryConfig::getShortKwds() const {
    return short_keywords;
}

const std::set<std::string>& SummaryConfig::getSmryKwds() const {
    return summary_keywords;
}

bool SummaryConfig::operator==(const Ewoms::SummaryConfig& data) const {
    return this->getKwds() == data.getKwds() &&
           this->getShortKwds() == data.getShortKwds() &&
           this->getSmryKwds() == data.getSmryKwds();
}

}
