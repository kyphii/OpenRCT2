/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/MemoryStream.h"
#include "../core/Money.hpp"
#include "../localisation/StringIds.h"
#include "../object/ObjectTypes.h"
#include "../ride/RideRatings.h"

#include <vector>

namespace OpenRCT2
{
    struct GameState_t;
}

namespace OpenRCT2::Park
{
    struct ParkData;
}

namespace OpenRCT2::Scenario
{
    static constexpr uint16_t kObjectiveGuestsDefault   = 1500;
    static constexpr uint16_t kObjectiveGuestsMin       = 250;
    static constexpr uint16_t kObjectiveGuestsMax       = 50000;
    static constexpr uint16_t kObjectiveGuestsIncrement = 50;

    static constexpr uint16_t kObjectiveParkRatingSustain   = 700;
    static constexpr uint16_t kObjectiveParkRatingDefault   = 600;
    static constexpr uint16_t kObjectiveParkRatingMin       = 250;
    static constexpr uint16_t kObjectiveParkRatingMax       = 950;
    static constexpr uint16_t kObjectiveParkRatingIncrement = 50;

    static constexpr money64 kObjectiveParkValueDefault   = 100000.00_GBP;
    static constexpr money64 kObjectiveParkValueMin       = 1000.00_GBP;
    static constexpr money64 kObjectiveParkValueMax       = 2000000.00_GBP;
    static constexpr money64 kObjectiveParkValueIncrement = 1000.00_GBP;

    static constexpr money64 kObjectiveIncomeRidesDefault = 10000.00_GBP;
    static constexpr money64 kObjectiveIncomeShopsDefault = 1000.00_GBP;
    static constexpr money64 kObjectiveIncomeMin          = 1000.00_GBP;
    static constexpr money64 kObjectiveIncomeMax          = 2000000.00_GBP;
    static constexpr money64 kObjectiveIncomeIncrement    = 100.00_GBP;

    static constexpr uint16_t kObjectiveCoastersDefault   = 10;
    static constexpr uint16_t kObjectiveCoastersMin       = 5;
    static constexpr uint16_t kObjectiveCoastersMax       = 20;
    static constexpr uint16_t kObjectiveCoastersIncrement = 1;

    static constexpr RideRating_t kObjectiveExcitementDefault    = RideRating::make(7, 00);
    static constexpr RideRating_t kObjectiveExcitementMin        = RideRating::make(4, 00);
    static constexpr RideRating_t kObjectiveExcitementMax        = RideRating::make(9, 90);
    static constexpr RideRating_t kObjectiveExcitementIncrement  = RideRating::make(0, 10);

    static constexpr uint16_t kObjectiveLengthDefault   = 5000;
    static constexpr uint16_t kObjectiveLengthMin       = 500;
    static constexpr uint16_t kObjectiveLengthMax       = 5000;
    static constexpr uint16_t kObjectiveLengthIncrement = 100;

    static constexpr uint8_t kObjectiveYearNoDeadline = 0;
    static constexpr uint8_t kObjectiveYearDefault    = 3;
    static constexpr uint8_t kObjectiveYearMin        = 1;
    static constexpr uint8_t kObjectiveYearMax        = 25;
    static constexpr uint8_t kObjectiveYearIncrement  = 1;

    static constexpr size_t kMaxObjectiveGoals = 3;
    static constexpr size_t kMaxGoalArguments = 4;

    struct ScenarioGoal;

    enum class ObjectiveStatus : uint8_t
    {
        Undecided,
        Success,
        Failure,
    };

    enum class GoalArgumentType : uint8_t
    {
        boolean,
        number,
        money,
        rating,
        distance,
        // RideType used by RCT1 competitions scenarios
        rideType
    };

    enum class LegacyObjectiveType : uint8_t
    {
        none,
        guestsBy,
        parkValueBy,
        haveFun,
        buildTheBest,
        tenRollercoasters,
        guestsAndRating,
        monthlyRideIncome,
        tenRollercoastersLength,
        finishFiveRollercoasters,
        repayLoanAndParkValue,
        monthlyFoodIncome,

