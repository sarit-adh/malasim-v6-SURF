#include "Reporter.h"
// #include "ConsoleReporter.h"
#include "ConsoleReporter.h"
#include "MMCReporter.h"
#include "Simulation/Model.h"
#include "MonthlyReporter.h"
#include "NovelDrugReporter.h"
#include "SQLiteMonthlyReporter.h"
#include "SQLiteValidationReporter.h"
#include "TACTReporter.h"
#include "ValidationReporter.h"
#include "Configuration/Config.h"
#include "Specialist/AgeBandReporter.h"
#include "Specialist/CellularReporter.h"
#include "Specialist/PopulationReporter.h"
#include "Specialist/SeasonalImmunity.h"
#include "SMCReporter.h"

std::map<std::string, Reporter::ReportType> Reporter::ReportTypeMap{
    {"Console",         CONSOLE},
    {"MonthlyReporter", MONTHLY_REPORTER},
    {"MMC",             MMC_REPORTER},
    {"TACT",            TACT_REPORTER},
    {"NovelDrug",       NOVEL_DRUG_REPOTER},
    {"ValidationReporter",       VALIDATION_REPORTER},
    {"PopulationReporter", POPULATION_REPORTER},
    {"CellularReporter", CELLULAR_REPORTER},
    {"SeasonalImmunity", SEASONAL_IMMUNITY},
    {"AgeBand", AGE_BAND_REPORTER},
    {"SMCReporter", SMC_REPORTER}, // SMC Reporter
    {"SQLiteMonthlyReporter", SQLITE_MONTHLY_REPORTER},
    {"SQLiteValidationReporter", SQLITE_VALIDATION_REPORTER},
#ifdef ENABLE_TRAVEL_TACKING
        {"TravelTrackingReporter", TRAVEL_TRACKING_REPORTER},
#endif
};

Reporter::Reporter() : model_(nullptr) {
}

Reporter::~Reporter() = default;

std::unique_ptr<Reporter> Reporter::MakeReport(ReportType report_type) {
  switch (report_type) {
  case CONSOLE:
    return std::make_unique<ConsoleReporter>();
  case MONTHLY_REPORTER:
    return std::make_unique<MonthlyReporter>();
  case MMC_REPORTER:
    return std::make_unique<MMCReporter>();
  case TACT_REPORTER:
    return std::make_unique<TACTReporter>();
  case NOVEL_DRUG_REPOTER:
    return std::make_unique<NovelDrugReporter>();
  case VALIDATION_REPORTER:
    return std::make_unique<ValidationReporter>();
  case POPULATION_REPORTER:
    return std::make_unique<PopulationReporter>();
  case CELLULAR_REPORTER:
    return std::make_unique<CellularReporter>();
  case SEASONAL_IMMUNITY:
    return std::make_unique<SeasonalImmunity>();
  case AGE_BAND_REPORTER:
    return std::make_unique<AgeBandReporter>();
  case SMC_REPORTER:
    return std::make_unique<SMCReporter>(); // SMC Reporter
  case SQLITE_MONTHLY_REPORTER: {
    auto cell_level_reporting = Model::get_config()->get_model_settings().get_cell_level_reporting();
    return std::make_unique<SQLiteMonthlyReporter>(cell_level_reporting);
  }
  case SQLITE_VALIDATION_REPORTER: {
    auto cell_level_reporting = Model::get_config()->get_model_settings().get_cell_level_reporting();
    return std::make_unique<SQLiteValidationReporter>();
  }
#ifdef ENABLE_TRAVEL_TRACKING
    case TRAVEL_TRACKING_REPORTER:
      return std::make_unique<TravelTrackingReporter>();
#endif
  default:
    return std::make_unique<MonthlyReporter>();
  }
}