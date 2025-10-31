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
    #pragma region constants
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
    #pragma endregion

    struct ObjectiveGoal;

    enum class ObjectiveStatus : uint8_t
    {
        Undecided,
        Success,
        Failure,
    };

    enum class ArgumentType : uint8_t
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

    #pragma region Descriptors
    union ArgumentValue
    {
        uint16_t number;
        money64 money;
        // Used by "Build Coasters" type goals
        RideRating_t rating;
        // Used by RCT1 competitions scenarios
        ObjectEntryIndex rideType;
    };

    struct ArgumentDescriptor
    {
        uint8_t index;
        bool isOptional;
        bool allowClosingPark;
        ArgumentType type;
        ArgumentValue defaultValue;
        ArgumentValue minimumValue;
        ArgumentValue maximumValue;
        ArgumentValue valueIncrement;

        # pragma region Value Getters
        template <typename T>
        T DefaultValue() const
        {
            switch (type)
            {
                case ArgumentType::number:
                case ArgumentType::distance:
                    return defaultValue.number;
                case ArgumentType::money:
                    return defaultValue.money;
                case ArgumentType::rating:
                    return defaultValue.rating;
                default:
                    return 0;
            }
        }

        template<typename T>
        T MinimumValue() const
        {
            switch (type)
            {
                case ArgumentType::number:
                case ArgumentType::distance:
                    return minimumValue.number;
                case ArgumentType::money:
                    return minimumValue.money;
                case ArgumentType::rating:
                    return minimumValue.rating;
                default:
                    return 0;
            }
        }

        template<typename T>
        T MaximumValue() const
        {
            switch (type)
            {
                case ArgumentType::number:
                case ArgumentType::distance:
                    return maximumValue.number;
                case ArgumentType::money:
                    return maximumValue.money;
                case ArgumentType::rating:
                    return maximumValue.rating;
                default:
                    return 0;
            }
        }

        template<typename T>
        T ValueIncrement() const
        {
            switch (type)
            {
                case ArgumentType::number:
                case ArgumentType::distance:
                    return valueIncrement.number;
                case ArgumentType::money:
                    return valueIncrement.money;
                case ArgumentType::rating:
                    return valueIncrement.rating;
                default:
                    return 0;
            }
        }
        #pragma endregion

        bool operator==(const ArgumentDescriptor& other) const
        {
            return this->index == other.index;
        }
    };

    using GoalEvaluationFunc = ObjectiveStatus (*)(Park::ParkData& park, GameState_t& gameState, struct ObjectiveGoal goal);

    struct GoalDescriptor
    {
        uint8_t index{};
        // Whether the objective type is valid when money is disabled
        bool requiresMoney{};
        // Whether the objective type is valid for pay-per-entry price models
        bool requiresRideTickets{};
        // Function by which the objective is evaluated for completion/failure
        GoalEvaluationFunc evaluationFunction{};
        const ArgumentDescriptor* arguments[kMaxGoalArguments];
        // Must ALWAYS match the number of arguments
        uint8_t argCount;

        bool IsValidForSettings(bool useMoney, bool canAskMoneyForRides) const
        {
            return (useMoney || !requiresMoney) && (canAskMoneyForRides || !requiresRideTickets);
        }
    };

    struct ObjectiveDescriptor
    {
        StringId name{};
        StringId dropdownDesc{};
        uint8_t deadlineYear{};
        const GoalDescriptor* goals[kMaxObjectiveGoals];
    };
    #pragma endregion

    #pragma region ArgumentDescriptors
    constexpr ArgumentValue ArgNumber(uint16_t val)
    {
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

    constexpr ArgumentDescriptor kArgumentGuestCount = {
        .index = 0,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::number,
        .defaultValue = ArgNumber(kObjectiveGuestsDefault),
        .minimumValue = ArgNumber(kObjectiveGuestsMin),
        .maximumValue = ArgNumber(kObjectiveGuestsMax),
        .valueIncrement = ArgNumber(kObjectiveGuestsIncrement),
    };

    constexpr ArgumentDescriptor kArgumentParkRating = {
        .index = 1,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::number,
        .defaultValue = ArgMoney(kObjectiveParkRatingDefault),
        .minimumValue = ArgMoney(kObjectiveParkRatingMin),
        .maximumValue = ArgMoney(kObjectiveParkRatingMax),
        .valueIncrement = ArgMoney(kObjectiveParkRatingIncrement),
    };

    constexpr ArgumentDescriptor kArgumentSustainParkRating = {
        .index = 2, .isOptional = true, .allowClosingPark = false, .type = ArgumentType::boolean
    };

    constexpr ArgumentDescriptor kArgumentParkValue = {
        .index = 3,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::money,
        .defaultValue = ArgMoney(kObjectiveParkValueDefault),
        .minimumValue = ArgMoney(kObjectiveParkValueMin),
        .maximumValue = ArgMoney(kObjectiveParkValueMax),
        .valueIncrement = ArgMoney(kObjectiveParkValueIncrement),
    };

    constexpr ArgumentDescriptor kArgumentIncomeRides = {
        .index = 4,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::money,
        .defaultValue = ArgMoney(kObjectiveIncomeRidesDefault),
        .minimumValue = ArgMoney(kObjectiveIncomeMin),
        .maximumValue = ArgMoney(kObjectiveIncomeMax),
        .valueIncrement = ArgMoney(kObjectiveIncomeIncrement),
    };

    constexpr ArgumentDescriptor kArgumentIncomeShops = {
        .index = 5,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::money,
        .defaultValue = ArgMoney(kObjectiveIncomeShopsDefault),
        .minimumValue = ArgMoney(kObjectiveIncomeMin),
        .maximumValue = ArgMoney(kObjectiveIncomeMax),
        .valueIncrement = ArgMoney(kObjectiveIncomeIncrement),
    };

    constexpr ArgumentDescriptor kArgumentCoasterCount = {
        .index = 6,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::number,
        .defaultValue = ArgNumber(kObjectiveCoastersDefault),
        .minimumValue = ArgNumber(kObjectiveCoastersMin),
        .maximumValue = ArgNumber(kObjectiveCoastersMax),
        .valueIncrement = ArgNumber(kObjectiveCoastersIncrement),
    };

    constexpr ArgumentDescriptor kArgumentCoasterCompleteExisting = {
        .index = 7, .isOptional = true, .allowClosingPark = true, .type = ArgumentType::boolean
    };

    constexpr ArgumentDescriptor kArgumentCoasterExcitement = {
        .index = 8,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::rating,
        .defaultValue = ArgRating(kObjectiveExcitementDefault),
        .minimumValue = ArgRating(kObjectiveExcitementMin),
        .maximumValue = ArgRating(kObjectiveExcitementMax),
        .valueIncrement = ArgRating(kObjectiveExcitementIncrement),
    };

    constexpr ArgumentDescriptor kArgumentCoasterLength = {
        .index = 9,
        .isOptional = true,
        .allowClosingPark = true,
        .type = ArgumentType::distance,
        .defaultValue = ArgNumber(kObjectiveLengthDefault),
        .minimumValue = ArgNumber(kObjectiveLengthMin),
        .maximumValue = ArgNumber(kObjectiveLengthMax),
        .valueIncrement = ArgNumber(kObjectiveLengthIncrement),
    };

    constexpr ArgumentDescriptor kArgumentBuildTheBest = {
        .index = 10,
        .isOptional = false,
        .allowClosingPark = true,
        .type = ArgumentType::rideType,
    };

    // For proper serialization, index value and list position must match
    constexpr const ArgumentDescriptor* kArgumentList[] = {
        &kArgumentGuestCount,        &kArgumentParkRating,    &kArgumentSustainParkRating, &kArgumentParkValue,
        &kArgumentIncomeRides,       &kArgumentIncomeShops,   &kArgumentCoasterCount,      &kArgumentCoasterCompleteExisting,
        &kArgumentCoasterExcitement, &kArgumentCoasterLength, &kArgumentBuildTheBest,
    };

    // Map arguments to legacy type
    constexpr const ArgumentDescriptor* kArgumentByLegacyList[] = {
        nullptr,
        &kArgumentGuestCount,
        &kArgumentParkValue,
        nullptr,
        &kArgumentBuildTheBest,
        &kArgumentCoasterExcitement,
        &kArgumentGuestCount,
        &kArgumentIncomeRides,
        &kArgumentCoasterLength,
        &kArgumentCoasterExcitement,
        &kArgumentParkValue,
        &kArgumentIncomeShops,
        nullptr,
    };
    #pragma endregion

    #pragma region GoalDescriptors
    ObjectiveStatus EvaluateEmpty(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);
    ObjectiveStatus EvaluateGuests(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);
    ObjectiveStatus EvaluateParkRating(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);
    ObjectiveStatus EvaluateParkValue(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);
    ObjectiveStatus EvaluateLoan(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);
    ObjectiveStatus EvaluateIncomeRides(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);
    ObjectiveStatus EvaluateIncomeShops(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);
    ObjectiveStatus EvaluateRollerCoasters(Park::ParkData& park, GameState_t& gameState, ObjectiveGoal goal);

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
        &kGoalGuests,      &kGoalParkRating,  &kGoalParkValue, &kGoalRepayLoan,
        &kGoalIncomeRides, &kGoalIncomeShops, &kGoalCoasters,  &kGoalBuildTheBest,
    };
