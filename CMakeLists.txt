# -*- mode: cmake; tab-width: 2; indent-tabs-mode: t; truncate-lines: t
# vim: set filetype=cmake autoindent tabstop=2 shiftwidth=2 noexpandtab softtabstop=2 nowrap:
cmake_minimum_required(VERSION 3.10)

# set up project and specify the minimum cmake version
project("ewoms-eclio" C CXX)

# find the build system (i.e., ewoms-common) and set cmake's module path
find_package(ewoms-common REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ewoms-common_MODULE_PATH})

# include the eWoms cmake macros
include(EwomsMacros)

# do most of the book-keeping required
ewoms_project()

# find the packages needed to compile the module
find_package(Boost COMPONENTS regex system filesystem unit_test_framework REQUIRED)

# we want all features detected by the build system to be enabled,
# thank you!
dune_enable_all_packages()

# set up the library for ECL I/O
ewoms_recursive_add_library("ewomseclio" "ewoms/eclio")
target_link_libraries("ewomseclio")

# recursively mark all header files beneath the "ewoms" directory for
# installation.
ewoms_recusive_export_all_headers("ewoms")

# the code generator
file(GLOB_RECURSE genkw_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "genkw/*.cc" "genkw/*.c")
ewoms_add_application("genkw" SOURCES "${genkw_SOURCES}")

# comparison tool for binary ECL files
ewoms_add_application(utils
  SOURCES utils/compareecl.cc utils/eclregressiontest.cc utils/eclfilescomparator.cc)

# conversion tool for binary ECL to ASCII files
ewoms_add_application(convert_ecl SOURCES utils/convertecl.cc)

# tool for inspecting summary files
ewoms_add_application(inspect_summary SOURCES utils/summary.cc)