        count
    };

    union ArgumentValue
    {
        uint16_t number;
        money64 money;
        // Used by "Build Coasters" type goals
        RideRating_t rating;
        // Used by RCT1 competitions scenarios
        ObjectEntryIndex rideType;
    };

    struct GoalArgumentDescriptor
    {
        uint8_t index;
        bool isOptional;
        bool allowClosingPark;
        GoalArgumentType type;
        // Following are determined by the goal type
        ArgumentValue defaultValue;
        ArgumentValue minimumValue;
        ArgumentValue maximumValue;
        ArgumentValue valueIncrement;

        bool operator==(const GoalArgumentDescriptor& other) const
        {
            return this->index == other.index;
        }
    };

    using GoalEvaluationFunc = ObjectiveStatus (*)(Park::ParkData& park, GameState_t& gameState, struct ScenarioGoal goal);

    struct GoalDescriptor
    {
        uint8_t index{};
        // Whether the objective type is valid when money is disabled
        bool requiresMoney{};
        // Whether the objective type is valid for pay-per-entry price models
        bool requiresRideTickets{};
        // Function by which the objective is evaluated for completion/failure
        GoalEvaluationFunc evaluationFunction{};
        const GoalArgumentDescriptor* arguments[kMaxGoalArguments];
        // Must ALWAYS match the number of arguments
        uint8_t argCount;

        bool IsValidForSettings(bool useMoney, bool canAskMoneyForRides) const
        {
            return (useMoney || !requiresMoney) && (canAskMoneyForRides || !requiresRideTickets);
        }
    };

    struct ScenarioObjectiveDescriptor
    {
        StringId name{};
        StringId dropdownDesc{};
        uint8_t deadlineYear{};
        const GoalDescriptor* goals[kMaxObjectiveGoals];

        bool IsValidForSettings(bool useMoney, bool canAskMoneyForRides) const
        {
            for (const auto& goal : goals)
            {
                if (!goal->IsValidForSettings(useMoney, canAskMoneyForRides)) {
                    return false;
                }
            }
            return true;
        }
    };

    // Modifiable argument container for active scenario
    struct ScenarioGoalArgument
    {
        GoalArgumentDescriptor* descriptor;
        ArgumentValue value{};
        bool enabled{};

        bool Increment();
        bool Decrement();
    };

    // Modifiable goal container for active scenario
    struct ScenarioGoal
    {
        GoalDescriptor* descriptor;
        std::vector<ScenarioGoalArgument*> values;

        ObjectiveStatus Evaluate(Park::ParkData& park, GameState_t& gameState) const;
        // TODO: Clean up with generics
        bool GetArgumentEnabled(size_t listIndex) const;
        uint16_t GetArgumentValueNumber(size_t listIndex) const;
        money64 GetArgumentValueMoney(size_t listIndex) const;
        RideRating_t GetArgumentValueRating(size_t listIndex) const;
        uint16_t GetArgumentValueDistance(size_t listIndex) const;
    };

    // Objective container for active scenario
    struct ScenarioObjective
    {
    public:
        // Time limit for the objective, in years. 0 = no time limit.
        uint8_t deadlineYear;
        std::vector<ScenarioGoal*> goals;
        StringId format;
        // Whether the objective description text window should open on loading a save.
        bool displayOnLoad;

        ObjectiveStatus ScenarioEvaluateObjective(Park::ParkData& park, GameState_t& gameState) const;

        // TODO: Clean up with generics
        void SetArgumentNumber(int32_t goalIndex, int32_t argIndex, uint16_t value);
        void SetArgumentMoney(int32_t goalIndex, int32_t argIndex, money64 value);
        void SetArgumentRating(int32_t goalIndex, int32_t argIndex, RideRating_t value);
        void SetArgumentDistance(int32_t goalIndex, int32_t argIndex, uint16_t value);
        void SetArgumentRideType(int32_t goalIndex, int32_t argIndex, ObjectEntryIndex value);

