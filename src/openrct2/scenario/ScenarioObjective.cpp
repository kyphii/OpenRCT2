/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioObjective.h"

#include "../GameState.h"
#include "../config/Config.h"
#include "../core/UnitConversion.h"
#include "../object/ObjectLimits.h"
#include "../ride/RideManager.hpp"
#include "Scenario.h"

namespace OpenRCT2::Scenario
{
    // For convenience
    static ObjectiveStatus StatusFromBool(bool input)
    {
        return (input) ? ObjectiveStatus::Success : ObjectiveStatus::Undecided;
    }

    ObjectiveStatus ScenarioObjective::ScenarioEvaluateObjective(Park::ParkData& park, GameState_t& gameState) const
    {
        ObjectiveStatus status = ObjectiveStatus::Success;

        bool hasDeadline = this->deadlineYear > 0;
        bool atDeadline = hasDeadline && (GetDate().GetMonthsElapsed() >= this->deadlineYear * MONTH_COUNT);
        bool earlyCompletionEnabled = AllowEarlyCompletion();
        bool eligibleForCompletion = earlyCompletionEnabled || atDeadline;

        for (auto& goal : this->goals)
        {
            const auto& goalStatus = goal.Evaluate(park, gameState);
            if (goalStatus == ObjectiveStatus::Failure)
            {
                return ObjectiveStatus::Failure;
            }
            else if (goalStatus == ObjectiveStatus::Undecided)
            {
                eligibleForCompletion = false;
            }
        }

        if (eligibleForCompletion)
        {
            return ObjectiveStatus::Success;
        }
        else if (atDeadline)
        {
            return ObjectiveStatus::Failure;
        }
        else
        {
            return ObjectiveStatus::Undecided;
        }
    }

    ObjectiveStatus ScenarioGoal::Evaluate(Park::ParkData& park, GameState_t& gameState) const
    {
        return this->descriptor->evaluationFunction(park, gameState, this);
    }

