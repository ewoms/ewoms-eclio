# -*- mode: cmake; tab-width: 2; indent-tabs-mode: t; truncate-lines: t
# vim: set filetype=cmake autoindent tabstop=2 shiftwidth=2 noexpandtab softtabstop=2 nowrap:
cmake_minimum_required(VERSION 3.5)

# set up project and specify the minimum cmake version
project("ewoms-eclio" C CXX)

# find the build system (i.e., ewoms-common) and set cmake's module path
find_package(ewoms-common REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ewoms-common_MODULE_PATH})

# include the eWoms cmake macros
include(EwomsMacros NO_POLICY_SCOPE)

# do most of the book-keeping required
ewoms_project()

# the boost base package is required to compile ewoms-eclio, the unit
# testing framework is optional
find_package(Boost REQUIRED OPTIONAL_COMPONENTS unit_test_framework)

# either std::filesystem or std::experimental::filesystem  needs to be supported
find_package(StdFilesystem REQUIRED)
if(STD_FILESYSTEM_LIBRARIES)
  dune_register_package_flags(
    LIBRARIES "${STD_FILESYSTEM_LIBRARIES}")
endif()

# either std::optional or std::experimental::optional needs to be supported
find_package(StdOptional REQUIRED)

# either std::variant or std::experimental::variant needs to be supported
find_package(StdVariant REQUIRED)

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