        ScenarioGoalArgument* GetArgumentByDescriptor(const GoalArgumentDescriptor* descriptor) const;
        uint16_t GetArgumentNumberByDescriptor(const GoalArgumentDescriptor* descriptor) const;
        money64 GetArgumentMoneyByDescriptor(const GoalArgumentDescriptor* descriptor) const;
        RideRating_t GetArgumentRatingByDescriptor(const GoalArgumentDescriptor* descriptor) const;

        bool IncrementDeadlineYear();
        bool DecrementDeadlineYear();

        void EnableArgument(int32_t goalIndex, int32_t argIndex);
        void DisableArgument(int32_t goalIndex, int32_t argIndex);
        bool IsArgumentEnabled(GoalArgumentDescriptor descriptor);

        uint8_t GetGoalCount() const;

        bool AllowsClosingPark() const;
    };

    ScenarioObjective* ScenarioObjectiveInitFromPreset(const ScenarioObjectiveDescriptor& preset);
    ScenarioObjective* ScenarioObjectiveInitFromLegacyType(
        const LegacyObjectiveType type, uint8_t arg1, int64_t arg2, uint16_t arg3);
    ScenarioObjective* ScenarioObjectiveInitFromLegacyType(
        const LegacyObjectiveType type);

    #pragma region GoalArgumentDescriptors
    constexpr ArgumentValue ArgNumber(uint16_t val) {
        return ArgumentValue{
            .number = val,
        };
    }

    constexpr ArgumentValue ArgMoney(money64 val)
    {
        return ArgumentValue{
            .money = val,
        };
    }

    constexpr ArgumentValue ArgRating(RideRating_t val)
    {
        return ArgumentValue{
            .rating = val,
        };
    }

    constexpr GoalArgumentDescriptor kArgumentGuestCount = {
        .index = 0,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::number,
        .defaultValue =   ArgNumber(kObjectiveGuestsDefault),
        .minimumValue =   ArgNumber(kObjectiveGuestsMin),
        .maximumValue =   ArgNumber(kObjectiveGuestsMax),
        .valueIncrement = ArgNumber(kObjectiveGuestsIncrement), 
    };

    constexpr GoalArgumentDescriptor kArgumentParkRating = {
        .index = 1,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::number,
        .defaultValue =   ArgMoney(kObjectiveParkRatingDefault),
        .minimumValue =   ArgMoney(kObjectiveParkRatingMin),
        .maximumValue =   ArgMoney(kObjectiveParkRatingMax),
        .valueIncrement = ArgMoney(kObjectiveParkRatingIncrement),
    };

    constexpr GoalArgumentDescriptor kArgumentSustainParkRating = {
        .index = 2,
        .isOptional = true,
        .allowClosingPark = false,
        .type = GoalArgumentType::boolean
    };

    constexpr GoalArgumentDescriptor kArgumentParkValue = {
        .index = 3,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::money,
        .defaultValue =   ArgMoney(kObjectiveParkValueDefault),
        .minimumValue =   ArgMoney(kObjectiveParkValueMin),
        .maximumValue =   ArgMoney(kObjectiveParkValueMax),
        .valueIncrement = ArgMoney(kObjectiveParkValueIncrement),
    };

    constexpr GoalArgumentDescriptor kArgumentIncomeRides = {
        .index = 4,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::money,
        .defaultValue =   ArgMoney(kObjectiveIncomeRidesDefault),
        .minimumValue =   ArgMoney(kObjectiveIncomeMin),
        .maximumValue =   ArgMoney(kObjectiveIncomeMax),
        .valueIncrement = ArgMoney(kObjectiveIncomeIncrement),
    };

    constexpr GoalArgumentDescriptor kArgumentIncomeShops = {
        .index = 5,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::money,
        .defaultValue =   ArgMoney(kObjectiveIncomeShopsDefault),
        .minimumValue =   ArgMoney(kObjectiveIncomeMin),
        .maximumValue =   ArgMoney(kObjectiveIncomeMax),
        .valueIncrement = ArgMoney(kObjectiveIncomeIncrement),
    };