    bool ScenarioGoal::GetModifierEnabled(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex).descriptor->type == GoalModifierType::boolean);
        return this->values.at(listIndex).enabled;
    }

    uint16_t ScenarioGoal::GetModifierValueNumber(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex).descriptor->type == GoalModifierType::number);
        return this->values.at(listIndex).value.number;
    }

    money64 ScenarioGoal::GetModifierValueMoney(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex).descriptor->type == GoalModifierType::money);
        return this->values.at(listIndex).value.money;
    }

    RideRating_t ScenarioGoal::GetModifierValueRating(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex).descriptor->type == GoalModifierType::rating);
        return this->values.at(listIndex).value.rating;
    }

    uint16_t ScenarioGoal::GetModifierValueDistance(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex).descriptor->type == GoalModifierType::distance);
        return this->values.at(listIndex).value.number;
    }

    ObjectiveStatus GoalDescriptor::GoalEvaluateGuests(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal)
    {
        uint16_t reqGuestCount = goal.GetModifierValueNumber(0);
        return StatusFromBool(park.numGuestsInPark >= reqGuestCount);
    }

    ObjectiveStatus GoalDescriptor::GoalEvaluateParkRating(
        Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal)
    {
        uint16_t reqParkRating = goal.GetModifierValueNumber(0);
        bool reqSustain = goal.GetModifierEnabled(1);

        // Handle warning & failure thresholds for sustained park rating
        if (reqSustain && park.rating < reqParkRating && GetDate().GetMonthsElapsed() >= 1)
        {
            gameState.scenarioParkRatingWarningDays++;
            if (gameState.scenarioParkRatingWarningDays == 1)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_4_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 8)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_3_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 15)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_2_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 22)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_1_WEEK_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 29)
            {
                News::AddItemToQueue(News::ItemType::graph, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0, {});
                park.flags &= ~PARK_FLAGS_PARK_OPEN;
                gameState.scenarioOptions.guestInitialHappiness = 50;
                return ObjectiveStatus::Failure;
            }
        }
        else if (gameState.scenarioCompletedCompanyValue != kCompanyValueOnFailedObjective)
        {
            gameState.scenarioParkRatingWarningDays = 0;
        }

        return StatusFromBool(park.rating >= reqParkRating);
    }

    ObjectiveStatus GoalDescriptor::GoalEvaluateParkValue(
        Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal)
    {
        money64 reqParkValue = goal.GetModifierValueMoney(0);
        return StatusFromBool(park.value >= reqParkValue);
    }

    ObjectiveStatus GoalDescriptor::GoalEvaluateLoan(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal)
    {
        return StatusFromBool(park.bankLoan <= 0);
    }

    ObjectiveStatus GoalDescriptor::GoalEvaluateIncomeRides(
        Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal)
    {
        money64 reqIncome = goal.GetModifierValueMoney(0);
        money64 lastMonthRideIncome = park.expenditureTable[1][EnumValue(ExpenditureType::parkRideTickets)];
        return StatusFromBool(lastMonthRideIncome >= reqIncome);
    }

    ObjectiveStatus GoalDescriptor::GoalEvaluateIncomeShops(
        Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal)
    {
        money64 reqIncome = goal.GetModifierValueMoney(0);
        const auto* lastMonthExpenditure = park.expenditureTable[1];
        auto lastMonthProfit = lastMonthExpenditure[EnumValue(ExpenditureType::shopSales)]
            + lastMonthExpenditure[EnumValue(ExpenditureType::shopStock)]
            + lastMonthExpenditure[EnumValue(ExpenditureType::foodDrinkSales)]
            + lastMonthExpenditure[EnumValue(ExpenditureType::foodDrinkStock)];
        return StatusFromBool(lastMonthProfit >= reqIncome);
    }

    static bool CoasterQualifiesForGoal(
        const Ride& ride, const RideObjectEntry* rideEntry, RideRating_t reqExcitement, bool isLengthRequired,
        uint16_t reqLength)
    {
        if (ride.ratings.excitement < reqExcitement)
        {
            return false;
        }
        if (isLengthRequired && ToHumanReadableRideLength(ride.getTotalLength() < reqLength))
        {
            return false;
        }
        return true;
    }

    ObjectiveStatus GoalDescriptor::GoalEvaluateRollerCoasters(
        Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal)
    {
        uint16_t reqCoasterCount = goal.GetModifierValueNumber(0);
        RideRating_t reqCoasterExcitement = goal.GetModifierValueRating(1);
        bool coasterLengthRequired = goal.GetModifierEnabled(2);
        uint16_t reqCoasterLength = goal.GetModifierValueDistance(2);
        bool coasterCompletionRequired = goal.GetModifierEnabled(3);

        BitSet<kMaxRideObjects> type_already_counted;
        uint16_t qualifiedCoasters = 0;
        for (const auto& ride : RideManager(gameState))
        {
            auto rideEntry = ride.getRideEntry();
            if (rideEntry != nullptr && ride.status == RideStatus::open
                && RideEntryHasCategory(*rideEntry, RideCategory::rollerCoaster))
            {
                if (coasterCompletionRequired)
                {
                    // Check for pre-existing rides based on whether indestructible track is present
                    if ((ride.lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
                        && CoasterQualifiesForGoal(
                            ride, rideEntry, reqCoasterExcitement, coasterLengthRequired, reqCoasterLength))
                    {
                        qualifiedCoasters++;
                    }
                }
                else if (
                    ride.subtype != kObjectEntryIndexNull && !type_already_counted[ride.subtype]
                    && CoasterQualifiesForGoal(ride, rideEntry, reqCoasterExcitement, coasterLengthRequired, reqCoasterLength))
                {
                    type_already_counted[ride.subtype] = true;
                    qualifiedCoasters++;
                }
            }
        }

        return StatusFromBool(qualifiedCoasters >= reqCoasterCount);
    }
} // namespace OpenRCT2::Scenario