ewoms_add_test(calculateCellVol SOURCES tests/test_calculatecellvol.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(cmp SOURCES tests/test_cmp.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(cubic SOURCES tests/test_cubic.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(messagelimiter SOURCES tests/test_messagelimiter.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(nonuniformtablelinear SOURCES tests/test_nonuniformtablelinear.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(OpmLog SOURCES tests/test_opmlog.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SimulationDataContainer SOURCES tests/test_simulationdatacontainer.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(sparsevector SOURCES tests/test_sparsevector.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(uniformtablelinear SOURCES tests/test_uniformtablelinear.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ACTIONX SOURCES tests/actionx.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ADDREGTests SOURCES tests/addregtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AquiferCTTests SOURCES tests/aquifercttests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AquifetpTests SOURCES tests/aquifetptests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AqudimsTests SOURCES tests/aqudimstests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AquanconTests SOURCES tests/aquancontests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(BoxTests SOURCES tests/boxtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ColumnSchemaTests SOURCES tests/columnschematests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ConnectionTests SOURCES tests/connectiontests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(COMPSEGUnits SOURCES tests/compsegunits.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(CopyRegTests SOURCES tests/copyregtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DeckValueTests SOURCES tests/deckvaluetests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DeckTests SOURCES tests/decktests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DynamicStateTests SOURCES tests/dynamicstatetests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DynamicVectorTests SOURCES tests/dynamicvectortests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Eclipse3DPropertiesTests SOURCES tests/eclipse3dpropertiestests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EclipseGridTests SOURCES tests/eclipsegridtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EqualRegTests SOURCES tests/equalregtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EventTests SOURCES tests/eventtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FaceDirTests SOURCES tests/facedirtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FaultTests SOURCES tests/faulttests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FieldPropsTests SOURCES tests/fieldpropstests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FoamTests SOURCES tests/foamtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FunctionalTests SOURCES tests/functionaltests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(GeomodifierTests SOURCES tests/geomodifiertests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(GridPropertyTests SOURCES tests/gridpropertytests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(GroupTests SOURCES tests/grouptests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(InitConfigTest SOURCES tests/initconfigtest.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(IOConfigTests SOURCES tests/ioconfigtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MessageLimitTests SOURCES tests/messagelimittests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MultiRegTests SOURCES tests/multiregtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MultisegmentWellTests SOURCES tests/multisegmentwelltests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MULTREGTScannerTests SOURCES tests/multregtscannertests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(OrderedMapTests SOURCES tests/orderedmaptests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ParseContextTests SOURCES tests/parsecontexttests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ParseContext_EXIT1 SOURCES tests/parsecontext_exit1.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ParseDATAWithDefault SOURCES tests/parsedatawithdefault.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(PORVTests SOURCES tests/porvtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RawKeywordTests SOURCES tests/rawkeywordtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ResinsightTest SOURCES tests/resinsighttest.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RestartConfigTests SOURCES tests/restartconfigtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RockTableTests SOURCES tests/rocktabletests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RunspecTests SOURCES tests/runspectests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SatfuncPropertyInitializersTests SOURCES tests/satfuncpropertyinitializerstests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ScheduleTests SOURCES tests/scheduletests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SectionTests SOURCES tests/sectiontests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SimpleTableTests SOURCES tests/simpletabletests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SimulationConfigTest SOURCES tests/simulationconfigtest.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(StarTokenTests SOURCES tests/startokentests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(StringTests SOURCES tests/stringtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SummaryConfigTests SOURCES tests/summaryconfigtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TabdimsTests SOURCES tests/tabdimstests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableColumnTests SOURCES tests/tablecolumntests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableContainerTests SOURCES tests/tablecontainertests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableManagerTests SOURCES tests/tablemanagertests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableSchemaTests SOURCES tests/tableschematests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ThresholdPressureTest SOURCES tests/thresholdpressuretest.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TimeMapTest SOURCES tests/timemaptest.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TransMultTests SOURCES tests/transmulttests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TuningTests SOURCES tests/tuningtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(UDQTests SOURCES tests/udqtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(UnitTests SOURCES tests/unittests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ValueTests SOURCES tests/valuetests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WellSolventTests SOURCES tests/wellsolventtests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WellTracerTests SOURCES tests/welltracertests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WellTests SOURCES tests/welltests.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WLIST SOURCES tests/wlist.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WTEST SOURCES tests/wtest.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateActionxData SOURCES tests/test_aggregateactionxdata.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateWellData SOURCES tests/test_aggregatewelldata.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateGroupData SOURCES tests/test_aggregategroupdata.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateMSWData SOURCES tests/test_aggregatemswdata.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateConnectionData SOURCES tests/test_aggregateconnectiondata.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateUDQData SOURCES tests/test_aggregateudqdata.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ArrayDimChecker SOURCES tests/test_arraydimchecker.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EclipseIO SOURCES tests/test_eclipseio.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DoubHEAD SOURCES tests/test_doubhead.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(InteHEAD SOURCES tests/test_intehead.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(LinearisedOutputTable SOURCES tests/test_linearisedoutputtable.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(LogiHEAD SOURCES tests/test_logihead.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(OutputStream SOURCES tests/test_outputstream.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(regionCache SOURCES tests/test_regioncache.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(PaddedOutputString SOURCES tests/test_paddedoutputstring.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Restart SOURCES tests/test_restart.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RFT SOURCES tests/test_rft.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Solution SOURCES tests/test_solution.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Summary SOURCES tests/test_summary.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Tables SOURCES tests/test_tables.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Wells SOURCES tests/test_wells.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WindowedArray SOURCES tests/test_windowedarray.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(restartwellinfo SOURCES tests/test_restartwellinfo.cc LIBRARIES "${Boost_LIBRARIES}")

# "extra" tests
set(_testdir ${PROJECT_SOURCE_DIR}/tests/data)

ewoms_add_test(ParserTests
  SOURCES tests/parsertests.cc
  TEST_ARGS "--" ${_testdir}/
  LIBRARIES "${Boost_LIBRARIES}")

ewoms_add_test(ParserIncludeTests
  SOURCES tests/parserincludetests.cc
  LIBRARIES "${Boost_LIBRARIES}"
  TEST_ARGS "--" ${_testdir}/parser/)
target_compile_definitions(ParserIncludeTests PRIVATE -DHAVE_CASE_SENSITIVE_FILESYSTEM=1)

ewoms_add_test(PvtxTableTests
  SOURCES tests/pvtxtabletests.cc
  LIBRARIES "${Boost_LIBRARIES}"
  TEST_ARGS "--" ${_testdir}/integration_tests/)

ewoms_add_test(EclipseStateTests
  SOURCES tests/eclipsestatetests.cc
  LIBRARIES "${Boost_LIBRARIES}"
  TEST_ARGS "--" ${_testdir}/integration_tests/)

foreach(TEST_NAME BoxTest
    CheckDeckValidity
    EclipseGridCreateFromDeck
    EDITNNCTests
    IncludeTest
    IntegrationTests
    IOConfigIntegrationTest
    NNCTests
    ParseKEYWORD
    Polymer
    ScheduleCreateFromDeck
    TransMultIntegrationTests)

  string(TOLOWER "${TEST_NAME}.cc" "TEST_SOURCE_FILE")
  message("${TEST_NAME} -> tests/integration/${TEST_SOURCE_FILE}")
  ewoms_add_test(${TEST_NAME}
    SOURCES tests/integration/${TEST_SOURCE_FILE}
    LIBRARIES "${Boost_LIBRARIES}"
    TEST_ARGS "--" ${_testdir}/integration_tests/)
endforeach ()

ewoms_add_test(jsonTests
  SOURCES tests/json/jsontests.cc
  LIBRARIES "${Boost_LIBRARIES}"
  TEST_ARGS "--" ${PROJECT_SOURCE_DIR}/tests/json/example1.json)

ewoms_add_test(EclFilesComparator
  SOURCES
  tests/test_eclfilescomparator.cc
  utils/eclfilescomparator.cc
  LIBRARIES "${Boost_LIBRARIES}")

ewoms_add_test(EclRegressionTest
  SOURCES
  tests/test_eclregressiontest.cc
  utils/eclfilescomparator.cc
  utils/eclregressiontest.cc
  LIBRARIES "${Boost_LIBRARIES}")

foreach(TEST_NAME EclIO EGrid ERft ERst ESmry)
  string(TOLOWER "test_${TEST_NAME}.cc" "TEST_SOURCE_FILE")
  ewoms_add_test(${TEST_NAME}
    SOURCES "tests/${TEST_SOURCE_FILE}"
    LIBRARIES "${Boost_LIBRARIES}")
endforeach()

ewoms_add_test(RootFinders
  SOURCES tests/test_rootfinders.cc
  LIBRARIES "${Boost_LIBRARIES}")

# deal with the data required by the unit tests
ewoms_recusive_copy_testdata_to_builddir("tests/SPE*" "tests/ECLFILE*" "tests/*.json" "tests/*.SMSPEC" "tests/*.EGRID" "tests/*.txt " "tests/*.out" "tests/*.grdecl" "tests/*.data" "tests/*.DATA")

# finalize the project, e.g. generate the config.h etc.
finalize_ewoms_project()
