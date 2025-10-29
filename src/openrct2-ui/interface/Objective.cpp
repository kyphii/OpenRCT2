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
        // TODO: Reformat this stuff
        switch (objective.format)
        {
            case STR_OBJECTIVE_GUESTS_BY:
                ft.Add<int32_t>(objective.NumGuests);
                ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective.Year));

                break;
            case STR_OBJECTIVE_BUILD_THE_BEST:
                StringId rideTypeString = kStringIdNone;
                auto rideTypeId = objective.RideId;
                if (rideTypeId != kRideTypeNull && rideTypeId < RIDE_TYPE_COUNT)
                {
                    rideTypeString = GetRideTypeDescriptor(rideTypeId).Naming.Name;
                }
                ft.Add<StringId>(rideTypeString);

                break;
            case STR_OBJECTIVE_GUESTS_AND_RATING:
                ft.Add<int32_t>(objective.NumGuests);

                break;
            case STR_OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                ft.Add<int16_t>(objective.MinimumLength);

                break;
            default:
                ft.Add<int16_t>(objective.NumGuests);
                ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective.Year));
                if (objective.Type == Scenario::ObjectiveType::finishFiveRollercoasters)
                    ft.Add<RideRating_t>(objective.MinimumExcitement);
                else
                    ft.Add<money64>(objective.Currency);

                break;
        }
    }
} // namespace OpenRCT2::Ui