ewoms_add_test(ActiveGridCells SOURCES tests/test_activegridcells.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AquiferTests SOURCES tests/aquifertests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(calculateCellVol SOURCES tests/test_calculatecellvol.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(rst SOURCES tests/test_rst.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(rst_spe1 SOURCES tests/rst_test.cc TEST_ARGS "SPE1CASE2.DATA" "SPE1CASE2_RESTART.DATA" CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(rst_msw SOURCES tests/rst_test.cc TEST_ARGS "MSW.DATA" "MSW_RESTART.DATA" CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(cmp SOURCES tests/test_cmp.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(cubic SOURCES tests/test_cubic.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(messagelimiter SOURCES tests/test_messagelimiter.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(nonuniformtablelinear SOURCES tests/test_nonuniformtablelinear.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(OpmLog SOURCES tests/test_opmlog.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(sparsevector SOURCES tests/test_sparsevector.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(uniformtablelinear SOURCES tests/test_uniformtablelinear.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ACTIONX SOURCES tests/actionx.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ADDREGTests SOURCES tests/addregtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(BoxTests SOURCES tests/boxtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ColumnSchemaTests SOURCES tests/columnschematests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ConnectionTests SOURCES tests/connectiontests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(COMPSEGUnits SOURCES tests/compsegunits.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(CopyRegTests SOURCES tests/copyregtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DeckValueTests SOURCES tests/deckvaluetests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DeckTests SOURCES tests/decktests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DynamicStateTests SOURCES tests/dynamicstatetests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DynamicVectorTests SOURCES tests/dynamicvectortests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EclipseGridTests SOURCES tests/eclipsegridtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EqualRegTests SOURCES tests/equalregtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EventTests SOURCES tests/eventtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FaceDirTests SOURCES tests/facedirtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FaultTests SOURCES tests/faulttests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FieldPropsTests SOURCES tests/fieldpropstests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FoamTests SOURCES tests/foamtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(FunctionalTests SOURCES tests/functionaltests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(GeomodifierTests SOURCES tests/geomodifiertests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(GroupTests SOURCES tests/grouptests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(InitConfigTest SOURCES tests/initconfigtest.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(IOConfigTests SOURCES tests/ioconfigtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MessageLimitTests SOURCES tests/messagelimittests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MultiRegTests SOURCES tests/multiregtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MultisegmentWellTests SOURCES tests/multisegmentwelltests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(MULTREGTScannerTests SOURCES tests/multregtscannertests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(OrderedMapTests SOURCES tests/orderedmaptests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ParseContextTests SOURCES tests/parsecontexttests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ParseContext_EXIT1 SOURCES tests/parsecontext_exit1.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ParseDATAWithDefault SOURCES tests/parsedatawithdefault.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ReportConfig SOURCES tests/test_reportconfig.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RawKeywordTests SOURCES tests/rawkeywordtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ResinsightTest SOURCES tests/resinsighttest.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RestartConfigTests SOURCES tests/restartconfigtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RockTableTests SOURCES tests/rocktabletests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RunspecTests SOURCES tests/runspectests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ScheduleRestartTests SOURCES tests/schedulerestarttests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SaltTableTests SOURCES tests/salttabletests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ScheduleTests SOURCES tests/scheduletests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SectionTests SOURCES tests/sectiontests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SimpleTableTests SOURCES tests/simpletabletests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SimulationConfigTest SOURCES tests/simulationconfigtest.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(StarTokenTests SOURCES tests/startokentests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(StringTests SOURCES tests/stringtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SummaryConfigTests SOURCES tests/summaryconfigtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Summary_Group SOURCES tests/test_summary_group.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(SummaryNode SOURCES tests/test_summarynode.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TabdimsTests SOURCES tests/tabdimstests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableColumnTests SOURCES tests/tablecolumntests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableContainerTests SOURCES tests/tablecontainertests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableManagerTests SOURCES tests/tablemanagertests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TableSchemaTests SOURCES tests/tableschematests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ThresholdPressureTest SOURCES tests/thresholdpressuretest.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TimeMapTest SOURCES tests/timemaptest.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TracerTests SOURCES tests/tracertests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TransMultTests SOURCES tests/transmulttests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(TuningTests SOURCES tests/tuningtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(UDQTests SOURCES tests/udqtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(UnitTests SOURCES tests/unittests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ValueTests SOURCES tests/valuetests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WellSolventTests SOURCES tests/wellsolventtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WellTracerTests SOURCES tests/welltracertests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WellTests SOURCES tests/welltests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WLIST SOURCES tests/wlist.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WTEST SOURCES tests/wtest.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateActionxData SOURCES tests/test_aggregateactionxdata.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateWellData SOURCES tests/test_aggregatewelldata.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateGroupData SOURCES tests/test_aggregategroupdata.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateMSWData SOURCES tests/test_aggregatemswdata.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateConnectionData SOURCES tests/test_aggregateconnectiondata.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(AggregateUDQData SOURCES tests/test_aggregateudqdata.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ArrayDimChecker SOURCES tests/test_arraydimchecker.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(EclipseIO SOURCES tests/test_eclipseio.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(DoubHEAD SOURCES tests/test_doubhead.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(InteHEAD SOURCES tests/test_intehead.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(LinearisedOutputTable SOURCES tests/test_linearisedoutputtable.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(LogiHEAD SOURCES tests/test_logihead.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(OutputStream SOURCES tests/test_outputstream.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(regionCache SOURCES tests/test_regioncache.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(PaddedOutputString SOURCES tests/test_paddedoutputstring.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Restart SOURCES tests/test_restart.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RFT SOURCES tests/test_rft.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(RFTConfigTests SOURCES tests/rftconfigtests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Solution SOURCES tests/test_solution.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Summary SOURCES tests/test_summary.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Tables SOURCES tests/test_tables.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(Wells SOURCES tests/test_wells.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(WindowedArray SOURCES tests/test_windowedarray.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(restartwellinfo SOURCES tests/test_restartwellinfo.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(data_GuideRateValue SOURCES tests/test_data_guideratevalue.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(NetworkTests SOURCES tests/networktests.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(param SOURCES tests/test_param.cc CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND LIBRARIES "${Boost_LIBRARIES}")

# "extra" tests
set(_testdir ${PROJECT_SOURCE_DIR}/tests/data)

ewoms_add_test(ParserTests
  SOURCES tests/parsertests.cc
  TEST_ARGS "--" ${_testdir}/
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
  LIBRARIES "${Boost_LIBRARIES}")

ewoms_add_test(ParserIncludeTests
  SOURCES tests/parserincludetests.cc
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
  LIBRARIES "${Boost_LIBRARIES}"
  TEST_ARGS "--" ${_testdir}/parser/)
if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
  target_compile_definitions(ParserIncludeTests PRIVATE -DHAVE_CASE_SENSITIVE_FILESYSTEM=1)
endif()

ewoms_add_test(PvtxTableTests
  SOURCES tests/pvtxtabletests.cc
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
  LIBRARIES "${Boost_LIBRARIES}"
  TEST_ARGS "--" ${_testdir}/integration_tests/)

ewoms_add_test(EclipseStateTests
  SOURCES tests/eclipsestatetests.cc
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
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
  ewoms_add_test(${TEST_NAME}
    SOURCES tests/integration/${TEST_SOURCE_FILE}
    CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
    LIBRARIES "${Boost_LIBRARIES}"
    TEST_ARGS "--" ${_testdir}/integration_tests/)
endforeach ()

ewoms_add_test(jsonTests
  SOURCES tests/json/jsontests.cc
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
  LIBRARIES "${Boost_LIBRARIES}"
  TEST_ARGS "--" ${PROJECT_SOURCE_DIR}/tests/json/example1.json)

ewoms_add_test(EclFilesComparator
  SOURCES
  tests/test_eclfilescomparator.cc
  utils/eclfilescomparator.cc
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
  LIBRARIES "${Boost_LIBRARIES}")

ewoms_add_test(EclRegressionTest
  SOURCES
  tests/test_eclregressiontest.cc
  utils/eclfilescomparator.cc
  utils/eclregressiontest.cc
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
LIBRARIES "${Boost_LIBRARIES}")

foreach(TEST_NAME EclIO EGrid ERft ERsm ERst ESmry)
  string(TOLOWER "test_${TEST_NAME}.cc" "TEST_SOURCE_FILE")
  ewoms_add_test(${TEST_NAME}
    SOURCES "tests/${TEST_SOURCE_FILE}"
    CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
    LIBRARIES "${Boost_LIBRARIES}")
endforeach()

ewoms_add_test(RootFinders
  SOURCES tests/test_rootfinders.cc
  CONDITION Boost_UNIT_TEST_FRAMEWORK_FOUND
  LIBRARIES "${Boost_LIBRARIES}")

# deal with the data required by the unit tests
ewoms_recusive_copy_testdata_to_builddir("tests/SPE*" "tests/ECLFILE*" "tests/*.json" "tests/*.SMSPEC" "tests/*.EGRID" "tests/*.txt " "tests/*.out" "tests/*.grdecl" "tests/*.data" "tests/*.DATA" "tests/*.param")

# link the UNRST files for the restart test to the build directory
set(FROMDIR "${PROJECT_BINARY_DIR}/restart")
set(TODIR "${PROJECT_SOURCE_DIR}/tests/restart")
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${TODIR}" "${FROMDIR}")

# finalize the project, e.g. generate the config.h etc.
finalize_ewoms_project()
