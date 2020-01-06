#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/parseritem.hh>
#include <ewoms/eclio/parser/parserrecord.hh>
#include <ewoms/eclio/parser/parser.hh>





#include <ewoms/eclio/parser/parserkeywords/o.hh>
namespace Ewoms {
namespace ParserKeywords {
OCOMPIDX::OCOMPIDX( ) : ParserKeyword("OCOMPIDX")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("RUNSPEC");
  clearDeckNames();
  addDeckName("OCOMPIDX");
  {
     ParserRecord record;
     {
        ParserItem item("OIL_COMPONENT_INDEX", ParserItem::itype::INT);
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OCOMPIDX::keywordName = "OCOMPIDX";
const std::string OCOMPIDX::OIL_COMPONENT_INDEX::itemName = "OIL_COMPONENT_INDEX";


OFM::OFM( ) : ParserKeyword("OFM")
{
  setFixedSize( (size_t) 0);
  addValidSectionName("SUMMARY");
  clearDeckNames();
  addDeckName("OFM");
}
const std::string OFM::keywordName = "OFM";


OIL::OIL( ) : ParserKeyword("OIL")
{
  setFixedSize( (size_t) 0);
  addValidSectionName("RUNSPEC");
  clearDeckNames();
  addDeckName("OIL");
}
const std::string OIL::keywordName = "OIL";


OILAPI::OILAPI( ) : ParserKeyword("OILAPI")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("SOLUTION");
  clearDeckNames();
  addDeckName("OILAPI");
  {
     ParserRecord record;
     {
        ParserItem item("data", ParserItem::itype::DOUBLE);
        item.setSizeType(ParserItem::item_size::ALL);
        record.addDataItem(item);
     }
     addDataRecord( record );
  }
}
const std::string OILAPI::keywordName = "OILAPI";
const std::string OILAPI::data::itemName = "data";


OILCOMPR::OILCOMPR( ) : ParserKeyword("OILCOMPR")
{
  setSizeType(OTHER_KEYWORD_IN_DECK);
  initSizeKeyword("TABDIMS","NTPVT",0);
  addValidSectionName("PROPS");
  clearDeckNames();
  addDeckName("OILCOMPR");
  {
     ParserRecord record;
     {
        ParserItem item("COMPRESSIBILITY", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        record.addItem(item);
     }
     {
        ParserItem item("EXPANSION_COEFF_LINEAR", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        item.push_backDimension("1/AbsoluteTemperature");
        record.addItem(item);
     }
     {
        ParserItem item("EXPANSION_COEFF_QUADRATIC", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        item.push_backDimension("1/AbsoluteTemperature*AbsoluteTemperature");
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OILCOMPR::keywordName = "OILCOMPR";
const std::string OILCOMPR::COMPRESSIBILITY::itemName = "COMPRESSIBILITY";
const double OILCOMPR::COMPRESSIBILITY::defaultValue = 0.000000;
const std::string OILCOMPR::EXPANSION_COEFF_LINEAR::itemName = "EXPANSION_COEFF_LINEAR";
const double OILCOMPR::EXPANSION_COEFF_LINEAR::defaultValue = 0.000000;
const std::string OILCOMPR::EXPANSION_COEFF_QUADRATIC::itemName = "EXPANSION_COEFF_QUADRATIC";
const double OILCOMPR::EXPANSION_COEFF_QUADRATIC::defaultValue = 0.000000;


OILDENT::OILDENT( ) : ParserKeyword("OILDENT")
{
  setSizeType(OTHER_KEYWORD_IN_DECK);
  initSizeKeyword("TABDIMS","NTPVT",0);
  addValidSectionName("PROPS");
  clearDeckNames();
  addDeckName("OILDENT");
  {
     ParserRecord record;
     {
        ParserItem item("REFERENCE_TEMPERATURE", ParserItem::itype::DOUBLE);
        item.setDefault( double(293.150000) );
        item.push_backDimension("AbsoluteTemperature");
        record.addItem(item);
     }
     {
        ParserItem item("EXPANSION_COEFF_LINEAR", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        item.push_backDimension("1/AbsoluteTemperature");
        record.addItem(item);
     }
     {
        ParserItem item("EXPANSION_COEFF_QUADRATIC", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        item.push_backDimension("1/AbsoluteTemperature*AbsoluteTemperature");
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OILDENT::keywordName = "OILDENT";
const std::string OILDENT::REFERENCE_TEMPERATURE::itemName = "REFERENCE_TEMPERATURE";
const double OILDENT::REFERENCE_TEMPERATURE::defaultValue = 293.150000;
const std::string OILDENT::EXPANSION_COEFF_LINEAR::itemName = "EXPANSION_COEFF_LINEAR";
const double OILDENT::EXPANSION_COEFF_LINEAR::defaultValue = 0.000000;
const std::string OILDENT::EXPANSION_COEFF_QUADRATIC::itemName = "EXPANSION_COEFF_QUADRATIC";
const double OILDENT::EXPANSION_COEFF_QUADRATIC::defaultValue = 0.000000;


OILMW::OILMW( ) : ParserKeyword("OILMW")
{
  setSizeType(OTHER_KEYWORD_IN_DECK);
  initSizeKeyword("TABDIMS","NTPVT",0);
  addValidSectionName("PROPS");
  clearDeckNames();
  addDeckName("OILMW");
  {
     ParserRecord record;
     {
        ParserItem item("MOLAR_WEIGHT", ParserItem::itype::DOUBLE);
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OILMW::keywordName = "OILMW";
const std::string OILMW::MOLAR_WEIGHT::itemName = "MOLAR_WEIGHT";


OILVISCT::OILVISCT( ) : ParserKeyword("OILVISCT")
{
  setSizeType(OTHER_KEYWORD_IN_DECK);
  initSizeKeyword("TABDIMS","NTPVT",0);
  addValidSectionName("PROPS");
  clearDeckNames();
  addDeckName("OILVISCT");
  {
     ParserRecord record;
     {
        ParserItem item("DATA", ParserItem::itype::DOUBLE);
        item.setSizeType(ParserItem::item_size::ALL);
        item.push_backDimension("Temperature");
        item.push_backDimension("Viscosity");
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OILVISCT::keywordName = "OILVISCT";
const std::string OILVISCT::DATA::itemName = "DATA";


OILVTIM::OILVTIM( ) : ParserKeyword("OILVTIM")
{
  setSizeType(SLASH_TERMINATED);
  addValidSectionName("PROPS");
  clearDeckNames();
  addDeckName("OILVTIM");
  {
     ParserRecord record;
     {
        ParserItem item("INTERPOLATION_METHOD", ParserItem::itype::STRING);
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OILVTIM::keywordName = "OILVTIM";
const std::string OILVTIM::INTERPOLATION_METHOD::itemName = "INTERPOLATION_METHOD";


OLDTRAN::OLDTRAN( ) : ParserKeyword("OLDTRAN")
{
  setFixedSize( (size_t) 0);
  addValidSectionName("GRID");
  clearDeckNames();
  addDeckName("OLDTRAN");
}
const std::string OLDTRAN::keywordName = "OLDTRAN";


OLDTRANR::OLDTRANR( ) : ParserKeyword("OLDTRANR")
{
  setFixedSize( (size_t) 0);
  addValidSectionName("GRID");
  clearDeckNames();
  addDeckName("OLDTRANR");
}
const std::string OLDTRANR::keywordName = "OLDTRANR";


OPERATE::OPERATE( ) : ParserKeyword("OPERATE")
{
  setSizeType(SLASH_TERMINATED);
  addValidSectionName("EDIT");
  addValidSectionName("GRID");
  addValidSectionName("PROPS");
  addValidSectionName("REGIONS");
  addValidSectionName("SOLUTION");
  clearDeckNames();
  addDeckName("OPERATE");
  {
     ParserRecord record;
     {
        ParserItem item("TARGET_ARRAY", ParserItem::itype::STRING);
        record.addItem(item);
     }
     {
        ParserItem item("I1", ParserItem::itype::INT);
        record.addItem(item);
     }
     {
        ParserItem item("I2", ParserItem::itype::INT);
        record.addItem(item);
     }
     {
        ParserItem item("J1", ParserItem::itype::INT);
        record.addItem(item);
     }
     {
        ParserItem item("J2", ParserItem::itype::INT);
        record.addItem(item);
     }
     {
        ParserItem item("K1", ParserItem::itype::INT);
        record.addItem(item);
     }
     {
        ParserItem item("K2", ParserItem::itype::INT);
        record.addItem(item);
     }
     {
        ParserItem item("OPERATION", ParserItem::itype::STRING);
        record.addItem(item);
     }
     {
        ParserItem item("ARRAY", ParserItem::itype::STRING);
        record.addItem(item);
     }
     {
        ParserItem item("PARAM1", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        record.addItem(item);
     }
     {
        ParserItem item("PARAM2", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OPERATE::keywordName = "OPERATE";
const std::string OPERATE::TARGET_ARRAY::itemName = "TARGET_ARRAY";
const std::string OPERATE::I1::itemName = "I1";
const std::string OPERATE::I2::itemName = "I2";
const std::string OPERATE::J1::itemName = "J1";
const std::string OPERATE::J2::itemName = "J2";
const std::string OPERATE::K1::itemName = "K1";
const std::string OPERATE::K2::itemName = "K2";
const std::string OPERATE::OPERATION::itemName = "OPERATION";
const std::string OPERATE::ARRAY::itemName = "ARRAY";
const std::string OPERATE::PARAM1::itemName = "PARAM1";
const double OPERATE::PARAM1::defaultValue = 0.000000;
const std::string OPERATE::PARAM2::itemName = "PARAM2";
const double OPERATE::PARAM2::defaultValue = 0.000000;


OPERATER::OPERATER( ) : ParserKeyword("OPERATER")
{
  setSizeType(SLASH_TERMINATED);
  addValidSectionName("EDIT");
  addValidSectionName("GRID");
  addValidSectionName("PROPS");
  addValidSectionName("REGIONS");
  addValidSectionName("SOLUTION");
  clearDeckNames();
  addDeckName("OPERATER");
  {
     ParserRecord record;
     {
        ParserItem item("RESULT_ARRAY", ParserItem::itype::STRING);
        record.addItem(item);
     }
     {
        ParserItem item("REGION_NUMBER", ParserItem::itype::INT);
        record.addItem(item);
     }
     {
        ParserItem item("OPERATION", ParserItem::itype::STRING);
        record.addItem(item);
     }
     {
        ParserItem item("ARRAY_PARAMETER", ParserItem::itype::STRING);
        record.addItem(item);
     }
     {
        ParserItem item("PARAM1", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        record.addItem(item);
     }
     {
        ParserItem item("PARAM2", ParserItem::itype::DOUBLE);
        item.setDefault( double(0) );
        record.addItem(item);
     }
     {
        ParserItem item("REGION_NAME", ParserItem::itype::STRING);
        item.setDefault( std::string("OPERNUM") );
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OPERATER::keywordName = "OPERATER";
const std::string OPERATER::RESULT_ARRAY::itemName = "RESULT_ARRAY";
const std::string OPERATER::REGION_NUMBER::itemName = "REGION_NUMBER";
const std::string OPERATER::OPERATION::itemName = "OPERATION";
const std::string OPERATER::ARRAY_PARAMETER::itemName = "ARRAY_PARAMETER";
const std::string OPERATER::PARAM1::itemName = "PARAM1";
const double OPERATER::PARAM1::defaultValue = 0.000000;
const std::string OPERATER::PARAM2::itemName = "PARAM2";
const double OPERATER::PARAM2::defaultValue = 0.000000;
const std::string OPERATER::REGION_NAME::itemName = "REGION_NAME";
const std::string OPERATER::REGION_NAME::defaultValue = "OPERNUM";


OPERNUM::OPERNUM( ) : ParserKeyword("OPERNUM")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("GRID");
  addValidSectionName("REGIONS");
  clearDeckNames();
  addDeckName("OPERNUM");
  {
     ParserRecord record;
     {
        ParserItem item("data", ParserItem::itype::INT);
        item.setSizeType(ParserItem::item_size::ALL);
        record.addDataItem(item);
     }
     addDataRecord( record );
  }
}
const std::string OPERNUM::keywordName = "OPERNUM";
const std::string OPERNUM::data::itemName = "data";


OPTIONS::OPTIONS( ) : ParserKeyword("OPTIONS")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("RUNSPEC");
  addValidSectionName("SCHEDULE");
  clearDeckNames();
  addDeckName("OPTIONS");
  {
     ParserRecord record;
     {
        ParserItem item("ITEM1", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM2", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM3", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM4", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM5", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM6", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM7", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM8", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM9", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM10", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM11", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM12", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM13", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM14", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM15", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM16", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM17", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM18", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM19", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM20", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM21", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM22", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM23", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM24", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM25", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM26", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM27", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM28", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM29", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM30", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM31", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM32", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM33", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM34", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM35", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM36", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM37", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM38", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM39", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM40", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM41", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM42", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM43", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM44", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM45", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM46", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM47", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM48", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM49", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM50", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM51", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM52", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM53", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM54", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM55", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM56", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM57", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM58", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM59", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM60", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM61", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM62", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM63", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM64", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM65", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM66", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM67", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM68", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM69", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM70", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM71", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM72", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM73", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM74", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM75", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM76", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM77", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM78", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM79", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM80", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM81", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM82", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM83", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM84", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM85", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM86", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM87", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM88", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM89", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM90", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM91", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM92", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM93", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM94", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM95", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM96", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM97", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM98", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM99", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM100", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM101", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM102", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM103", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM104", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM105", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM106", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM107", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM108", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM109", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM110", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM111", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM112", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM113", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM114", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM115", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM116", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM117", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM118", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM119", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM120", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM121", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM122", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM123", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM124", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM125", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM126", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM127", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM128", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM129", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM130", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM131", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM132", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM133", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM134", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM135", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM136", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM137", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM138", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM139", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM140", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM141", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM142", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM143", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM144", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM145", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM146", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM147", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM148", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM149", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM150", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM151", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM152", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM153", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM154", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM155", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM156", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM157", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM158", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM159", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM160", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM161", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM162", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM163", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM164", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM165", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM166", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM167", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM168", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM169", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM170", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM171", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM172", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM173", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM174", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM175", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM176", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM177", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM178", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM179", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM180", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM181", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM182", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM183", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM184", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM185", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM186", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM187", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM188", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM189", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM190", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM191", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM192", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM193", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM194", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM195", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM196", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     {
        ParserItem item("ITEM197", ParserItem::itype::INT);
        item.setDefault( 0 );
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OPTIONS::keywordName = "OPTIONS";
const std::string OPTIONS::ITEM1::itemName = "ITEM1";
const int OPTIONS::ITEM1::defaultValue = 0;
const std::string OPTIONS::ITEM2::itemName = "ITEM2";
const int OPTIONS::ITEM2::defaultValue = 0;
const std::string OPTIONS::ITEM3::itemName = "ITEM3";
const int OPTIONS::ITEM3::defaultValue = 0;
const std::string OPTIONS::ITEM4::itemName = "ITEM4";
const int OPTIONS::ITEM4::defaultValue = 0;
const std::string OPTIONS::ITEM5::itemName = "ITEM5";
const int OPTIONS::ITEM5::defaultValue = 0;
const std::string OPTIONS::ITEM6::itemName = "ITEM6";
const int OPTIONS::ITEM6::defaultValue = 0;
const std::string OPTIONS::ITEM7::itemName = "ITEM7";
const int OPTIONS::ITEM7::defaultValue = 0;
const std::string OPTIONS::ITEM8::itemName = "ITEM8";
const int OPTIONS::ITEM8::defaultValue = 0;
const std::string OPTIONS::ITEM9::itemName = "ITEM9";
const int OPTIONS::ITEM9::defaultValue = 0;
const std::string OPTIONS::ITEM10::itemName = "ITEM10";
const int OPTIONS::ITEM10::defaultValue = 0;
const std::string OPTIONS::ITEM11::itemName = "ITEM11";
const int OPTIONS::ITEM11::defaultValue = 0;
const std::string OPTIONS::ITEM12::itemName = "ITEM12";
const int OPTIONS::ITEM12::defaultValue = 0;
const std::string OPTIONS::ITEM13::itemName = "ITEM13";
const int OPTIONS::ITEM13::defaultValue = 0;
const std::string OPTIONS::ITEM14::itemName = "ITEM14";
const int OPTIONS::ITEM14::defaultValue = 0;
const std::string OPTIONS::ITEM15::itemName = "ITEM15";
const int OPTIONS::ITEM15::defaultValue = 0;
const std::string OPTIONS::ITEM16::itemName = "ITEM16";
const int OPTIONS::ITEM16::defaultValue = 0;
const std::string OPTIONS::ITEM17::itemName = "ITEM17";
const int OPTIONS::ITEM17::defaultValue = 0;
const std::string OPTIONS::ITEM18::itemName = "ITEM18";
const int OPTIONS::ITEM18::defaultValue = 0;
const std::string OPTIONS::ITEM19::itemName = "ITEM19";
const int OPTIONS::ITEM19::defaultValue = 0;
const std::string OPTIONS::ITEM20::itemName = "ITEM20";
const int OPTIONS::ITEM20::defaultValue = 0;
const std::string OPTIONS::ITEM21::itemName = "ITEM21";
const int OPTIONS::ITEM21::defaultValue = 0;
const std::string OPTIONS::ITEM22::itemName = "ITEM22";
const int OPTIONS::ITEM22::defaultValue = 0;
const std::string OPTIONS::ITEM23::itemName = "ITEM23";
const int OPTIONS::ITEM23::defaultValue = 0;
const std::string OPTIONS::ITEM24::itemName = "ITEM24";
const int OPTIONS::ITEM24::defaultValue = 0;
const std::string OPTIONS::ITEM25::itemName = "ITEM25";
const int OPTIONS::ITEM25::defaultValue = 0;
const std::string OPTIONS::ITEM26::itemName = "ITEM26";
const int OPTIONS::ITEM26::defaultValue = 0;
const std::string OPTIONS::ITEM27::itemName = "ITEM27";
const int OPTIONS::ITEM27::defaultValue = 0;
const std::string OPTIONS::ITEM28::itemName = "ITEM28";
const int OPTIONS::ITEM28::defaultValue = 0;
const std::string OPTIONS::ITEM29::itemName = "ITEM29";
const int OPTIONS::ITEM29::defaultValue = 0;
const std::string OPTIONS::ITEM30::itemName = "ITEM30";
const int OPTIONS::ITEM30::defaultValue = 0;
const std::string OPTIONS::ITEM31::itemName = "ITEM31";
const int OPTIONS::ITEM31::defaultValue = 0;
const std::string OPTIONS::ITEM32::itemName = "ITEM32";
const int OPTIONS::ITEM32::defaultValue = 0;
const std::string OPTIONS::ITEM33::itemName = "ITEM33";
const int OPTIONS::ITEM33::defaultValue = 0;
const std::string OPTIONS::ITEM34::itemName = "ITEM34";
const int OPTIONS::ITEM34::defaultValue = 0;
const std::string OPTIONS::ITEM35::itemName = "ITEM35";
const int OPTIONS::ITEM35::defaultValue = 0;
const std::string OPTIONS::ITEM36::itemName = "ITEM36";
const int OPTIONS::ITEM36::defaultValue = 0;
const std::string OPTIONS::ITEM37::itemName = "ITEM37";
const int OPTIONS::ITEM37::defaultValue = 0;
const std::string OPTIONS::ITEM38::itemName = "ITEM38";
const int OPTIONS::ITEM38::defaultValue = 0;
const std::string OPTIONS::ITEM39::itemName = "ITEM39";
const int OPTIONS::ITEM39::defaultValue = 0;
const std::string OPTIONS::ITEM40::itemName = "ITEM40";
const int OPTIONS::ITEM40::defaultValue = 0;
const std::string OPTIONS::ITEM41::itemName = "ITEM41";
const int OPTIONS::ITEM41::defaultValue = 0;
const std::string OPTIONS::ITEM42::itemName = "ITEM42";
const int OPTIONS::ITEM42::defaultValue = 0;
const std::string OPTIONS::ITEM43::itemName = "ITEM43";
const int OPTIONS::ITEM43::defaultValue = 0;
const std::string OPTIONS::ITEM44::itemName = "ITEM44";
const int OPTIONS::ITEM44::defaultValue = 0;
const std::string OPTIONS::ITEM45::itemName = "ITEM45";
const int OPTIONS::ITEM45::defaultValue = 0;
const std::string OPTIONS::ITEM46::itemName = "ITEM46";
const int OPTIONS::ITEM46::defaultValue = 0;
const std::string OPTIONS::ITEM47::itemName = "ITEM47";
const int OPTIONS::ITEM47::defaultValue = 0;
const std::string OPTIONS::ITEM48::itemName = "ITEM48";
const int OPTIONS::ITEM48::defaultValue = 0;
const std::string OPTIONS::ITEM49::itemName = "ITEM49";
const int OPTIONS::ITEM49::defaultValue = 0;
const std::string OPTIONS::ITEM50::itemName = "ITEM50";
const int OPTIONS::ITEM50::defaultValue = 0;
const std::string OPTIONS::ITEM51::itemName = "ITEM51";
const int OPTIONS::ITEM51::defaultValue = 0;
const std::string OPTIONS::ITEM52::itemName = "ITEM52";
const int OPTIONS::ITEM52::defaultValue = 0;
const std::string OPTIONS::ITEM53::itemName = "ITEM53";
const int OPTIONS::ITEM53::defaultValue = 0;
const std::string OPTIONS::ITEM54::itemName = "ITEM54";
const int OPTIONS::ITEM54::defaultValue = 0;
const std::string OPTIONS::ITEM55::itemName = "ITEM55";
const int OPTIONS::ITEM55::defaultValue = 0;
const std::string OPTIONS::ITEM56::itemName = "ITEM56";
const int OPTIONS::ITEM56::defaultValue = 0;
const std::string OPTIONS::ITEM57::itemName = "ITEM57";
const int OPTIONS::ITEM57::defaultValue = 0;
const std::string OPTIONS::ITEM58::itemName = "ITEM58";
const int OPTIONS::ITEM58::defaultValue = 0;
const std::string OPTIONS::ITEM59::itemName = "ITEM59";
const int OPTIONS::ITEM59::defaultValue = 0;
const std::string OPTIONS::ITEM60::itemName = "ITEM60";
const int OPTIONS::ITEM60::defaultValue = 0;
const std::string OPTIONS::ITEM61::itemName = "ITEM61";
const int OPTIONS::ITEM61::defaultValue = 0;
const std::string OPTIONS::ITEM62::itemName = "ITEM62";
const int OPTIONS::ITEM62::defaultValue = 0;
const std::string OPTIONS::ITEM63::itemName = "ITEM63";
const int OPTIONS::ITEM63::defaultValue = 0;
const std::string OPTIONS::ITEM64::itemName = "ITEM64";
const int OPTIONS::ITEM64::defaultValue = 0;
const std::string OPTIONS::ITEM65::itemName = "ITEM65";
const int OPTIONS::ITEM65::defaultValue = 0;
const std::string OPTIONS::ITEM66::itemName = "ITEM66";
const int OPTIONS::ITEM66::defaultValue = 0;
const std::string OPTIONS::ITEM67::itemName = "ITEM67";
const int OPTIONS::ITEM67::defaultValue = 0;
const std::string OPTIONS::ITEM68::itemName = "ITEM68";
const int OPTIONS::ITEM68::defaultValue = 0;
const std::string OPTIONS::ITEM69::itemName = "ITEM69";
const int OPTIONS::ITEM69::defaultValue = 0;
const std::string OPTIONS::ITEM70::itemName = "ITEM70";
const int OPTIONS::ITEM70::defaultValue = 0;
const std::string OPTIONS::ITEM71::itemName = "ITEM71";
const int OPTIONS::ITEM71::defaultValue = 0;
const std::string OPTIONS::ITEM72::itemName = "ITEM72";
const int OPTIONS::ITEM72::defaultValue = 0;
const std::string OPTIONS::ITEM73::itemName = "ITEM73";
const int OPTIONS::ITEM73::defaultValue = 0;
const std::string OPTIONS::ITEM74::itemName = "ITEM74";
const int OPTIONS::ITEM74::defaultValue = 0;
const std::string OPTIONS::ITEM75::itemName = "ITEM75";
const int OPTIONS::ITEM75::defaultValue = 0;
const std::string OPTIONS::ITEM76::itemName = "ITEM76";
const int OPTIONS::ITEM76::defaultValue = 0;
const std::string OPTIONS::ITEM77::itemName = "ITEM77";
const int OPTIONS::ITEM77::defaultValue = 0;
const std::string OPTIONS::ITEM78::itemName = "ITEM78";
const int OPTIONS::ITEM78::defaultValue = 0;
const std::string OPTIONS::ITEM79::itemName = "ITEM79";
const int OPTIONS::ITEM79::defaultValue = 0;
const std::string OPTIONS::ITEM80::itemName = "ITEM80";
const int OPTIONS::ITEM80::defaultValue = 0;
const std::string OPTIONS::ITEM81::itemName = "ITEM81";
const int OPTIONS::ITEM81::defaultValue = 0;
const std::string OPTIONS::ITEM82::itemName = "ITEM82";
const int OPTIONS::ITEM82::defaultValue = 0;
const std::string OPTIONS::ITEM83::itemName = "ITEM83";
const int OPTIONS::ITEM83::defaultValue = 0;
const std::string OPTIONS::ITEM84::itemName = "ITEM84";
const int OPTIONS::ITEM84::defaultValue = 0;
const std::string OPTIONS::ITEM85::itemName = "ITEM85";
const int OPTIONS::ITEM85::defaultValue = 0;
const std::string OPTIONS::ITEM86::itemName = "ITEM86";
const int OPTIONS::ITEM86::defaultValue = 0;
const std::string OPTIONS::ITEM87::itemName = "ITEM87";
const int OPTIONS::ITEM87::defaultValue = 0;
const std::string OPTIONS::ITEM88::itemName = "ITEM88";
const int OPTIONS::ITEM88::defaultValue = 0;
const std::string OPTIONS::ITEM89::itemName = "ITEM89";
const int OPTIONS::ITEM89::defaultValue = 0;
const std::string OPTIONS::ITEM90::itemName = "ITEM90";
const int OPTIONS::ITEM90::defaultValue = 0;
const std::string OPTIONS::ITEM91::itemName = "ITEM91";
const int OPTIONS::ITEM91::defaultValue = 0;
const std::string OPTIONS::ITEM92::itemName = "ITEM92";
const int OPTIONS::ITEM92::defaultValue = 0;
const std::string OPTIONS::ITEM93::itemName = "ITEM93";
const int OPTIONS::ITEM93::defaultValue = 0;
const std::string OPTIONS::ITEM94::itemName = "ITEM94";
const int OPTIONS::ITEM94::defaultValue = 0;
const std::string OPTIONS::ITEM95::itemName = "ITEM95";
const int OPTIONS::ITEM95::defaultValue = 0;
const std::string OPTIONS::ITEM96::itemName = "ITEM96";
const int OPTIONS::ITEM96::defaultValue = 0;
const std::string OPTIONS::ITEM97::itemName = "ITEM97";
const int OPTIONS::ITEM97::defaultValue = 0;
const std::string OPTIONS::ITEM98::itemName = "ITEM98";
const int OPTIONS::ITEM98::defaultValue = 0;
const std::string OPTIONS::ITEM99::itemName = "ITEM99";
const int OPTIONS::ITEM99::defaultValue = 0;
const std::string OPTIONS::ITEM100::itemName = "ITEM100";
const int OPTIONS::ITEM100::defaultValue = 0;
const std::string OPTIONS::ITEM101::itemName = "ITEM101";
const int OPTIONS::ITEM101::defaultValue = 0;
const std::string OPTIONS::ITEM102::itemName = "ITEM102";
const int OPTIONS::ITEM102::defaultValue = 0;
const std::string OPTIONS::ITEM103::itemName = "ITEM103";
const int OPTIONS::ITEM103::defaultValue = 0;
const std::string OPTIONS::ITEM104::itemName = "ITEM104";
const int OPTIONS::ITEM104::defaultValue = 0;
const std::string OPTIONS::ITEM105::itemName = "ITEM105";
const int OPTIONS::ITEM105::defaultValue = 0;
const std::string OPTIONS::ITEM106::itemName = "ITEM106";
const int OPTIONS::ITEM106::defaultValue = 0;
const std::string OPTIONS::ITEM107::itemName = "ITEM107";
const int OPTIONS::ITEM107::defaultValue = 0;
const std::string OPTIONS::ITEM108::itemName = "ITEM108";
const int OPTIONS::ITEM108::defaultValue = 0;
const std::string OPTIONS::ITEM109::itemName = "ITEM109";
const int OPTIONS::ITEM109::defaultValue = 0;
const std::string OPTIONS::ITEM110::itemName = "ITEM110";
const int OPTIONS::ITEM110::defaultValue = 0;
const std::string OPTIONS::ITEM111::itemName = "ITEM111";
const int OPTIONS::ITEM111::defaultValue = 0;
const std::string OPTIONS::ITEM112::itemName = "ITEM112";
const int OPTIONS::ITEM112::defaultValue = 0;
const std::string OPTIONS::ITEM113::itemName = "ITEM113";
const int OPTIONS::ITEM113::defaultValue = 0;
const std::string OPTIONS::ITEM114::itemName = "ITEM114";
const int OPTIONS::ITEM114::defaultValue = 0;
const std::string OPTIONS::ITEM115::itemName = "ITEM115";
const int OPTIONS::ITEM115::defaultValue = 0;
const std::string OPTIONS::ITEM116::itemName = "ITEM116";
const int OPTIONS::ITEM116::defaultValue = 0;
const std::string OPTIONS::ITEM117::itemName = "ITEM117";
const int OPTIONS::ITEM117::defaultValue = 0;
const std::string OPTIONS::ITEM118::itemName = "ITEM118";
const int OPTIONS::ITEM118::defaultValue = 0;
const std::string OPTIONS::ITEM119::itemName = "ITEM119";
const int OPTIONS::ITEM119::defaultValue = 0;
const std::string OPTIONS::ITEM120::itemName = "ITEM120";
const int OPTIONS::ITEM120::defaultValue = 0;
const std::string OPTIONS::ITEM121::itemName = "ITEM121";
const int OPTIONS::ITEM121::defaultValue = 0;
const std::string OPTIONS::ITEM122::itemName = "ITEM122";
const int OPTIONS::ITEM122::defaultValue = 0;
const std::string OPTIONS::ITEM123::itemName = "ITEM123";
const int OPTIONS::ITEM123::defaultValue = 0;
const std::string OPTIONS::ITEM124::itemName = "ITEM124";
const int OPTIONS::ITEM124::defaultValue = 0;
const std::string OPTIONS::ITEM125::itemName = "ITEM125";
const int OPTIONS::ITEM125::defaultValue = 0;
const std::string OPTIONS::ITEM126::itemName = "ITEM126";
const int OPTIONS::ITEM126::defaultValue = 0;
const std::string OPTIONS::ITEM127::itemName = "ITEM127";
const int OPTIONS::ITEM127::defaultValue = 0;
const std::string OPTIONS::ITEM128::itemName = "ITEM128";
const int OPTIONS::ITEM128::defaultValue = 0;
const std::string OPTIONS::ITEM129::itemName = "ITEM129";
const int OPTIONS::ITEM129::defaultValue = 0;
const std::string OPTIONS::ITEM130::itemName = "ITEM130";
const int OPTIONS::ITEM130::defaultValue = 0;
const std::string OPTIONS::ITEM131::itemName = "ITEM131";
const int OPTIONS::ITEM131::defaultValue = 0;
const std::string OPTIONS::ITEM132::itemName = "ITEM132";
const int OPTIONS::ITEM132::defaultValue = 0;
const std::string OPTIONS::ITEM133::itemName = "ITEM133";
const int OPTIONS::ITEM133::defaultValue = 0;
const std::string OPTIONS::ITEM134::itemName = "ITEM134";
const int OPTIONS::ITEM134::defaultValue = 0;
const std::string OPTIONS::ITEM135::itemName = "ITEM135";
const int OPTIONS::ITEM135::defaultValue = 0;
const std::string OPTIONS::ITEM136::itemName = "ITEM136";
const int OPTIONS::ITEM136::defaultValue = 0;
const std::string OPTIONS::ITEM137::itemName = "ITEM137";
const int OPTIONS::ITEM137::defaultValue = 0;
const std::string OPTIONS::ITEM138::itemName = "ITEM138";
const int OPTIONS::ITEM138::defaultValue = 0;
const std::string OPTIONS::ITEM139::itemName = "ITEM139";
const int OPTIONS::ITEM139::defaultValue = 0;
const std::string OPTIONS::ITEM140::itemName = "ITEM140";
const int OPTIONS::ITEM140::defaultValue = 0;
const std::string OPTIONS::ITEM141::itemName = "ITEM141";
const int OPTIONS::ITEM141::defaultValue = 0;
const std::string OPTIONS::ITEM142::itemName = "ITEM142";
const int OPTIONS::ITEM142::defaultValue = 0;
const std::string OPTIONS::ITEM143::itemName = "ITEM143";
const int OPTIONS::ITEM143::defaultValue = 0;
const std::string OPTIONS::ITEM144::itemName = "ITEM144";
const int OPTIONS::ITEM144::defaultValue = 0;
const std::string OPTIONS::ITEM145::itemName = "ITEM145";
const int OPTIONS::ITEM145::defaultValue = 0;
const std::string OPTIONS::ITEM146::itemName = "ITEM146";
const int OPTIONS::ITEM146::defaultValue = 0;
const std::string OPTIONS::ITEM147::itemName = "ITEM147";
const int OPTIONS::ITEM147::defaultValue = 0;
const std::string OPTIONS::ITEM148::itemName = "ITEM148";
const int OPTIONS::ITEM148::defaultValue = 0;
const std::string OPTIONS::ITEM149::itemName = "ITEM149";
const int OPTIONS::ITEM149::defaultValue = 0;
const std::string OPTIONS::ITEM150::itemName = "ITEM150";
const int OPTIONS::ITEM150::defaultValue = 0;
const std::string OPTIONS::ITEM151::itemName = "ITEM151";
const int OPTIONS::ITEM151::defaultValue = 0;
const std::string OPTIONS::ITEM152::itemName = "ITEM152";
const int OPTIONS::ITEM152::defaultValue = 0;
const std::string OPTIONS::ITEM153::itemName = "ITEM153";
const int OPTIONS::ITEM153::defaultValue = 0;
const std::string OPTIONS::ITEM154::itemName = "ITEM154";
const int OPTIONS::ITEM154::defaultValue = 0;
const std::string OPTIONS::ITEM155::itemName = "ITEM155";
const int OPTIONS::ITEM155::defaultValue = 0;
const std::string OPTIONS::ITEM156::itemName = "ITEM156";
const int OPTIONS::ITEM156::defaultValue = 0;
const std::string OPTIONS::ITEM157::itemName = "ITEM157";
const int OPTIONS::ITEM157::defaultValue = 0;
const std::string OPTIONS::ITEM158::itemName = "ITEM158";
const int OPTIONS::ITEM158::defaultValue = 0;
const std::string OPTIONS::ITEM159::itemName = "ITEM159";
const int OPTIONS::ITEM159::defaultValue = 0;
const std::string OPTIONS::ITEM160::itemName = "ITEM160";
const int OPTIONS::ITEM160::defaultValue = 0;
const std::string OPTIONS::ITEM161::itemName = "ITEM161";
const int OPTIONS::ITEM161::defaultValue = 0;
const std::string OPTIONS::ITEM162::itemName = "ITEM162";
const int OPTIONS::ITEM162::defaultValue = 0;
const std::string OPTIONS::ITEM163::itemName = "ITEM163";
const int OPTIONS::ITEM163::defaultValue = 0;
const std::string OPTIONS::ITEM164::itemName = "ITEM164";
const int OPTIONS::ITEM164::defaultValue = 0;
const std::string OPTIONS::ITEM165::itemName = "ITEM165";
const int OPTIONS::ITEM165::defaultValue = 0;
const std::string OPTIONS::ITEM166::itemName = "ITEM166";
const int OPTIONS::ITEM166::defaultValue = 0;
const std::string OPTIONS::ITEM167::itemName = "ITEM167";
const int OPTIONS::ITEM167::defaultValue = 0;
const std::string OPTIONS::ITEM168::itemName = "ITEM168";
const int OPTIONS::ITEM168::defaultValue = 0;
const std::string OPTIONS::ITEM169::itemName = "ITEM169";
const int OPTIONS::ITEM169::defaultValue = 0;
const std::string OPTIONS::ITEM170::itemName = "ITEM170";
const int OPTIONS::ITEM170::defaultValue = 0;
const std::string OPTIONS::ITEM171::itemName = "ITEM171";
const int OPTIONS::ITEM171::defaultValue = 0;
const std::string OPTIONS::ITEM172::itemName = "ITEM172";
const int OPTIONS::ITEM172::defaultValue = 0;
const std::string OPTIONS::ITEM173::itemName = "ITEM173";
const int OPTIONS::ITEM173::defaultValue = 0;
const std::string OPTIONS::ITEM174::itemName = "ITEM174";
const int OPTIONS::ITEM174::defaultValue = 0;
const std::string OPTIONS::ITEM175::itemName = "ITEM175";
const int OPTIONS::ITEM175::defaultValue = 0;
const std::string OPTIONS::ITEM176::itemName = "ITEM176";
const int OPTIONS::ITEM176::defaultValue = 0;
const std::string OPTIONS::ITEM177::itemName = "ITEM177";
const int OPTIONS::ITEM177::defaultValue = 0;
const std::string OPTIONS::ITEM178::itemName = "ITEM178";
const int OPTIONS::ITEM178::defaultValue = 0;
const std::string OPTIONS::ITEM179::itemName = "ITEM179";
const int OPTIONS::ITEM179::defaultValue = 0;
const std::string OPTIONS::ITEM180::itemName = "ITEM180";
const int OPTIONS::ITEM180::defaultValue = 0;
const std::string OPTIONS::ITEM181::itemName = "ITEM181";
const int OPTIONS::ITEM181::defaultValue = 0;
const std::string OPTIONS::ITEM182::itemName = "ITEM182";
const int OPTIONS::ITEM182::defaultValue = 0;
const std::string OPTIONS::ITEM183::itemName = "ITEM183";
const int OPTIONS::ITEM183::defaultValue = 0;
const std::string OPTIONS::ITEM184::itemName = "ITEM184";
const int OPTIONS::ITEM184::defaultValue = 0;
const std::string OPTIONS::ITEM185::itemName = "ITEM185";
const int OPTIONS::ITEM185::defaultValue = 0;
const std::string OPTIONS::ITEM186::itemName = "ITEM186";
const int OPTIONS::ITEM186::defaultValue = 0;
const std::string OPTIONS::ITEM187::itemName = "ITEM187";
const int OPTIONS::ITEM187::defaultValue = 0;
const std::string OPTIONS::ITEM188::itemName = "ITEM188";
const int OPTIONS::ITEM188::defaultValue = 0;
const std::string OPTIONS::ITEM189::itemName = "ITEM189";
const int OPTIONS::ITEM189::defaultValue = 0;
const std::string OPTIONS::ITEM190::itemName = "ITEM190";
const int OPTIONS::ITEM190::defaultValue = 0;
const std::string OPTIONS::ITEM191::itemName = "ITEM191";
const int OPTIONS::ITEM191::defaultValue = 0;
const std::string OPTIONS::ITEM192::itemName = "ITEM192";
const int OPTIONS::ITEM192::defaultValue = 0;
const std::string OPTIONS::ITEM193::itemName = "ITEM193";
const int OPTIONS::ITEM193::defaultValue = 0;
const std::string OPTIONS::ITEM194::itemName = "ITEM194";
const int OPTIONS::ITEM194::defaultValue = 0;
const std::string OPTIONS::ITEM195::itemName = "ITEM195";
const int OPTIONS::ITEM195::defaultValue = 0;
const std::string OPTIONS::ITEM196::itemName = "ITEM196";
const int OPTIONS::ITEM196::defaultValue = 0;
const std::string OPTIONS::ITEM197::itemName = "ITEM197";
const int OPTIONS::ITEM197::defaultValue = 0;


OUTRAD::OUTRAD( ) : ParserKeyword("OUTRAD")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("GRID");
  clearDeckNames();
  addDeckName("OUTRAD");
  {
     ParserRecord record;
     {
        ParserItem item("RADIUS", ParserItem::itype::DOUBLE);
        item.push_backDimension("Length");
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OUTRAD::keywordName = "OUTRAD";
const std::string OUTRAD::RADIUS::itemName = "RADIUS";


OVERBURD::OVERBURD( ) : ParserKeyword("OVERBURD")
{
  setSizeType(OTHER_KEYWORD_IN_DECK);
  initSizeKeyword("ROCKCOMP","NTROCC",0);
  addValidSectionName("PROPS");
  clearDeckNames();
  addDeckName("OVERBURD");
  {
     ParserRecord record;
     {
        ParserItem item("DATA", ParserItem::itype::DOUBLE);
        item.setSizeType(ParserItem::item_size::ALL);
        item.push_backDimension("Length");
        item.push_backDimension("Pressure");
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string OVERBURD::keywordName = "OVERBURD";
const std::string OVERBURD::DATA::itemName = "DATA";


}
}
