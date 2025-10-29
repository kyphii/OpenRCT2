/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
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

    static constexpr uint16_t kObjectiveRatingDefault   = 600;
    static constexpr uint16_t kObjectiveRatingMin       = 250;
    static constexpr uint16_t kObjectiveRatingMax       = 950;
    static constexpr uint16_t kObjectiveRatingIncrement = 50;

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

    static constexpr uint8_t kObjectiveYearDefault   = 3;
    static constexpr uint8_t kObjectiveYearMin       = 1;
    static constexpr uint8_t kObjectiveYearMax       = 25;
    static constexpr uint8_t kObjectiveYearIncrement = 1;

    enum class ObjectiveStatus : uint8_t
    {
        Undecided,
        Success,
        Failure,
    };

    enum GoalModifierType : uint8_t
    {
        boolean,
        number,
        money,
        rating,
        distance,
    };

    union ModifierValue
    {
        uint32_t number;
        money64 money;
        // Used by "Build Coasters" type goals
        RideRating_t rating;
    };

    struct GoalModifierDescriptor
    {
        uint8_t index;
        bool isOptional;
        GoalModifierType type;
        // Following are determined by the goal type
        ModifierValue defaultValue;
        ModifierValue minimumValue;
        ModifierValue maximumValue;
        ModifierValue valueIncrement;
    };

    using GoalEvaluationFunc = ObjectiveStatus (*)(Park::ParkData&, GameState_t&, ScenarioGoal&);

    struct GoalDescriptor
    {
        uint8_t index{};
        // Whether the objective type is valid when money is disabled
        bool requiresMoney{};
        // Whether the objective type is valid for pay-per-entry price models
        bool requiresRideTickets{};
        // Function by which the objective is evaluated for completion/failure
        GoalEvaluationFunc evaluationFunction{};
        GoalModifierDescriptor modifiers[];

        bool IsValidForSettings(bool useMoney, bool canAskMoneyForRides) const
        {
            return (useMoney || !requiresMoney) && (canAskMoneyForRides || !requiresRideTickets);
        }

        ObjectiveStatus GoalEvaluateGuests(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal);
        ObjectiveStatus GoalEvaluateParkRating(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal);
        ObjectiveStatus GoalEvaluateParkValue(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal);
        ObjectiveStatus GoalEvaluateLoan(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal);
        ObjectiveStatus GoalEvaluateIncomeRides(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal);
        ObjectiveStatus GoalEvaluateIncomeShops(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal);
        ObjectiveStatus GoalEvaluateRollerCoasters(Park::ParkData& park, GameState_t& gameState, const ScenarioGoal& goal);
    };

    // Modifiable modifier container for active scenario
    struct ScenarioGoalModifier
    {
        GoalModifierDescriptor* descriptor;
        ModifierValue value;
        bool enabled;
    };

    // Modifiable goal container for active scenario
    struct ScenarioGoal
    {
        GoalDescriptor* descriptor;
        std::vector<ScenarioGoalModifier> values;

        ObjectiveStatus Evaluate(Park::ParkData& park, GameState_t& gameState) const;
        bool GetModifierEnabled(size_t listIndex) const;
        uint16_t GetModifierValueNumber(size_t listIndex) const;
        money64 GetModifierValueMoney(size_t listIndex) const;
        RideRating_t GetModifierValueRating(size_t listIndex) const;
        uint16_t GetModifierValueDistance(size_t listIndex) const;
    };

    class ScenarioObjective
    {
    public:
        // Time limit for the objective, in years. 0 = no time limit.
        uint8_t deadlineYear;
        std::vector<ScenarioGoal> goals;

        ObjectiveStatus ScenarioEvaluateObjective(Park::ParkData& park, GameState_t& gameState) const;
    };

    #pragma region ModifierDescriptors
    constexpr GoalModifierDescriptor modifierGuestCount = {
        .index = 0,
        .isOptional = false,
        .type = GoalModifierType::number,
        .defaultValue =   kObjectiveGuestsDefault,
        .minimumValue =   kObjectiveGuestsMin,
        .maximumValue =   kObjectiveGuestsMax,
        .valueIncrement = kObjectiveGuestsIncrement, 
    };

    constexpr GoalModifierDescriptor modifierParkRating = {
        .index = 1,
        .isOptional = false,
        .type = GoalModifierType::number,
        .defaultValue =   kObjectiveRatingDefault,
        .minimumValue =   kObjectiveRatingMin,
        .maximumValue =   kObjectiveRatingMax,
        .valueIncrement = kObjectiveRatingIncrement,
    };

    constexpr GoalModifierDescriptor modifierSustainParkRating = {
        .index = 2,
        .isOptional = true,
        .type = GoalModifierType::boolean
    };

    constexpr GoalModifierDescriptor modifierParkValue = {
        .index = 3,
        .isOptional = false,
        .type = GoalModifierType::money,
        .defaultValue =   kObjectiveParkValueDefault,
        .minimumValue =   kObjectiveParkValueMin,
        .maximumValue =   kObjectiveParkValueMax,
        .valueIncrement = kObjectiveParkValueIncrement,
    };

    constexpr GoalModifierDescriptor modifierIncomeRides = {
        .index = 4,
        .isOptional = false,
        .type = GoalModifierType::money,
        .defaultValue =   kObjectiveIncomeRidesDefault,
        .minimumValue =   kObjectiveIncomeMin,
        .maximumValue =   kObjectiveIncomeMax,
        .valueIncrement = kObjectiveIncomeIncrement,
    };

    constexpr GoalModifierDescriptor modifierIncomeShops = {
        .index = 5,
        .isOptional = false,
        .type = GoalModifierType::money,
        .defaultValue =   kObjectiveIncomeShopsDefault,
        .minimumValue =   kObjectiveIncomeMin,
        .maximumValue =   kObjectiveIncomeMax,
        .valueIncrement = kObjectiveIncomeIncrement,
    };

    constexpr GoalModifierDescriptor modifierCoasterCount = {
        .index = 6,
        .isOptional = false,
        .type = GoalModifierType::number,
        .defaultValue =   kObjectiveCoastersDefault,
        .minimumValue =   kObjectiveCoastersMin,
        .maximumValue =   kObjectiveCoastersMax,
        .valueIncrement = kObjectiveCoastersIncrement,
    };

    constexpr GoalModifierDescriptor modifierCoasterCompleteExisting = {
        .index = 7,
        .isOptional = true,
        .type = GoalModifierType::boolean
    };

    constexpr GoalModifierDescriptor modifierCoasterExcitement = {
        .index = 8,
        .isOptional = false,
        .type = GoalModifierType::rating,
        .defaultValue =   kObjectiveExcitementDefault,
        .minimumValue =   kObjectiveExcitementMin,
        .maximumValue =   kObjectiveExcitementMax,
        .valueIncrement = kObjectiveExcitementIncrement,
    };

    constexpr GoalModifierDescriptor modifierCoasterLength = {
        .index = 9,
        .isOptional = true,
        .type = GoalModifierType::distance,
        .defaultValue =   kObjectiveLengthDefault,
        .minimumValue =   kObjectiveLengthMin,
        .maximumValue =   kObjectiveLengthMax,
        .valueIncrement = kObjectiveLengthIncrement,
    };
    #pragma endregion

    #pragma region GoalDescriptors
    constexpr GoalDescriptor goalGuests = {
        .index = 0,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = GoalDescriptor::GoalEvaluateGuests,
        .modifiers = { modifierGuestCount },
    };

    constexpr GoalDescriptor goalParkRating = {
        .index = 1,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = GoalDescriptor::GoalEvaluateParkRating,
        .modifiers = { modifierParkRating, modifierSustainParkRating},
    };

    constexpr GoalDescriptor goalParkValue = {
        .index = 2,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = GoalDescriptor::GoalEvaluateParkValue,
        .modifiers = { modifierParkValue },
    };

    constexpr GoalDescriptor goalRepayLoan = {
        .index = 3,
        .requiresMoney = true,
        .requiresRideTickets = false,
        .evaluationFunction = GoalDescriptor::GoalEvaluateLoan,
        .modifiers = { },
    };

    constexpr GoalDescriptor goalIncomeRides = {
        .index = 4,
        .requiresMoney = true,
        .requiresRideTickets = true,
        .evaluationFunction = GoalDescriptor::GoalEvaluateIncomeRides,
        .modifiers = { modifierIncomeRides },
    };

    constexpr GoalDescriptor goalIncomeShops = {
        .index = 5,
        .requiresMoney = true,
        .requiresRideTickets = false,
        .evaluationFunction = GoalDescriptor::GoalEvaluateIncomeShops,
        .modifiers = { modifierIncomeShops },
    };

    constexpr GoalDescriptor goalCoasters = {
        .index = 6,
        .requiresMoney = false,
        .requiresRideTickets = false,
        .evaluationFunction = GoalDescriptor::GoalEvaluateRollerCoasters,
        .modifiers = { modifierCoasterCount, modifierCoasterExcitement, modifierCoasterLength, modifierCoasterCompleteExisting },
    };
    #pragma endregion
} // namespace OpenRCT2::Scenario
