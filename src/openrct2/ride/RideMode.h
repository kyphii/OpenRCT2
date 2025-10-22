/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../core/Money.hpp"
#include "../localisation/StringIds.h"
#include "RideRatings.h"
#include "Vehicle.h"

#include <vector>

namespace OpenRCT2
{
    static const enum class RideModeID : uint8_t {
        normal,
        continuousCircuit,
        reverseInclineLaunchedShuttle,
        poweredLaunchPassthrough,
        shuttle,
        boatHire,
        upwardLaunch,
        rotatingLift,
        stationToStation,
        singleRidePerAdmission,
        unlimitedRidesPerAdmission,
        maze,
        race,
        dodgems,
        swing,
        shopStall,
        rotation,
        forwardRotation,
        backwardRotation,
        filmAvengingAviators,
        filmMouseTails,
        spaceRings,
        beginners,
        limPoweredLaunch,
        filmThrillRiders,
        filmStormChasers,
        filmSpaceRaiders,
        intense,
        berserk,
        hauntedHouse,
        circusShow,
        downwardLaunch,
        crookedHouse,
        freefallDrop,
        continuousCircuitBlockSectioned,
        poweredLaunch,
        poweredLaunchBlockSectioned,
        count,
    };

    enum : uint8_t
    {
        rideModeFlagNone = 0,
        rideModeFlagIsBlockSectioned = (1 << 0),
        rideModeFlagIsPoweredLaunch = (1 << 1),
        rideModeFlagIsFerrisWheel = (1 << 2),
        rideModeFlagSingleTrainOnly = (1 << 3),
        rideModeFlagAllowMultipleCircuits = (1 << 4),
    };

    using StationUpdateFunction = void (*)(Ride&, StationIndex);
    using VehicleUpdateFunction = void (*)(Vehicle*, Ride*);
    using VehicleCarEntryUpdateFunction = void (*)(Vehicle*, Ride*, CarEntry);

    // Number of stations supported by the mode
    enum : uint8_t
    {
        rideModeStationTypeAny,
        rideModeStationTypeMaxOne,
        rideModeStationTypeMinTwo,
    };

    // Window UI Mode
    enum : uint8_t
    {
        rideModeUIFormatNormal,
        rideModeUIFormatLaunch,
        rideModeUIFormatSwing,
        rideModeUIFormatRotation,
        rideModeUIFormatDodgems,
        rideModeUIFormatRace,
        rideModeUIFormatChairlift,
    };

    struct RideMode
    {
        RideModeID Index{};
        StringId Name{};
        RideRating::Tuple StatBonus{};
        money64 UpkeepCost{};
        uint64_t Flags{};
        uint8_t StationType{};
        // Used by Top Spin, Motion Sim, & 3D Cinema
        uint8_t ModeSubstate{};
        StationUpdateFunction FuncUpdateStation{};
        VehicleUpdateFunction FuncUpdateMovingToEndOfStation{};
        VehicleUpdateFunction FuncUpdateWaitingToDepart{};
        VehicleCarEntryUpdateFunction FuncUpdateDeparting{};
        VehicleUpdateFunction FuncUpdateArriving{};
        bool TweakUITextInput{};
        uint8_t UIOptionFormat{};
        StringId ErrorMessage{};

        friend bool operator==(const RideMode a, const RideMode b)
        {
            return &a == &b || a.Index == b.Index;
        }

        bool HasFlag(uint8_t flag) const
        {
            return Flags & flag;
        }
    };

    namespace RideModes
    {
        constexpr RideRating::Tuple kStatBonusNone = RideRating::makeTuple(0, 0, 0, 0, 0, 0);