#pragma endregion

    #pragma region ScenarioObjectivePresets
    constexpr ObjectiveDescriptor kObjectivePresetGuestsBy = { .name = STR_OBJECTIVE_GUESTS_BY,
                                                                       .deadlineYear = kObjectiveYearDefault,
                                                                       .goals = { &kGoalGuests, &kGoalParkRating } };

    constexpr ObjectiveDescriptor kObjectivePresetParkValueBy = { .name = STR_OBJECTIVE_PARK_VALUE_BY,
                                                                          .deadlineYear = kObjectiveYearDefault,
                                                                          .goals = { &kGoalParkValue, &kGoalParkRating } };

    constexpr ObjectiveDescriptor kObjectivePresetHaveFun = { .name = STR_OBJECTIVE_HAVE_FUN,
                                                                      .deadlineYear = kObjectiveYearNoDeadline,
                                                                      .goals = {} };

    constexpr ObjectiveDescriptor kObjectivePresetBuildTheBest = { .name = STR_OBJECTIVE_BUILD_THE_BEST,
                                                                           .deadlineYear = kObjectiveYearNoDeadline,
                                                                           .goals = { &kGoalBuildTheBest } };

    constexpr ObjectiveDescriptor kObjectivePresetTenRollerCoasters = { .name = STR_OBJECTIVE_10_ROLLERCOASTERS,
                                                                                .deadlineYear = kObjectiveYearNoDeadline,
                                                                                .goals = { &kGoalCoasters } };

    constexpr ObjectiveDescriptor kObjectivePresetGuestsAndRating = { .name = STR_OBJECTIVE_GUESTS_AND_RATING,
                                                                              .deadlineYear = kObjectiveYearNoDeadline,
                                                                              .goals = { &kGoalGuests, &kGoalParkRating } };

    constexpr ObjectiveDescriptor kObjectivePresetMonthlyRideIncome = { .name = STR_OBJECTIVE_MONTHLY_RIDE_INCOME,
                                                                                .deadlineYear = kObjectiveYearNoDeadline,
                                                                                .goals = { &kGoalIncomeRides } };

    constexpr ObjectiveDescriptor kObjectivePresetTenRollerCoastersLength = {
        .name = STR_OBJECTIVE_10_ROLLERCOASTERS_LENGTH, .deadlineYear = kObjectiveYearNoDeadline, .goals = { &kGoalCoasters }
    };

    constexpr ObjectiveDescriptor kObjectivePresetCompleteFiveRollerCoasters = {
        .name = STR_OBJECTIVE_FINISH_5_ROLLERCOASTERS, .deadlineYear = kObjectiveYearNoDeadline, .goals = { &kGoalCoasters }
    };

    constexpr ObjectiveDescriptor kObjectivePresetRepayLoanAndParkValue = {
        .name = STR_OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE,
        .deadlineYear = kObjectiveYearNoDeadline,
        .goals = { &kGoalParkValue, &kGoalRepayLoan }
    };

    constexpr ObjectiveDescriptor kObjectivePresetMonthlyFoodIncome = { .name = STR_OBJECTIVE_MONTHLY_FOOD_INCOME,
                                                                                .deadlineYear = kObjectiveYearNoDeadline,
                                                                                .goals = { &kGoalIncomeShops } };

    constexpr const ObjectiveDescriptor* kObjectivePresets[] = {
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

    #pragma region Data Structure
    // Modifiable argument container for active scenario
    struct ObjectiveArgument
    {
        ArgumentDescriptor* descriptor;
        ArgumentValue value{};
        bool enabled{};

        bool Increment();
        bool Decrement();

        template<typename T>
        T GetValue() const
        {
            switch (descriptor->type)
            {
                case ArgumentType::number:
                case ArgumentType::distance:
                    return value.number;
                case ArgumentType::money:
                    return value.money;
                case ArgumentType::rating:
                    return value.rating;
                case ArgumentType::rideType:
                    return value.rideType;
                default:
                    return 0;
            }
        }

        template<typename T>
        void SetValue(T v)
        {
            switch (descriptor->type)
            {
                case ArgumentType::number:
                case ArgumentType::distance:
                    value.number = v;
                case ArgumentType::money:
                    value.money = v;
                case ArgumentType::rating:
                    value.rating = v;
                case ArgumentType::rideType:
                    value.rideType = v;
            }
        }
    };

    // Modifiable goal container for active scenario
    struct ObjectiveGoal
    {
        GoalDescriptor* descriptor;
        std::vector<ObjectiveArgument*> args;

        ObjectiveStatus Evaluate(Park::ParkData& park, GameState_t& gameState) const;

        #pragma region Argument Funcs
        ObjectiveArgument* GetArgument(const ArgumentDescriptor* desc) const
        {
            for (auto& arg : this->args)
            {
                if (arg->descriptor->index == desc->index && arg->enabled)
                {
                    return arg;
                }
            }
            return nullptr;
        }

        template<typename T>
        T GetArgumentValue(const ArgumentDescriptor* desc) const
        {
            auto* arg = GetArgument(desc);
            return (arg != nullptr) ? arg->GetValue<T>() : 0;
        }

        template<typename T>
        void SetArgumentValue(const ArgumentDescriptor* desc, T v) const
        {
            auto* arg = GetArgument(desc);
            if (arg != nullptr)
            {
                arg->SetValue<T>(v);
            }
        }

        bool IsArgumentEnabled(const ArgumentDescriptor* desc) const
        {
            auto* arg = GetArgument(desc);
            return (arg != nullptr) ? arg->enabled : 0;
        }
        #pragma endregion
    };

    // Objective container for active scenario
    struct Objective
    {
    public:
        // Time limit for the objective, in years. 0 = no time limit.
        uint8_t deadlineYear;
        std::vector<ObjectiveGoal*> goals;
        StringId format;
        // Whether the objective description text window should open on loading a save.
        bool displayOnLoad;

        ObjectiveStatus ScenarioEvaluateObjective(Park::ParkData& park, GameState_t& gameState) const;

        bool IncrementDeadlineYear()
        {
            uint8_t expectedResult = this->deadlineYear + kObjectiveYearIncrement;
            this->deadlineYear = std::min(expectedResult, kObjectiveYearMax);
            return this->deadlineYear == expectedResult;
        }

        bool DecrementDeadlineYear()
        {
            uint8_t expectedResult = this->deadlineYear - kObjectiveYearIncrement;
            this->deadlineYear = std::max(expectedResult, kObjectiveYearMin);
            return this->deadlineYear == expectedResult;
        }

        #pragma region Argument Funcs
        ObjectiveArgument* GetArgument(const ArgumentDescriptor* descriptor) const
        {
            for (auto& goal : this->goals)
            {
                auto* result = goal->GetArgument(descriptor);
                if (result != nullptr)
                {
                    return result;
                }
            }
            return nullptr;
        }

        ObjectiveArgument* GetArgument(LegacyObjectiveType type) const
        {
            return GetArgument(kArgumentByLegacyList[EnumValue(type)]);
        }

        template<typename T>
        T GetArgumentValue(const ArgumentDescriptor* descriptor) const
        {
            auto* arg = GetArgument(descriptor);
            return (arg != nullptr) ? arg->GetValue<T>() : 0;
        }

        template<typename T>
        void SetArgumentValue(const ArgumentDescriptor* descriptor, T v) const
        {
            auto* arg = GetArgument(descriptor);
            if (arg != nullptr)
            {
                arg->SetValue<T>(v);
            }
        }

        template<typename T>
        T GetArgumentValue(LegacyObjectiveType type) const
        {
            auto* arg = GetArgument(type);
            return (arg != nullptr) ? arg->GetValue<T>() : 0;
        }

        template<typename T>
        void SetArgumentValue(LegacyObjectiveType type, T v) const
        {
            auto* arg = GetArgument(type);
            if (arg != nullptr)
            {
                arg->SetValue<T>(v);
            }
        }

        void EnableArgument(const ArgumentDescriptor* descriptor)
        {
            auto* arg = GetArgument(descriptor);
            if (arg != nullptr)
            {
                arg->enabled = true;
            }
        }

        void DisableArgument(const ArgumentDescriptor* descriptor)
        {
            auto* arg = GetArgument(descriptor);
            if (arg != nullptr)
            {
                arg->enabled = false;
            }
        }

        bool IsArgumentEnabled(const ArgumentDescriptor* descriptor) const
        {
            auto* arg = GetArgument(descriptor);
            return (arg != nullptr && arg->enabled);
        }

        void EnableArgument(LegacyObjectiveType type)
        {
            auto* arg = GetArgument(type);
            if (arg != nullptr)
            {
                arg->enabled = true;
            }
        }

        void DisableArgument(LegacyObjectiveType type)
        {
            auto* arg = GetArgument(type);
            if (arg != nullptr)
            {
                arg->enabled = false;
            }
        }

        bool IsArgumentEnabled(LegacyObjectiveType type) const
        {
            auto* arg = GetArgument(type);
            return (arg != nullptr && arg->enabled);
        }
        #pragma endregion

        size_t GetGoalCount() const
        {
            return goals.size();
        }

        bool AllowsClosingPark() const;

        bool IsValidForSettings(bool useMoney, bool canAskMoneyForRides) const
        {
            for (const auto& goal : goals)
            {
                if (!goal->descriptor->IsValidForSettings(useMoney, canAskMoneyForRides))
                {
                    return false;
                }
            }
            return true;
        }
    };
    #pragma endregion

    Objective* ScenarioObjectiveInitFromPreset(const ObjectiveDescriptor& preset);
    Objective* ScenarioObjectiveInitFromLegacyType(
        const LegacyObjectiveType type, uint8_t arg1, int64_t arg2, uint16_t arg3);
    Objective* ScenarioObjectiveInitFromLegacyType(
        const LegacyObjectiveType type);
} // namespace OpenRCT2::Scenario
