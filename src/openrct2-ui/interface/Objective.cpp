/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Objective.h"

#include <cstdint>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/ScenarioObjective.h>

namespace OpenRCT2::Ui
{
    static StringId GetRideTypeString(const Scenario::Objective* objective)
    {
        auto rideTypeId = objective->GetArgumentValue<ObjectEntryIndex>(Scenario::LegacyObjectiveType::buildTheBest);
        if (rideTypeId != kRideTypeNull && rideTypeId < RIDE_TYPE_COUNT)
        {
            return GetRideTypeDescriptor(rideTypeId).Naming.Name;
        }
        return kStringIdNone;
    }

    void formatObjective(Formatter& ft, const Scenario::Objective* objective)
    {
        switch (objective->format)
        {
            case STR_OBJECTIVE_GUESTS_BY:
                ft.Add<int32_t>(objective->GetArgumentValue<uint16_t>(Scenario::LegacyObjectiveType::guestsBy));
                ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective->deadlineYear));
                break;
            case STR_OBJECTIVE_PARK_VALUE_BY:
                // buffer values
                ft.Add<int16_t>(0);
                ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective->deadlineYear));
                ft.Add<money64>(objective->GetArgumentValue<money64>(Scenario::LegacyObjectiveType::parkValueBy));
                break;
            case STR_OBJECTIVE_HAVE_FUN:
                break;
            case STR_OBJECTIVE_BUILD_THE_BEST:
                ft.Add<StringId>(GetRideTypeString(objective));
                break;
            case STR_OBJECTIVE_GUESTS_AND_RATING:
                ft.Add<int32_t>(objective->GetArgumentValue<uint16_t>(Scenario::LegacyObjectiveType::guestsAndRating));
                break;
            case STR_OBJECTIVE_MONTHLY_RIDE_INCOME:
                ft.Add<int16_t>(0);
                ft.Add<int16_t>(0);
                ft.Add<money64>(objective->GetArgumentValue<money64>(Scenario::LegacyObjectiveType::monthlyRideIncome));
                break;
            case STR_OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                ft.Add<int16_t>(objective->GetArgumentValue<uint16_t>(Scenario::LegacyObjectiveType::tenRollercoastersLength));
                break;
            case STR_OBJECTIVE_10_ROLLERCOASTERS:
            case STR_OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                ft.Add<int16_t>(0);
                ft.Add<int16_t>(0);
                ft.Add<RideRating_t>(objective->GetArgumentValue<RideRating_t>(Scenario::LegacyObjectiveType::tenRollercoasters));
                break;
            case STR_OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
                ft.Add<int16_t>(0);
                ft.Add<int16_t>(0);
                ft.Add<money64>(objective->GetArgumentValue<money64>(Scenario::LegacyObjectiveType::repayLoanAndParkValue));
                break;
            case STR_OBJECTIVE_MONTHLY_FOOD_INCOME:
                ft.Add<int16_t>(0);
                ft.Add<int16_t>(0);
                ft.Add<money64>(objective->GetArgumentValue<money64>(Scenario::LegacyObjectiveType::monthlyFoodIncome));
                break;
            default:
                // TODO: Format description for customized objectives
                break;
        }
    }
} // namespace OpenRCT2::Ui