        #pragma region Mode Definitions
        // clang-format off
        constexpr RideMode kNormal = {
            .Index = RideModeID::normal,
            .Name = STR_RIDE_MODE_NORMAL, 
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kContinuousCircuit = {
            .Index = RideModeID::continuousCircuit,
            .Name = STR_RIDE_MODE_CONTINUOUS_CIRCUIT,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagAllowMultipleCircuits,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingCircuit,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage =  0
        };
        constexpr RideMode kReverseInclineLaunchedShuttle = {
            .Index = RideModeID::reverseInclineLaunchedShuttle,
            .Name = STR_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 30,
            .Flags = rideModeFlagSingleTrainOnly | rideModeFlagAllowMultipleCircuits,
            .StationType = rideModeStationTypeMaxOne,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingReverseInclineShuttle,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage =  0
        };
        constexpr RideMode kPoweredLaunchPassthrough = {
            .Index = RideModeID::poweredLaunchPassthrough,
            .Name = STR_RIDE_MODE_POWERED_LAUNCH_PASSTROUGH,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 160,
            .Flags = rideModeFlagIsPoweredLaunch | rideModeFlagSingleTrainOnly | rideModeFlagAllowMultipleCircuits,
            .StationType = rideModeStationTypeMaxOne,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingLaunch,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = false,
            .UIOptionFormat = rideModeUIFormatLaunch,
            .ErrorMessage = 0
        };
        constexpr RideMode kShuttle = {
            .Index = RideModeID::shuttle,
            .Name =  STR_RIDE_MODE_SHUTTLE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagSingleTrainOnly,
            .StationType = rideModeStationTypeMinTwo,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kBoatHire = {
            .Index = RideModeID::boatHire,
            .Name =  STR_RIDE_MODE_BOAT_HIRE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingCircuit,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kUpwardLaunch = {
            .Index = RideModeID::upwardLaunch,
            .Name =  STR_RIDE_MODE_UPWARD_LAUNCH,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingLaunch,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = false,
            .UIOptionFormat = rideModeUIFormatLaunch,
            .ErrorMessage = 0
        };
        constexpr RideMode kRotatingLift = {
            .Index = RideModeID::rotatingLift,
            .Name =  STR_RIDE_MODE_ROTATING_LIFT,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingCircuit,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kStationToStation = {
            .Index = RideModeID::stationToStation,
            .Name =  STR_RIDE_MODE_STATION_TO_STATION,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = false,
            .UIOptionFormat = rideModeUIFormatChairlift,
            .ErrorMessage = STR_CANT_CHANGE_SPEED
        };
        constexpr RideMode kSingleRidePerAdmission = {
            .Index = RideModeID::singleRidePerAdmission,
            .Name =  STR_RIDE_MODE_SINGLE_RIDE_PER_ADMISSION,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kUnlimitedRidesPerAdmission = {
            .Index = RideModeID::unlimitedRidesPerAdmission,
            .Name = STR_RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kMaze = {
            .Index = RideModeID::maze, 
            .Name = STR_RIDE_MODE_MAZE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kRace = {
            .Index = RideModeID::race, 
            .Name = STR_RIDE_MODE_RACE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationRace,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatRace,
            .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_LAPS
        };
        constexpr RideMode kDodgems = {
            .Index = RideModeID::dodgems, 
            .Name = STR_RIDE_MODE_DODGEMS,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationDodgems,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartDodgems,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = false,
            .UIOptionFormat = rideModeUIFormatDodgems,
            .ErrorMessage = STR_CANT_CHANGE_TIME_LIMIT
        };
        constexpr RideMode kSwing = {
            .Index = RideModeID::swing, 
            .Name = STR_RIDE_MODE_SWING,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartSwing,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatSwing,
            .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_SWINGS
        };
        constexpr RideMode kShopStall = {
            .Index = RideModeID::shopStall, 
            .Name = STR_RIDE_MODE_SHOP_STALL,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kRotation = {
            .Index = RideModeID::rotation, 
            .Name = STR_RIDE_MODE_ROTATION,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartRotation,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatRotation,
            .ErrorMessage = 0
        };
        constexpr RideMode kForwardRotation = {
            .Index = RideModeID::forwardRotation, 
            .Name = STR_RIDE_MODE_FORWARD_ROTATION,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagIsFerrisWheel,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartFerrisWheel,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatRotation,
            .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_ROTATIONS
        };
        constexpr RideMode kBackwardRotation = {
            .Index = RideModeID::backwardRotation, 
            .Name = STR_RIDE_MODE_BACKWARD_ROTATION,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagIsFerrisWheel,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartFerrisWheel,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatRotation,
            .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_ROTATIONS
        };
        constexpr RideMode kFilmAvengingAviators = {
            .Index = RideModeID::filmAvengingAviators, 
            .Name = STR_RIDE_MODE_FILM_AVENGING_AVIATORS,
            .StatBonus = RideRating::makeTuple(2, 90, 3, 50, 3, 00),
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartMotionSim,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kMouseTails3DFilm = {
            .Index = RideModeID::filmMouseTails, 
            .Name = STR_RIDE_MODE_3D_FILM_MOUSE_TAILS,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepart3DCinema,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kSpaceRings = {
            .Index = RideModeID::spaceRings, 
            .Name = STR_RIDE_MODE_SPACE_RINGS,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartSpaceRings,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kBeginners = {
            .Index = RideModeID::beginners, 
            .Name = STR_RIDE_MODE_BEGINNERS,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartTopSpin,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .ErrorMessage = 0
        };
        constexpr RideMode kLimPoweredLaunch = {
            .Index = RideModeID::limPoweredLaunch, 
            .Name = STR_RIDE_MODE_LIM_POWERED_LAUNCH,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 320,
            .Flags = rideModeFlagSingleTrainOnly,
            .StationType = rideModeStationTypeMaxOne,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingLaunch,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kFilmThrillRiders = {
            .Index = RideModeID::filmThrillRiders, 
            .Name = STR_RIDE_MODE_FILM_THRILL_RIDERS,
            .StatBonus = RideRating::makeTuple(3, 25, 4, 10, 3, 30),
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 1,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartMotionSim,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kStormChasers3DFilm = {
            .Index = RideModeID::filmStormChasers, 
            .Name = STR_RIDE_MODE_3D_FILM_STORM_CHASERS,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 1,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepart3DCinema,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kSpaceRaiders3DFilm = {
            .Index = RideModeID::filmSpaceRaiders, 
            .Name = STR_RIDE_MODE_3D_FILM_SPACE_RAIDERS,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 2,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepart3DCinema,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kIntense = {
            .Index = RideModeID::intense, 
            .Name = STR_RIDE_MODE_INTENSE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 1,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartTopSpin,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kBerserk = {
            .Index = RideModeID::berserk, 
            .Name = STR_RIDE_MODE_BERSERK,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 2,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartTopSpin,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kHauntedHouse = {
            .Index = RideModeID::hauntedHouse, 
            .Name = STR_RIDE_MODE_HAUNTED_HOUSE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartHauntedHouse,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kCircus = {
            .Index = RideModeID::circusShow, 
            .Name = STR_RIDE_MODE_CIRCUS_SHOW,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartCircus,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kDownwardLaunch = {
            .Index = RideModeID::downwardLaunch, 
            .Name = STR_RIDE_MODE_DOWNWARD_LAUNCH,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingDownwardLaunch,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kCrookedHouse = {
            .Index = RideModeID::crookedHouse, 
            .Name = STR_RIDE_MODE_CROOKED_HOUSE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartCrookedHouse,
            .FuncUpdateArriving = UpdateArrivingFlatRide,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kFreefallDrop = {
            .Index = RideModeID::freefallDrop, 
            .Name = STR_RIDE_MODE_FREEFALL_DROP,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagNone,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingCircuit,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kContinuousCircuitBlockSectioned = {
            .Index = RideModeID::continuousCircuitBlockSectioned, 
            .Name = STR_RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 0,
            .Flags = rideModeFlagIsBlockSectioned,
            .StationType = rideModeStationTypeAny,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationBlockSection,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingCircuit,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = true,
            .UIOptionFormat = rideModeUIFormatNormal,
            .ErrorMessage = 0
        };
        constexpr RideMode kPoweredLaunch = {
            .Index = RideModeID::poweredLaunch, 
            .Name = STR_RIDE_MODE_POWERED_LAUNCH,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 220,
            .Flags = rideModeFlagIsPoweredLaunch | rideModeFlagSingleTrainOnly,
            .StationType = rideModeStationTypeMaxOne,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationNormal,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingLaunch,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = false,
            .UIOptionFormat = rideModeUIFormatLaunch,
            .ErrorMessage = 0
        };
        constexpr RideMode kPoweredLaunchBlockSectioned = {
            .Index = RideModeID::poweredLaunchBlockSectioned, 
            .Name = STR_RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED_MODE,
            .StatBonus = kStatBonusNone,
            .UpkeepCost = 220,
            .Flags = rideModeFlagIsBlockSectioned | rideModeFlagIsPoweredLaunch,
            .StationType = rideModeStationTypeMaxOne,
            .ModeSubstate = 0,
            .FuncUpdateStation = RideUpdateStationBlockSection,
            .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
            .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
            .FuncUpdateDeparting = UpdateDepartingLaunch,
            .FuncUpdateArriving = UpdateArrivingNormal,
            .TweakUITextInput = false,
            .UIOptionFormat = rideModeUIFormatLaunch,
            .ErrorMessage = 0
        };
        #pragma endregion

        constexpr RideMode kOperatingModes[static_cast<uint8_t>(RideModeID::count)] = {
            kNormal,
            kContinuousCircuit,
            kReverseInclineLaunchedShuttle,
            kPoweredLaunchPassthrough,
            kShuttle,
            kBoatHire,
            kUpwardLaunch,
            kRotatingLift,
            kStationToStation,
            kSingleRidePerAdmission,
            kUnlimitedRidesPerAdmission,
            kMaze,
            kRace,
            kDodgems,
            kSwing,
            kShopStall,
            kRotation,
            kForwardRotation,
            kBackwardRotation,
            kFilmAvengingAviators,
            kMouseTails3DFilm,
            kSpaceRings,
            kBeginners,
            kLimPoweredLaunch,
            kFilmThrillRiders,
            kStormChasers3DFilm,
            kSpaceRaiders3DFilm,
            kIntense,
            kBerserk,
            kHauntedHouse,
            kCircus,
            kDownwardLaunch,
            kCrookedHouse,
            kFreefallDrop,
            kContinuousCircuitBlockSectioned,
            kPoweredLaunch,
            kPoweredLaunchBlockSectioned,
        };
        // clang-format on

        RideMode FromID(RideModeID index);

        RideMode FromIndex(uint8_t index);

        uint8_t ToIndex(RideMode mode);

        uint64_t ToFlags(std::vector<RideMode> modes);

        constexpr uint64_t IndexToFlags(std::vector<RideModeID> modeID)
        {
            uint64_t result = 0;
            for (RideModeID m : modeID)
            {
                result |= (uint64_t(1) << static_cast<uint8_t>(m));
            }
            return result;
        }

        RideMode GetBlockSectionedCounterpart(RideMode originalMode);

        RideMode GetNonBlockSectionedCounterpart(RideMode originalMode, RideModeID defaultMode);

    } // namespace RideModes
} // namespace OpenRCT2
