#include "NovelDrugIntroductionStrategy.h"

#include <iostream>

#include "Core/Scheduler/Scheduler.h"
#include "MDC/ModelDataCollector.h"
#include "MFTStrategy.h"
#include "SFTStrategy.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"

NovelDrugIntroductionStrategy::NovelDrugIntroductionStrategy() {
  name = "NovelDrugIntroductionStrategy";
  type = StrategyType::NovelDrugIntroduction;
}

std::string NovelDrugIntroductionStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  sstm << NestedMFTStrategy::to_string();
  return sstm.str();
}

void NovelDrugIntroductionStrategy::monthly_update() {
  NestedMFTStrategy::monthly_update();

  if (!is_switched) {
    auto* public_sector_strategy = strategy_list[0];

    int current_public_therapy_id =
        public_sector_strategy->type == StrategyType::SFT
            ? dynamic_cast<SFTStrategy*>(public_sector_strategy)->get_therapy_list()[0]->get_id()
            : dynamic_cast<MFTStrategy*>(public_sector_strategy)->therapy_list[0]->get_id();
    if (Model::get_scheduler()->current_time() > 3000
        && Model::get_mdc()->current_tf_by_therapy()[current_public_therapy_id] >= tf_threshold) {
      // switch to novel drugs

      auto* novel_sft_strategy = Model::get_strategy_db()[newly_introduced_strategy_id].get();

      auto new_public_stategy = std::make_unique<NestedMFTStrategy>();

      new_public_stategy->strategy_list.push_back(public_sector_strategy);
      new_public_stategy->strategy_list.push_back(novel_sft_strategy);
      new_public_stategy->distribution.push_back(1 - replacement_fraction);
      new_public_stategy->distribution.push_back(replacement_fraction);

      new_public_stategy->start_distribution.push_back(1);
      new_public_stategy->start_distribution.push_back(0);

      new_public_stategy->peak_distribution.push_back(1 - replacement_fraction);
      new_public_stategy->peak_distribution.push_back(replacement_fraction);

      new_public_stategy->peak_after = replacement_duration;
      new_public_stategy->starting_time = Model::get_scheduler()->current_time();

      strategy_list[0] = new_public_stategy.get();
      new_public_stategy->id = static_cast<int>(Model::get_strategy_db().size());

      Model::get_strategy_db().push_back(std::move(new_public_stategy));

      // reset the time point to collect ntf
      //  Model::get_config()->get_simulation_timeframe().set_start_of_comparison_period(Model::get_scheduler()->current_time());

      // reset the total time to 10 years after this time point
      //  const auto new_total_time = Model::get_scheduler()->current_time() +
      //  Model::get_scheduler()->get_days_to_next_n_year(10);

      // if (new_total_time > Model::get_config()->get_simulation_timeframe().get_total_time()) {
      //   //extend the scheduler
      //   Model::get_scheduler()->extend_total_time(new_total_time);
      // }
      // Model::get_config()->get_simulation_timeframe().set_total_time(new_total_time);

      std::cout << Model::get_scheduler()->get_current_date_string()
                << ": Switch to novel drug with id " << newly_introduced_strategy_id;
      // std::cout << "New total time: " << new_total_time;
      is_switched = true;
    } else {
      // check and extend total time if total time is less than 10 years
      // const auto new_total_time = Model::get_scheduler()->current_time() +
      // Model::get_scheduler()->get_days_to_next_n_year(10);

      // if (new_total_time > Model::get_config()->get_simulation_timeframe().get_total_time()) {
      //   //extend the scheduler
      //   Model::get_scheduler()->extend_total_time(new_total_time + 10 * 365);
      //   Model::get_config()->get_simulation_timeframe().set_total_time(new_total_time + 10 *
      //   365);
      //   Model::get_config()->get_simulation_timeframe().set_start_of_comparison_period(new_total_time
      //   + 10 * 365);
      // }
    }
  }
}
