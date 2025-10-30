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
    void formatObjective(Formatter& ft, const Scenario::ScenarioObjective& objective)
    {
        switch (objective.format)
        {
            case STR_OBJECTIVE_GUESTS_BY:
                ft.Add<int32_t>(objective.GetArgumentNumberByDescriptor(Scenario::kArgumentGuestCount));
                ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective.deadlineYear));
                break;
            case STR_OBJECTIVE_PARK_VALUE_BY:
                ft.Add<money64>(objective.GetArgumentMoneyByDescriptor(Scenario::kArgumentParkValue));
                ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective.deadlineYear));
                break;
            case STR_OBJECTIVE_HAVE_FUN:
                break;
            case STR_OBJECTIVE_BUILD_THE_BEST:
                StringId rideTypeString = kStringIdNone;
                auto rideTypeId = objective.GetArgumentByDescriptor(Scenario::kArgumentBuildTheBest)->value.rideType;
                if (rideTypeId != kRideTypeNull && rideTypeId < RIDE_TYPE_COUNT)
                {
                    rideTypeString = GetRideTypeDescriptor(rideTypeId).Naming.Name;
                }
                ft.Add<StringId>(rideTypeString);
                break;
            case STR_OBJECTIVE_GUESTS_AND_RATING:
                ft.Add<int32_t>(objective.GetArgumentNumberByDescriptor(Scenario::kArgumentGuestCount));
                break;
            case STR_OBJECTIVE_MONTHLY_RIDE_INCOME:
                ft.Add<money64>(objective.GetArgumentMoneyByDescriptor(Scenario::kArgumentIncomeRides));
                break;
            case STR_OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                ft.Add<int16_t>(objective.GetArgumentNumberByDescriptor(Scenario::kArgumentCoasterLength));
                break;
            case STR_OBJECTIVE_10_ROLLERCOASTERS:
            case STR_OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                ft.Add<RideRating_t>(objective.GetArgumentRatingByDescriptor(Scenario::kArgumentCoasterExcitement));
                break;
            case STR_OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
                ft.Add<money64>(objective.GetArgumentMoneyByDescriptor(Scenario::kArgumentParkValue));
                break;
            case STR_OBJECTIVE_MONTHLY_FOOD_INCOME:
                ft.Add<money64>(objective.GetArgumentMoneyByDescriptor(Scenario::kArgumentIncomeShops));
                break;
            default:
                // TODO: Format description for customized objectives
                break;
        }
    }
} // namespace OpenRCT2::Ui