    constexpr GoalArgumentDescriptor kArgumentCoasterCount = {
        .index = 6,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::number,
        .defaultValue =   ArgNumber(kObjectiveCoastersDefault),
        .minimumValue =   ArgNumber(kObjectiveCoastersMin),
        .maximumValue =   ArgNumber(kObjectiveCoastersMax),
        .valueIncrement = ArgNumber(kObjectiveCoastersIncrement),
    };

    constexpr GoalArgumentDescriptor kArgumentCoasterCompleteExisting = {
        .index = 7,
        .isOptional = true,
        .allowClosingPark = true,
        .type = GoalArgumentType::boolean
    };

    constexpr GoalArgumentDescriptor kArgumentCoasterExcitement = {
        .index = 8,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::rating,
        .defaultValue =   ArgRating(kObjectiveExcitementDefault),
        .minimumValue =   ArgRating(kObjectiveExcitementMin),
        .maximumValue =   ArgRating(kObjectiveExcitementMax),
        .valueIncrement = ArgRating(kObjectiveExcitementIncrement),
    };

    constexpr GoalArgumentDescriptor kArgumentCoasterLength = {
        .index = 9,
        .isOptional = true,
        .allowClosingPark = true,
        .type = GoalArgumentType::distance,
        .defaultValue =   ArgNumber(kObjectiveLengthDefault),
        .minimumValue =   ArgNumber(kObjectiveLengthMin),
        .maximumValue =   ArgNumber(kObjectiveLengthMax),
        .valueIncrement = ArgNumber(kObjectiveLengthIncrement),
    };

    constexpr GoalArgumentDescriptor kArgumentBuildTheBest = {
        .index = 10,
        .isOptional = false,
        .allowClosingPark = true,
        .type = GoalArgumentType::rideType,
    };

    // For proper serialization, index value and list position must match
    constexpr const GoalArgumentDescriptor* kArgumentList[] = {
        &kArgumentGuestCount,
        &kArgumentParkRating,
        &kArgumentSustainParkRating,
        &kArgumentParkValue,
        &kArgumentIncomeRides,
        &kArgumentIncomeShops,
        &kArgumentCoasterCount,
        &kArgumentCoasterCompleteExisting,
        &kArgumentCoasterExcitement,
        &kArgumentCoasterLength,
        &kArgumentBuildTheBest,
    };
    #pragma endregion

    #pragma region GoalDescriptors
    ObjectiveStatus EvaluateEmpty(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);
    ObjectiveStatus EvaluateGuests(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);
    ObjectiveStatus EvaluateParkRating(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);
    ObjectiveStatus EvaluateParkValue(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);
    ObjectiveStatus EvaluateLoan(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);
    ObjectiveStatus EvaluateIncomeRides(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);
    ObjectiveStatus EvaluateIncomeShops(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);
    ObjectiveStatus EvaluateRollerCoasters(Park::ParkData& park, GameState_t& gameState, ScenarioGoal goal);

    constexpr GoalDescriptor kGoalGuests = {
        .index = 0,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = EvaluateGuests,
        .arguments = { &kArgumentGuestCount },
        .argCount = 1,
    };

    constexpr GoalDescriptor kGoalParkRating = {
        .index = 1,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = EvaluateParkRating,
        .arguments = { &kArgumentParkRating, &kArgumentSustainParkRating },
        .argCount = 2,
    };

    constexpr GoalDescriptor kGoalParkValue = {
        .index = 2,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = EvaluateParkValue,
        .arguments = { &kArgumentParkValue },
        .argCount = 1,
    };

    constexpr GoalDescriptor kGoalRepayLoan = {
        .index = 3,
        .requiresMoney = true,
        .requiresRideTickets = false,
        .evaluationFunction = EvaluateLoan,
        .arguments = {},
        .argCount = 0,
    };

    constexpr GoalDescriptor kGoalIncomeRides = {
        .index = 4,
        .requiresMoney = true,
        .requiresRideTickets = true,
        .evaluationFunction = EvaluateIncomeRides,
        .arguments = { &kArgumentIncomeRides },
        .argCount = 1,
    };

