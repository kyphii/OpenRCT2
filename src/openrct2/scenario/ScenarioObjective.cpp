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

    ScenarioObjective ScenarioObjectiveInitFromPreset(const ScenarioObjectiveDescriptor& preset)
    {
        ScenarioObjective objective = {};
        objective.deadlineYear = preset.deadlineYear;
        objective.format = preset.name;
        for (const auto* goalDescriptor : preset.goals)
        {
            if (goalDescriptor == nullptr)
            {
                break;
            }
            auto newGoal = new ScenarioGoal();
            newGoal->descriptor = const_cast<GoalDescriptor*>(goalDescriptor);

            for (const auto* argumentDescriptor : goalDescriptor->arguments)
            {
                if (argumentDescriptor == nullptr)
                {
                    break;
                }
                auto newArgument = new ScenarioGoalArgument();

                newArgument->descriptor = const_cast<GoalArgumentDescriptor*>(argumentDescriptor);
                switch (newArgument->descriptor->type)
                {
                    case GoalArgumentType::number:
                    case GoalArgumentType::distance:
                        newArgument->value.number = argumentDescriptor->defaultValue.number;
                        break;
                    case GoalArgumentType::money:
                        newArgument->value.money = argumentDescriptor->defaultValue.money;
                        break;
                    case GoalArgumentType::rating:
                        newArgument->value.rating = argumentDescriptor->defaultValue.rating;
                        break;
                }
                
                newArgument->enabled = !argumentDescriptor->isOptional;

                newGoal->values.push_back(newArgument);
            }

            objective.goals.push_back(newGoal);
        }
        return objective;
    }

    ScenarioObjective ScenarioObjectiveInitFromLegacyType(
        const LegacyObjectiveType type, uint8_t arg1, int64_t arg2, uint16_t arg3)
    {
        // arg1 = years
        // arg2 = money, excitement
        // arg3 = guests, rideID, coasterLength

        ScenarioObjective objective = ScenarioObjectiveInitFromPreset(*kObjectivePresets[static_cast<uint8_t>(type)]);

        switch (type)
        {
            case LegacyObjectiveType::guestsBy:
                objective.deadlineYear = arg1;
                // Guest count
                objective.SetArgumentNumber(0, 0, arg2);
                break;
            case LegacyObjectiveType::parkValueBy:
                objective.deadlineYear = arg1;
                // Park Value
                objective.SetArgumentMoney(0, 0, arg2);
                break;
            case LegacyObjectiveType::buildTheBest:
                // Ride Type
                objective.SetArgumentRideType(0, 0, arg3);
                break;
            case LegacyObjectiveType::tenRollercoasters:
                // Excitement
                objective.SetArgumentRating(0, 1, arg2);
                break;
            case LegacyObjectiveType::guestsAndRating:
                // Guest count
                objective.SetArgumentNumber(0, 0, arg2);
                break;
            case LegacyObjectiveType::monthlyRideIncome:
                // Income
                objective.SetArgumentMoney(0, 0, arg2);
                break;
            case LegacyObjectiveType::tenRollercoastersLength:
                // Excitement
                objective.SetArgumentRating(0, 1, arg2);
                // Length
                objective.SetArgumentDistance(0, 2, arg3);
                break;
            case LegacyObjectiveType::finishFiveRollercoasters:
                // Excitement
                objective.SetArgumentRating(0, 1, arg2);
                break;
            case LegacyObjectiveType::repayLoanAndParkValue:
                // Park Value
                objective.SetArgumentMoney(0, 0, arg2);
                break;
            case LegacyObjectiveType::monthlyFoodIncome:
                // Income
                objective.SetArgumentMoney(0, 0, arg2);
                break;
            default:
                objective = ScenarioObjectiveInitFromPreset(kObjectivePresetHaveFun);
        }
        return objective;
    }

    ScenarioObjective ScenarioObjectiveInitFromLegacyType(const LegacyObjectiveType type)
    {
        ScenarioObjective objective = ScenarioObjectiveInitFromPreset(*kObjectivePresets[static_cast<uint8_t>(type)]);
        switch (type)
        {
            case LegacyObjectiveType::guestsAndRating:
                // Park rating
                objective.SetArgumentNumber(1, 0, kObjectiveParkRatingSustain);
                // Enable sustain checks
                objective.EnableArgument(1, 1);
                break;
            case LegacyObjectiveType::tenRollercoastersLength:
                // Enable length requirement
                objective.EnableArgument(0, 2);
                break;
            case LegacyObjectiveType::finishFiveRollercoasters:
                // Coaster Count
                objective.SetArgumentNumber(0, 0, 5);
                // Enable finish prebuilts
                objective.EnableArgument(0, 3);
                break;
        }
        return objective;
    }

    void ScenarioObjective::SetArgumentNumber(int32_t goalIndex, int32_t argIndex, uint16_t value)
    {
        auto* argument = this->goals.at(goalIndex)->values.at(argIndex);
        Guard::Assert(argument->descriptor->type == GoalArgumentType::number);
        argument->value.number = std::clamp(
            value, argument->descriptor->minimumValue.number, argument->descriptor->maximumValue.number);
    }

    void ScenarioObjective::SetArgumentMoney(int32_t goalIndex, int32_t argIndex, money64 value)
    {
        auto* argument = this->goals.at(goalIndex)->values.at(argIndex);
        Guard::Assert(argument->descriptor->type == GoalArgumentType::money);
        argument->value.money = std::clamp(
            value, argument->descriptor->minimumValue.money, argument->descriptor->maximumValue.money);
    }

    void ScenarioObjective::SetArgumentRating(int32_t goalIndex, int32_t argIndex, RideRating_t value)
    {
        auto* argument = this->goals.at(goalIndex)->values.at(argIndex);
        Guard::Assert(argument->descriptor->type == GoalArgumentType::rating);
        argument->value.rating = std::clamp(
            value, argument->descriptor->minimumValue.rating, argument->descriptor->maximumValue.rating);
    }

    void ScenarioObjective::SetArgumentDistance(int32_t goalIndex, int32_t argIndex, uint16_t value)
    {
        auto* argument = this->goals.at(goalIndex)->values.at(argIndex);
        Guard::Assert(argument->descriptor->type == GoalArgumentType::distance);
        argument->value.number = std::clamp(
            value, argument->descriptor->minimumValue.number, argument->descriptor->maximumValue.number);
    }

    void ScenarioObjective::SetArgumentRideType(int32_t goalIndex, int32_t argIndex, ObjectEntryIndex value)
    {
        auto* argument = this->goals.at(goalIndex)->values.at(argIndex);
        Guard::Assert(argument->descriptor->type == GoalArgumentType::rideType);
        argument->value.rideType = value;
    }

    void ScenarioObjective::EnableArgument(int32_t goalIndex, int32_t argIndex)
    {
        this->goals.at(goalIndex)->values.at(argIndex)->enabled = true;
    }

    void ScenarioObjective::DisableArgument(int32_t goalIndex, int32_t argIndex)
    {
        this->goals.at(goalIndex)->values.at(argIndex)->enabled = false;
    }

    bool ScenarioObjective::IsArgumentEnabled(GoalArgumentDescriptor descriptor)
    {
        for (auto& goal : this->goals)
        {
            if (goal == nullptr)
            {
                break;
            }
            for (auto& arg : goal->values)
            {
                if (arg->descriptor == &descriptor && arg->enabled)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool ScenarioObjective::IncrementDeadlineYear()
    {
        uint8_t expectedResult = this->deadlineYear + kObjectiveYearIncrement;
        this->deadlineYear = std::min(expectedResult, kObjectiveYearMax);
        return this->deadlineYear == expectedResult;
    }

    bool ScenarioObjective::DecrementDeadlineYear()
    {
        uint8_t expectedResult = this->deadlineYear - kObjectiveYearIncrement;
        this->deadlineYear = std::max(expectedResult, kObjectiveYearMin);
        return this->deadlineYear == expectedResult;
    }

    bool ScenarioGoalArgument::Increment()
    {
        switch (this->descriptor->type)
        {
            case GoalArgumentType::number:
            case GoalArgumentType::distance:
            {
                uint16_t expNumber = this->value.number + this->descriptor->valueIncrement.number;
                this->value.number = std::min(expNumber, this->descriptor->maximumValue.number);
                return this->value.number == expNumber;
            }
            case GoalArgumentType::money:
            {
                money64 expMoney = this->value.money + this->descriptor->valueIncrement.money;
                this->value.money = std::min(expMoney, this->descriptor->maximumValue.money);
                return this->value.money == expMoney;
            }
            case GoalArgumentType::rating:
            {
                RideRating_t expRating = this->value.rating + this->descriptor->valueIncrement.rating;
                this->value.rating = std::min(expRating, this->descriptor->maximumValue.rating);
                return this->value.rating == expRating;
            }
        }
        return false;
    }

    bool ScenarioGoalArgument::Decrement()
    {
        switch (this->descriptor->type)
        {
            case GoalArgumentType::number:
            case GoalArgumentType::distance:
            {
                uint16_t expNumber = this->value.number - this->descriptor->valueIncrement.number;
                this->value.number = std::max(expNumber, this->descriptor->minimumValue.number);
                return this->value.number == expNumber;
            }
            case GoalArgumentType::money:
            {
                money64 expMoney = this->value.money - this->descriptor->valueIncrement.money;
                this->value.money = std::max(expMoney, this->descriptor->minimumValue.money);
                return this->value.money == expMoney;
            }
            case GoalArgumentType::rating:
            {
                RideRating_t expRating = this->value.rating - this->descriptor->valueIncrement.rating;
                this->value.rating = std::max(expRating, this->descriptor->minimumValue.rating);
                return this->value.rating == expRating;
            }
        }
        return false;
    }

    ScenarioGoalArgument* ScenarioObjective::GetArgumentByDescriptor(const GoalArgumentDescriptor* descriptor) const
    {
        for (auto& goal : this->goals)
        {
            for (auto& arg : goal->values)
            {
                if (*arg->descriptor == *descriptor && arg->enabled)
                {
                    return arg;
                }
            }
        }
        return nullptr;
    }

    uint16_t ScenarioObjective::GetArgumentNumberByDescriptor(const GoalArgumentDescriptor* descriptor) const
    {
        auto* arg = GetArgumentByDescriptor(descriptor);
        return (arg != nullptr) ? arg->value.number : 0;
    }

    money64 ScenarioObjective::GetArgumentMoneyByDescriptor(const GoalArgumentDescriptor* descriptor) const
    {
        auto* arg = GetArgumentByDescriptor(descriptor);
        return (arg != nullptr) ? arg->value.money : 0;
    }

    RideRating_t ScenarioObjective::GetArgumentRatingByDescriptor(const GoalArgumentDescriptor* descriptor) const
    {
        auto* arg = GetArgumentByDescriptor(descriptor);
        return (arg != nullptr) ? arg->value.rating : 0;
    }

    uint8_t ScenarioObjective::GetGoalCount() const
    {
        uint8_t i = 0;
        for (auto& goal : this->goals)
        {
            if (goal->descriptor == nullptr)
            {
                return i;
            }
            else
            {
                i++;
            }
        }
        return i;
    }

    bool ScenarioObjective::AllowsClosingPark() const
    {
        for (auto& goal : this->goals)
        {
            for (auto& arg : goal->values)
            {
                if (!&arg->descriptor->allowClosingPark && arg->enabled)
                {
                    return false;
                }
            }
        }
        return true;
    }

    ObjectiveStatus ScenarioObjective::ScenarioEvaluateObjective(Park::ParkData& park, GameState_t& gameState) const
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

    ObjectiveStatus ScenarioGoal::Evaluate(Park::ParkData& park, GameState_t& gameState) const
    {
        return this->descriptor->evaluationFunction(park, gameState, *this);
    }

    bool ScenarioGoal::GetArgumentEnabled(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex)->descriptor->type == GoalArgumentType::boolean);
        return this->values.at(listIndex)->enabled;
    }

    uint16_t ScenarioGoal::GetArgumentValueNumber(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex)->descriptor->type == GoalArgumentType::number);
        return this->values.at(listIndex)->value.number;
    }

    money64 ScenarioGoal::GetArgumentValueMoney(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex)->descriptor->type == GoalArgumentType::money);
        return this->values.at(listIndex)->value.money;
    }

    RideRating_t ScenarioGoal::GetArgumentValueRating(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex)->descriptor->type == GoalArgumentType::rating);
        return this->values.at(listIndex)->value.rating;
    }

    uint16_t ScenarioGoal::GetArgumentValueDistance(size_t listIndex) const
    {
        Guard::Assert(this->values.at(listIndex)->descriptor->type == GoalArgumentType::distance);
        return this->values.at(listIndex)->value.number;
    }

    static ObjectiveStatus StatusFromBool(bool input)
    {
        return (input) ? ObjectiveStatus::Success : ObjectiveStatus::Undecided;
    }

    #pragma region EvaluationFunctions
    // Empty evaluator for sandbox-type objectives
    ObjectiveStatus EvaluateEmpty(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus EvaluateGuests(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        uint16_t reqGuestCount = goal.GetArgumentValueNumber(0);
        return StatusFromBool(park.numGuestsInPark >= reqGuestCount);
    }

    ObjectiveStatus EvaluateParkRating(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        uint16_t reqParkRating = goal.GetArgumentValueNumber(0);
        bool reqSustain = goal.GetArgumentEnabled(1);

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

    ObjectiveStatus EvaluateParkValue(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        money64 reqParkValue = goal.GetArgumentValueMoney(0);
        return StatusFromBool(park.value >= reqParkValue);
    }

    ObjectiveStatus EvaluateLoan(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        return StatusFromBool(park.bankLoan <= 0);
    }

    ObjectiveStatus EvaluateIncomeRides(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        money64 reqIncome = goal.GetArgumentValueMoney(0);
        money64 lastMonthRideIncome = park.expenditureTable[1][EnumValue(ExpenditureType::parkRideTickets)];
        return StatusFromBool(lastMonthRideIncome >= reqIncome);
    }

    ObjectiveStatus EvaluateIncomeShops(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        money64 reqIncome = goal.GetArgumentValueMoney(0);
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

    ObjectiveStatus EvaluateRollerCoasters(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal)
    {
        uint16_t reqCoasterCount = goal.GetArgumentValueNumber(0);
        RideRating_t reqCoasterExcitement = goal.GetArgumentValueRating(1);
        bool coasterLengthRequired = goal.GetArgumentEnabled(2);
        uint16_t reqCoasterLength = goal.GetArgumentValueDistance(2);
        bool coasterCompletionRequired = goal.GetArgumentEnabled(3);

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
