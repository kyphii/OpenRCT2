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

    Objective* ScenarioObjectiveInitFromPreset(const ObjectiveDescriptor& preset)
    {
        auto* objective = new Objective();
        objective->deadlineYear = preset.deadlineYear;
        objective->format = preset.name;
        for (const auto* goalDescriptor : preset.goals)
        {
            if (goalDescriptor == nullptr)
            {
                break;
            }
            auto* newGoal = new ObjectiveGoal();
            newGoal->descriptor = const_cast<GoalDescriptor*>(goalDescriptor);

            for (const auto* argumentDescriptor : goalDescriptor->arguments)
            {
                if (argumentDescriptor == nullptr)
                {
                    break;
                }
                auto* newArgument = new ObjectiveArgument();

                newArgument->descriptor = const_cast<ArgumentDescriptor*>(argumentDescriptor);
                switch (newArgument->descriptor->type)
                {
                    case ArgumentType::number:
                    case ArgumentType::distance:
                        newArgument->value.number = argumentDescriptor->defaultValue.number;
                        break;
                    case ArgumentType::money:
                        newArgument->value.money = argumentDescriptor->defaultValue.money;
                        break;
                    case ArgumentType::rating:
                        newArgument->value.rating = argumentDescriptor->defaultValue.rating;
                        break;
                }
                
                newArgument->enabled = !argumentDescriptor->isOptional;

                newGoal->args.push_back(newArgument);
            }

            objective->goals.push_back(newGoal);
        }
        return objective;
    }

    Objective* ScenarioObjectiveInitFromLegacyType(
        const LegacyObjectiveType type, uint8_t arg1, int64_t arg2, uint16_t arg3)
    {
        // arg1 = years
        // arg2 = money, excitement
        // arg3 = guests, rideID, coasterLength

        auto* objective = ScenarioObjectiveInitFromLegacyType(type);

        switch (type)
        {
            case LegacyObjectiveType::guestsBy:
                objective->deadlineYear = arg1;
                // Guest count
                objective->SetArgumentValue(type, arg3);
                break;
            case LegacyObjectiveType::parkValueBy:
                objective->deadlineYear = arg1;
                // Park Value
                objective->SetArgumentValue(type, arg2);
                break;
            case LegacyObjectiveType::buildTheBest:
                // Ride Type
                objective->SetArgumentValue(type, arg3);
                break;
            case LegacyObjectiveType::tenRollercoasters:
                // Excitement
                objective->SetArgumentValue(type, arg2);
                break;
            case LegacyObjectiveType::guestsAndRating:
                // Guest count
                objective->SetArgumentValue(type, arg3);
                break;
            case LegacyObjectiveType::monthlyRideIncome:
                // Income
                objective->SetArgumentValue(type, arg2);
                break;
            case LegacyObjectiveType::tenRollercoastersLength:
                // Excitement
                objective->SetArgumentValue(&kArgumentCoasterExcitement, arg2);
                // Length
                objective->SetArgumentValue<uint16_t>(type, arg3);
                break;
            case LegacyObjectiveType::finishFiveRollercoasters:
                // Excitement
                objective->SetArgumentValue(type, arg2);
                break;
            case LegacyObjectiveType::repayLoanAndParkValue:
                // Park Value
                objective->SetArgumentValue(type, arg2);
                break;
            case LegacyObjectiveType::monthlyFoodIncome:
                // Income
                objective->SetArgumentValue(type, arg2);
                break;
        }
        return objective;
    }

    Objective* ScenarioObjectiveInitFromLegacyType(const LegacyObjectiveType type)
    {
        auto* objective = ScenarioObjectiveInitFromPreset(*kObjectivePresets[static_cast<uint8_t>(type)]);
        switch (type)
        {
            case LegacyObjectiveType::guestsAndRating:
                objective->SetArgumentValue<uint16_t>(&kArgumentParkRating, kObjectiveParkRatingSustain);
                objective->EnableArgument(&kArgumentSustainParkRating);
                break;
            case LegacyObjectiveType::tenRollercoastersLength:
                objective->EnableArgument(&kArgumentCoasterLength);
                break;
            case LegacyObjectiveType::finishFiveRollercoasters:
                // Coaster Count
                objective->SetArgumentValue<uint16_t>(&kArgumentCoasterCount, 5);
                // Enable finish prebuilts
                objective->EnableArgument(&kArgumentCoasterCompleteExisting);
                break;
        }
        return objective;
    }

    bool ObjectiveArgument::Increment()
    {
        switch (this->descriptor->type)
        {
            case ArgumentType::number:
            case ArgumentType::distance:
            {
                uint16_t expected = this->GetValue<uint16_t>() + this->descriptor->ValueIncrement<uint16_t>();
                this->SetValue<uint16_t>(std::min(expected, this->descriptor->MaximumValue<uint16_t>()));
                return this->GetValue<uint16_t>() == expected;
            }
            case ArgumentType::money:
            {
                money64 expected = this->GetValue<money64>() + this->descriptor->ValueIncrement<money64>();
                this->SetValue<money64>(std::min(expected, this->descriptor->MaximumValue<money64>()));
                return this->GetValue<money64>() == expected;
            }
            case ArgumentType::rating:
            {
                RideRating_t expected = this->GetValue<RideRating_t>() + this->descriptor->ValueIncrement<RideRating_t>();
                this->SetValue<RideRating_t>(std::min(expected, this->descriptor->MaximumValue<RideRating_t>()));
                return this->GetValue<RideRating_t>() == expected;
            }
        }
        return false;
    }

    bool ObjectiveArgument::Decrement()
    {
        switch (this->descriptor->type)
        {
            case ArgumentType::number:
            case ArgumentType::distance:
            {
                uint16_t expected = this->GetValue<uint16_t>() - this->descriptor->ValueIncrement<uint16_t>();
                this->SetValue<uint16_t>(std::min(expected, this->descriptor->MaximumValue<uint16_t>()));
                return this->GetValue<uint16_t>() == expected;
            }
            case ArgumentType::money:
            {
                money64 expected = this->GetValue<money64>() - this->descriptor->ValueIncrement<money64>();
                this->SetValue<money64>(std::min(expected, this->descriptor->MaximumValue<money64>()));
                return this->GetValue<money64>() == expected;
            }
            case ArgumentType::rating:
            {
                RideRating_t expected = this->GetValue<RideRating_t>() - this->descriptor->ValueIncrement<RideRating_t>();
                this->SetValue<RideRating_t>(std::min(expected, this->descriptor->MaximumValue<RideRating_t>()));
                return this->GetValue<RideRating_t>() == expected;
            }
        }
        return false;
    }

    bool Objective::AllowsClosingPark() const
    {
        for (auto& goal : this->goals)
        {
            for (auto& arg : goal->args)
            {
                if (!&arg->descriptor->allowClosingPark && arg->enabled)
                {
                    return false;
                }
            }
        }
        return true;
    }

    ObjectiveStatus Objective::ScenarioEvaluateObjective(Park::ParkData& park, GameState_t& gameState) const
    {
        // Quit out if scenario was already completed or failed
        if (gameState.scenarioCompletedCompanyValue != kMoney64Undefined)
        {
            return ObjectiveStatus::Undecided;
        }

        bool hasDeadline = this->deadlineYear > 0;
        bool atDeadline = hasDeadline && (GetDate().GetMonthsElapsed() >= static_cast<uint32_t>(this->deadlineYear * MONTH_COUNT));
        bool eligibleForCompletion = AllowEarlyCompletion() || atDeadline || !hasDeadline;

        for (auto& goal : this->goals)
        {
            if (goal == nullptr)
            {
                break;
            }
            const auto& goalStatus = goal->Evaluate(park, gameState);
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

    ObjectiveStatus ObjectiveGoal::Evaluate(Park::ParkData& park, GameState_t& gameState) const
    {
        return this->descriptor->evaluationFunction(park, gameState, *this);
    }

    static ObjectiveStatus StatusFromBool(bool input)
    {
        return (input) ? ObjectiveStatus::Success : ObjectiveStatus::Undecided;
    }

    #pragma region EvaluationFunctions
    // Empty evaluator for sandbox-type objectives
    ObjectiveStatus EvaluateEmpty(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus EvaluateGuests(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        uint16_t reqGuestCount = goal.GetArgumentValue<uint16_t>(&kArgumentGuestCount);
        return StatusFromBool(park.numGuestsInPark >= reqGuestCount);
    }

    ObjectiveStatus EvaluateParkRating(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        uint16_t reqParkRating = goal.GetArgumentValue<uint16_t>(&kArgumentParkRating);
        bool reqSustain = goal.IsArgumentEnabled(&kArgumentSustainParkRating);

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

    ObjectiveStatus EvaluateParkValue(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        money64 reqParkValue = goal.GetArgumentValue<money64>(&kArgumentParkValue);
        return StatusFromBool(park.value >= reqParkValue);
    }

    ObjectiveStatus EvaluateLoan(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        return StatusFromBool(park.bankLoan <= 0);
    }

    ObjectiveStatus EvaluateIncomeRides(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        money64 reqIncome = goal.GetArgumentValue<money64>(&kArgumentIncomeRides);
        money64 lastMonthRideIncome = park.expenditureTable[1][EnumValue(ExpenditureType::parkRideTickets)];
        return StatusFromBool(lastMonthRideIncome >= reqIncome);
    }

    ObjectiveStatus EvaluateIncomeShops(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        money64 reqIncome = goal.GetArgumentValue<money64>(&kArgumentIncomeShops);
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

    ObjectiveStatus EvaluateRollerCoasters(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal)
    {
        uint16_t reqCoasterCount = goal.GetArgumentValue<uint16_t>(&kArgumentCoasterCount);
        RideRating_t reqCoasterExcitement = goal.GetArgumentValue<RideRating_t>(&kArgumentCoasterExcitement);
        bool coasterLengthRequired = goal.IsArgumentEnabled(&kArgumentCoasterLength);
        uint16_t reqCoasterLength = goal.GetArgumentValue<uint16_t>(&kArgumentCoasterLength);
        bool coasterCompletionRequired = goal.IsArgumentEnabled(&kArgumentCoasterCompleteExisting);

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
    #pragma endregion
} // namespace OpenRCT2::Scenario