    constexpr GoalDescriptor kGoalIncomeShops = {
        .index = 5,
        .requiresMoney = true,
        .requiresRideTickets = false,
        .evaluationFunction = EvaluateIncomeShops,
        .arguments = { &kArgumentIncomeShops },
        .argCount = 1,
    };

    constexpr GoalDescriptor kGoalCoasters = {
        .index = 6,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = EvaluateRollerCoasters,
        .arguments = { &kArgumentCoasterCount, &kArgumentCoasterExcitement, &kArgumentCoasterLength,
                       &kArgumentCoasterCompleteExisting },
        .argCount = 4,
    };

    constexpr GoalDescriptor kGoalBuildTheBest = {
        .index = 7,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = EvaluateEmpty,
        .arguments = { &kArgumentBuildTheBest },
        .argCount = 1,
    };

    constexpr const GoalDescriptor* kGoalList[] = {
        &kGoalGuests,
        &kGoalParkRating,
        &kGoalParkValue,
        &kGoalRepayLoan,
        &kGoalIncomeRides,
        &kGoalIncomeShops,
        &kGoalCoasters,
        &kGoalBuildTheBest,
    };
    #pragma endregion

    #pragma region ScenarioObjectivePresets
    constexpr ScenarioObjectiveDescriptor kObjectivePresetGuestsBy = {
        .name = STR_OBJECTIVE_GUESTS_BY,
        .deadlineYear = kObjectiveYearDefault,
        .goals = { &kGoalGuests, &kGoalParkRating }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetParkValueBy = {
        .name = STR_OBJECTIVE_PARK_VALUE_BY,
        .deadlineYear = kObjectiveYearDefault,
        .goals = { &kGoalParkValue, &kGoalParkRating }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetHaveFun = {
        .name = STR_OBJECTIVE_HAVE_FUN,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = {}
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetBuildTheBest = {
        .name = STR_OBJECTIVE_BUILD_THE_BEST,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalBuildTheBest }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetTenRollerCoasters = {
        .name = STR_OBJECTIVE_10_ROLLERCOASTERS,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalCoasters }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetGuestsAndRating = {
        .name = STR_OBJECTIVE_GUESTS_AND_RATING,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalGuests, &kGoalParkRating }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetMonthlyRideIncome = {
        .name = STR_OBJECTIVE_MONTHLY_RIDE_INCOME,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalIncomeRides }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetTenRollerCoastersLength = {
        .name = STR_OBJECTIVE_10_ROLLERCOASTERS_LENGTH,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalCoasters }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetCompleteFiveRollerCoasters = {
        .name = STR_OBJECTIVE_FINISH_5_ROLLERCOASTERS,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalCoasters }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetRepayLoanAndParkValue = {
        .name = STR_OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalParkValue, &kGoalRepayLoan }
    };

    constexpr ScenarioObjectiveDescriptor kObjectivePresetMonthlyFoodIncome = {
        .name = STR_OBJECTIVE_MONTHLY_FOOD_INCOME,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalIncomeShops }
    };

    constexpr const ScenarioObjectiveDescriptor* kObjectivePresets[] = {
        // HaveFun used to correspond to LegacyObjectiveType::none
        &kObjectivePresetHaveFun,
        &kObjectivePresetGuestsBy,
        &kObjectivePresetParkValueBy,
        &kObjectivePresetHaveFun,
        &kObjectivePresetBuildTheBest,
        &kObjectivePresetTenRollerCoasters,
        &kObjectivePresetGuestsAndRating,
        &kObjectivePresetMonthlyRideIncome,
        &kObjectivePresetTenRollerCoastersLength,
        &kObjectivePresetCompleteFiveRollerCoasters,
        &kObjectivePresetRepayLoanAndParkValue,
        &kObjectivePresetMonthlyFoodIncome,
    };
    #pragma endregion
} // namespace OpenRCT2::Scenario
