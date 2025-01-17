#include "SitRepEntry.h"

#include "i18n.h"
#include "Logger.h"
#include "../universe/Building.h"
#include "../universe/Planet.h"
#include "../universe/System.h"
#include "../universe/Ship.h"
#include "../universe/Fleet.h"
#include "../universe/Universe.h"

#include <boost/lexical_cast.hpp>

SitRepEntry::SitRepEntry() :
    VarText(),
    m_turn(INVALID_GAME_TURN),
    m_icon("/icons/sitrep/generic.png")
{}

SitRepEntry::SitRepEntry(const char* template_string, int turn, const char* icon,
                         const char* label, bool stringtable_lookup) :
    SitRepEntry(std::string(template_string), turn, std::string(icon),
                std::string(label), stringtable_lookup)
{}

SitRepEntry::SitRepEntry(std::string&& template_string, int turn,
                         std::string&& icon, std::string&& label,
                         bool stringtable_lookup) :
    VarText(std::move(template_string), stringtable_lookup),
    m_turn(turn),
    m_icon(icon.empty() ? "/icons/sitrep/generic.png" : std::move(icon)),
    m_label(std::move(label))
{}

int SitRepEntry::GetDataIDNumber(const std::string& tag) const {
    auto elem = m_variables.find(tag);
    try {
        if (elem != m_variables.end())
            return boost::lexical_cast<int>(elem->second);
    } catch (...) {
        return -1;
    }
    return -1;
}

const std::string& SitRepEntry::GetDataString(const std::string& tag) const {
    static const std::string EMPTY_STRING;
    auto elem = m_variables.find(tag);
    if (elem == m_variables.end())
        return EMPTY_STRING;
    return elem->second;
}

std::string SitRepEntry::Dump() const {
    std::string retval = "SitRep template_string = \"" + m_template_string + "\"";
    for (const auto& variable : m_variables)
        retval += " " + variable.first + " = " + variable.second;
    retval += " turn = " + std::to_string(m_turn);
    retval += " icon = " + m_icon;
    retval += " label = " + m_label;
    return retval;
}

SitRepEntry CreateTechResearchedSitRep(const std::string& tech_name, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_TECH_RESEARCHED"),
        current_turn,
        "icons/sitrep/tech_researched.png",
        UserStringNop("SITREP_TECH_RESEARCHED_LABEL"), true);
    sitrep.AddVariable(VarText::TECH_TAG, tech_name);
    return sitrep;
}

SitRepEntry CreateShipBuiltSitRep(int ship_id, int system_id, int shipdesign_id, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_SHIP_BUILT"),
        current_turn + 1,
        "icons/sitrep/ship_produced.png",
        UserStringNop("SITREP_SHIP_BUILT_LABEL"), true);
    sitrep.AddVariable(VarText::SYSTEM_ID_TAG, std::to_string(system_id));
    sitrep.AddVariable(VarText::SHIP_ID_TAG,   std::to_string(ship_id));
    sitrep.AddVariable(VarText::DESIGN_ID_TAG, std::to_string(shipdesign_id));
    return sitrep;
}

SitRepEntry CreateShipBlockBuiltSitRep(int system_id, int shipdesign_id, int number, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_SHIP_BATCH_BUILT"),
        current_turn + 1,
        "icons/sitrep/ship_produced.png",
        UserStringNop("SITREP_SHIP_BATCH_BUILT_LABEL"), true);
    sitrep.AddVariable(VarText::SYSTEM_ID_TAG, std::to_string(system_id));
    sitrep.AddVariable(VarText::DESIGN_ID_TAG, std::to_string(shipdesign_id));
    sitrep.AddVariable(VarText::RAW_TEXT_TAG,  std::to_string(number));
    return sitrep;
}

SitRepEntry CreateBuildingBuiltSitRep(int building_id, int planet_id, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_BUILDING_BUILT"),
        current_turn + 1,
        "icons/sitrep/building_produced.png",
        UserStringNop("SITREP_BUILDING_BUILT_LABEL"), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,   std::to_string(planet_id));
    sitrep.AddVariable(VarText::BUILDING_ID_TAG, std::to_string(building_id));
    return sitrep;
}

SitRepEntry CreateTechUnlockedSitRep(const std::string& tech_name, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_TECH_UNLOCKED"),
        current_turn,
        "icons/sitrep/tech_unlocked.png",
        UserStringNop("SITREP_TECH_UNLOCKED_LABEL"), true);
    sitrep.AddVariable(VarText::TECH_TAG, tech_name);
    return sitrep;
}

SitRepEntry CreatePolicyUnlockedSitRep(const std::string& policy_name, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_POLICY_UNLOCKED"),
        current_turn + 1,
        "icons/sitrep/policy_unlocked.png",
        UserStringNop("SITREP_POLICY_UNLOCKED_LABEL"), true);
    sitrep.AddVariable(VarText::POLICY_TAG, policy_name);
    return sitrep;
}

SitRepEntry CreateBuildingTypeUnlockedSitRep(const std::string& building_type_name, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_BUILDING_TYPE_UNLOCKED"),
        current_turn,
        "icons/sitrep/building_type_unlocked.png",
        UserStringNop("SITREP_BUILDING_TYPE_UNLOCKED_LABEL"), true);
    sitrep.AddVariable(VarText::BUILDING_TYPE_TAG, building_type_name);
    return sitrep;
}

SitRepEntry CreateShipHullUnlockedSitRep(const std::string& ship_hull_name, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_SHIP_HULL_UNLOCKED"),
        current_turn,
        "icons/sitrep/ship_hull_unlocked.png",
        UserStringNop("SITREP_SHIP_HULL_UNLOCKED_LABEL"), true);
    sitrep.AddVariable(VarText::SHIP_HULL_TAG, ship_hull_name);
    return sitrep;
}

SitRepEntry CreateShipPartUnlockedSitRep(const std::string& ship_part_name, int current_turn) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_SHIP_PART_UNLOCKED"),
        current_turn,
        "icons/sitrep/ship_part_unlocked.png",
        UserStringNop("SITREP_SHIP_PART_UNLOCKED_LABEL"), true);
    sitrep.AddVariable(VarText::SHIP_PART_TAG, ship_part_name);
    return sitrep;
}

SitRepEntry CreateCombatSitRep(int system_id, int log_id, int enemy_id) {
    std::string template_string = (enemy_id == ALL_EMPIRES)
        ? UserStringNop("SITREP_COMBAT_SYSTEM")
        : UserStringNop("SITREP_COMBAT_SYSTEM_ENEMY");
    std::string label_string = (enemy_id == ALL_EMPIRES)
        ? UserStringNop("SITREP_COMBAT_SYSTEM_LABEL")
        : UserStringNop("SITREP_COMBAT_SYSTEM_ENEMY_LABEL");
    SitRepEntry sitrep(
        std::move(template_string), CurrentTurn() + 1,
        "icons/sitrep/combat.png", std::move(label_string), true);
    sitrep.AddVariable(VarText::SYSTEM_ID_TAG, std::to_string(system_id));
    sitrep.AddVariable(VarText::COMBAT_ID_TAG, std::to_string(log_id));
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG, std::to_string(enemy_id));
    return sitrep;
}

SitRepEntry CreateGroundCombatSitRep(int planet_id, int enemy_id) {
    std::string template_string = (enemy_id == ALL_EMPIRES)
        ? UserStringNop("SITREP_GROUND_BATTLE")
        : UserStringNop("SITREP_GROUND_BATTLE_ENEMY");
    std::string label_string = (enemy_id == ALL_EMPIRES)
        ? UserStringNop("SITREP_GROUND_BATTLE_LABEL")
        : UserStringNop("SITREP_GROUND_BATTLE_ENEMY_LABEL");
    SitRepEntry sitrep(
        std::move(template_string), CurrentTurn() + 1,
        "icons/sitrep/ground_combat.png", std::move(label_string), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,     std::to_string(planet_id));
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG,     std::to_string(enemy_id));
    return sitrep;
}

SitRepEntry CreatePlanetCapturedSitRep(int planet_id, int empire_id) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_PLANET_CAPTURED"),
        CurrentTurn() + 1,
        "icons/sitrep/planet_captured.png",
        UserStringNop("SITREP_PLANET_CAPTURED_LABEL"), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,     std::to_string(planet_id));
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG,     std::to_string(empire_id));
    return sitrep;
}

SitRepEntry CreatePlanetRebelledSitRep(int planet_id, int empire_id) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_PLANET_CAPTURED_NEUTRALS"),
        CurrentTurn() + 1,
        "icons/sitrep/planet_captured.png",
        UserStringNop("SITREP_PLANET_CAPTURED_NEUTRALS_LABEL"), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,     std::to_string(planet_id));
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG,     std::to_string(empire_id));
    return sitrep;
}

namespace {
    SitRepEntry GenericCombatDamagedObjectSitrep(int combat_system_id, int current_turn) {
        SitRepEntry sitrep(
            UserStringNop("SITREP_OBJECT_DAMAGED_AT_SYSTEM"),
            current_turn + 1,
            "icons/sitrep/combat_damage.png",
            UserStringNop("SITREP_OBJECT_DAMAGED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::SYSTEM_ID_TAG, std::to_string(combat_system_id));
        return sitrep;
    }

    SitRepEntry GenericCombatDestroyedObjectSitrep(int combat_system_id, int current_turn) {
        SitRepEntry sitrep(
            UserStringNop("SITREP_OBJECT_DESTROYED_AT_SYSTEM"),
            current_turn + 1,
            "icons/sitrep/combat_destroyed.png",
            UserStringNop("SITREP_OBJECT_DESTROYED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::SYSTEM_ID_TAG, std::to_string(combat_system_id));
        return sitrep;
    }
}

SitRepEntry CreateCombatDamagedObjectSitRep(int object_id, int combat_system_id, int empire_id,
                                            const ObjectMap& objects, int current_turn)
{
    auto obj = objects.get(object_id);
    if (!obj)
        return GenericCombatDamagedObjectSitrep(combat_system_id, current_turn);

    SitRepEntry sitrep;

    if (auto ship = std::dynamic_pointer_cast<const Ship>(obj)) {
        if (ship->Unowned())
            sitrep = SitRepEntry(
                UserStringNop("SITREP_UNOWNED_SHIP_DAMAGED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_damage.png",
                UserStringNop("SITREP_UNOWNED_SHIP_DAMAGED_AT_SYSTEM_LABEL"), true);
        else
            sitrep = SitRepEntry(
                UserStringNop("SITREP_SHIP_DAMAGED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_damage.png",
                UserStringNop("SITREP_SHIP_DAMAGED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::SHIP_ID_TAG,   std::to_string(object_id));
        sitrep.AddVariable(VarText::DESIGN_ID_TAG, std::to_string(ship->DesignID()));

    } else if (auto planet = std::dynamic_pointer_cast<const Planet>(obj)) {
        if (planet->Unowned())
            sitrep = SitRepEntry(
                UserStringNop("SITREP_UNOWNED_PLANET_ATTACKED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/colony_bombarded.png",
                UserStringNop("SITREP_UNOWNED_PLANET_ATTACKED_AT_SYSTEM_LABEL"), true);
        else
            sitrep = SitRepEntry(
                UserStringNop("SITREP_PLANET_ATTACKED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/colony_bombarded.png",
                UserStringNop("SITREP_PLANET_ATTACKED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::PLANET_ID_TAG, std::to_string(object_id));

    } else {
        sitrep = GenericCombatDestroyedObjectSitrep(combat_system_id, current_turn);
    }

    sitrep.AddVariable(VarText::EMPIRE_ID_TAG, std::to_string(obj->Owner()));
    sitrep.AddVariable(VarText::SYSTEM_ID_TAG, std::to_string(combat_system_id));

    return sitrep;
}

SitRepEntry CreateCombatDestroyedObjectSitRep(int object_id, int combat_system_id, int empire_id,
                                              int current_turn)
{
    auto obj = EmpireKnownObjects(empire_id).get(object_id); // TODO: pass in objects?
    if (!obj) {
        DebugLogger() << "CreateCombatDestroyedObjectSitRep: Object " << object_id << " does not exist for empire " << empire_id;
        return GenericCombatDestroyedObjectSitrep(combat_system_id, current_turn);
    }

    SitRepEntry sitrep;

    if (auto ship = std::dynamic_pointer_cast<const Ship>(obj)) {
        if (ship->Unowned())
            sitrep = SitRepEntry(
                UserStringNop("SITREP_UNOWNED_SHIP_DESTROYED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_UNOWNED_SHIP_DESTROYED_AT_SYSTEM_LABEL"), true);
        else if (ship->OwnedBy(empire_id))
            sitrep = SitRepEntry(
                UserStringNop("SITREP_OWN_SHIP_DESTROYED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_OWN_SHIP_DESTROYED_AT_SYSTEM_LABEL"), true);
        else
            sitrep = SitRepEntry(
                UserStringNop("SITREP_SHIP_DESTROYED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_SHIP_DESTROYED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::SHIP_ID_TAG,   std::to_string(object_id));
        sitrep.AddVariable(VarText::DESIGN_ID_TAG, std::to_string(ship->DesignID()));

    } else if (auto fleet = std::dynamic_pointer_cast<const Fleet>(obj)) {
        if (fleet->Unowned())
            sitrep = SitRepEntry(
                UserStringNop("SITREP_UNOWNED_FLEET_DESTROYED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_UNOWNED_FLEET_DESTROYED_AT_SYSTEM_LABEL"), true);
        else
            sitrep = SitRepEntry(
                UserStringNop("SITREP_FLEET_DESTROYED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_FLEET_DESTROYED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::FLEET_ID_TAG, std::to_string(object_id));

    } else if (auto planet = std::dynamic_pointer_cast<const Planet>(obj)) {
        if (planet->Unowned())
            sitrep = SitRepEntry(
                UserStringNop("SITREP_UNOWNED_PLANET_DESTROYED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_UNOWNED_PLANET_DESTROYED_AT_SYSTEM_LABEL"), true);
        else
            sitrep = SitRepEntry(
                UserStringNop("SITREP_PLANET_DESTROYED_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_PLANET_DESTROYED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::PLANET_ID_TAG, std::to_string(object_id));

    } else if (auto building = std::dynamic_pointer_cast<const Building>(obj)) {
        if (building->Unowned())
            sitrep = SitRepEntry(
                UserStringNop("SITREP_UNOWNED_BUILDING_DESTROYED_ON_PLANET_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_UNOWNED_BUILDING_DESTROYED_ON_PLANET_AT_SYSTEM_LABEL"), true);
        else
            sitrep = SitRepEntry(
                UserStringNop("SITREP_BUILDING_DESTROYED_ON_PLANET_AT_SYSTEM"),
                current_turn + 1,
                "icons/sitrep/combat_destroyed.png",
                UserStringNop("SITREP_BUILDING_DESTROYED_ON_PLANET_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::BUILDING_ID_TAG, std::to_string(object_id));
        sitrep.AddVariable(VarText::PLANET_ID_TAG,   std::to_string(building->PlanetID()));
    } else {
        sitrep = GenericCombatDestroyedObjectSitrep(combat_system_id, current_turn);
    }

    sitrep.AddVariable(VarText::EMPIRE_ID_TAG, std::to_string(obj->Owner()));
    sitrep.AddVariable(VarText::SYSTEM_ID_TAG, std::to_string(combat_system_id));

    return sitrep;
}

SitRepEntry CreatePlanetDepopulatedSitRep(int planet_id) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_PLANET_DEPOPULATED"),
        CurrentTurn() + 1,
        "icons/sitrep/colony_destroyed.png",
        UserStringNop("SITREP_PLANET_DEPOPULATED_LABEL"), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,     std::to_string(planet_id));
    return sitrep;
}

SitRepEntry CreatePlanetColonizedSitRep(int planet_id, const std::string& species) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_PLANET_COLONIZED"),
        CurrentTurn() + 1,
        "icons/sitrep/planet_colonized.png",
        UserStringNop("SITREP_PLANET_COLONIZED_LABEL"), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,  std::to_string(planet_id));
    sitrep.AddVariable(VarText::SPECIES_TAG,    species);
    return sitrep;
}

SitRepEntry CreatePlanetOutpostedSitRep(int planet_id) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_PLANET_OUTPOSTED"),
        CurrentTurn() + 1,
        "icons/sitrep/planet_colonized.png",
        UserStringNop("SITREP_PLANET_OUTPOSTED_LABEL"), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,     std::to_string(planet_id));
    return sitrep;
}

SitRepEntry CreatePlanetGiftedSitRep(int planet_id, int empire_id) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_PLANET_GIFTED"),
        CurrentTurn() + 1,
        "icons/sitrep/gift.png",
        UserStringNop("SITREP_PLANET_GIFTED_LABEL"), true);
    sitrep.AddVariable(VarText::PLANET_ID_TAG,  std::to_string(planet_id));
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG,  std::to_string(empire_id));
    return sitrep;
}

SitRepEntry CreateFleetGiftedSitRep(int fleet_id, int empire_id) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_FLEET_GIFTED"),
        CurrentTurn() + 1,
        "icons/sitrep/gift.png",
        UserStringNop("SITREP_FLEET_GIFTED_LABEL"), true);
    sitrep.AddVariable(VarText::FLEET_ID_TAG,   std::to_string(fleet_id));
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG,  std::to_string(empire_id));
    return sitrep;
}

SitRepEntry CreateFleetArrivedAtDestinationSitRep(int system_id, int fleet_id, int recipient_empire_id) {
    const Universe& u = GetUniverse();
    const ObjectMap& o = u.Objects();

    auto fleet = o.get<Fleet>(fleet_id);

    //bool system_contains_recipient_empire_planets = false;
    //if (const System* system = Objects().get<System>(system_id)) {
    //    for (const auto& planet : system->all<Planet>()) {
    //        if (!planet || planet->Unowned())
    //            continue;
    //        if (planet->OwnedBy(recipient_empire_id)) {
    //            system_contains_recipient_empire_planets = true;
    //            break;
    //        }
    //    }
    //}

    //There are variants of this message for {monster, own, foreign} * {one ship, fleet}.
    // TODO: More variants for systems with / without recipient-owned planets
    //These should really be assembled from several pieces: a fleet description,
    //a system description, and a message template into which both are substituted.
    if (!fleet) {
        SitRepEntry sitrep(
            UserStringNop("SITREP_FLEET_ARRIVED_AT_SYSTEM"),
            CurrentTurn() + 1,
            "icons/sitrep/fleet_arrived.png",
            UserStringNop("SITREP_FLEET_ARRIVED_AT_SYSTEM_LABEL"), true);
        sitrep.AddVariable(VarText::SYSTEM_ID_TAG,  std::to_string(system_id));
        sitrep.AddVariable(VarText::FLEET_ID_TAG,   std::to_string(fleet_id));
        return sitrep;
    } else if (fleet->Unowned() && fleet->HasMonsters(u)) {
        if (fleet->NumShips() == 1) {
            SitRepEntry sitrep(
                UserStringNop("SITREP_MONSTER_SHIP_ARRIVED_AT_DESTINATION"),
                CurrentTurn() + 1,
                "icons/sitrep/fleet_arrived.png",
                UserStringNop("SITREP_MONSTER_SHIP_ARRIVED_AT_DESTINATION_LABEL"), true);
            sitrep.AddVariable(VarText::SYSTEM_ID_TAG,     std::to_string(system_id));
            sitrep.AddVariable(VarText::FLEET_ID_TAG,      std::to_string(fleet_id));
            int ship_id = *fleet->ShipIDs().begin();
            sitrep.AddVariable(VarText::SHIP_ID_TAG,       std::to_string(ship_id));
            if (auto ship = o.get<Ship>(ship_id))
                sitrep.AddVariable(VarText::DESIGN_ID_TAG, std::to_string(ship->DesignID()));
            return sitrep;
        } else {
            SitRepEntry sitrep(
                UserStringNop("SITREP_MONSTER_FLEET_ARRIVED_AT_DESTINATION"),
                CurrentTurn() + 1,
                "icons/sitrep/fleet_arrived.png",
                UserStringNop("SITREP_MONSTER_FLEET_ARRIVED_AT_DESTINATION_LABEL"), true);
            sitrep.AddVariable(VarText::SYSTEM_ID_TAG,  std::to_string(system_id));
            sitrep.AddVariable(VarText::FLEET_ID_TAG,   std::to_string(fleet_id));
            sitrep.AddVariable(VarText::RAW_TEXT_TAG,   std::to_string(fleet->NumShips()));
            return sitrep;
        }
    } else if (fleet->Unowned()) {
        SitRepEntry sitrep(
            UserStringNop("SITREP_FLEET_ARRIVED_AT_DESTINATION"),
            CurrentTurn() + 1,
            "icons/sitrep/fleet_arrived.png",
            UserStringNop("SITREP_FLEET_ARRIVED_AT_DESTINATION_LABEL"), true);
        sitrep.AddVariable(VarText::SYSTEM_ID_TAG,  std::to_string(system_id));
        sitrep.AddVariable(VarText::FLEET_ID_TAG,   std::to_string(fleet_id));
        sitrep.AddVariable(VarText::RAW_TEXT_TAG,   std::to_string(fleet->NumShips()));
        return sitrep;
    } else if (fleet->OwnedBy(recipient_empire_id)) {
        if (fleet->NumShips() == 1) {
            SitRepEntry sitrep(
                UserStringNop("SITREP_OWN_SHIP_ARRIVED_AT_DESTINATION"),
                CurrentTurn() + 1,
                "icons/sitrep/fleet_arrived.png",
                UserStringNop("SITREP_OWN_SHIP_ARRIVED_AT_DESTINATION_LABEL"), true);
            sitrep.AddVariable(VarText::SYSTEM_ID_TAG,     std::to_string(system_id));
            sitrep.AddVariable(VarText::FLEET_ID_TAG,      std::to_string(fleet_id));
            sitrep.AddVariable(VarText::EMPIRE_ID_TAG,     std::to_string(fleet->Owner()));
            int ship_id = *fleet->ShipIDs().begin();
            sitrep.AddVariable(VarText::SHIP_ID_TAG,       std::to_string(ship_id));
            if (auto ship = Objects().get<Ship>(ship_id))
                sitrep.AddVariable(VarText::DESIGN_ID_TAG, std::to_string(ship->DesignID()));
            return sitrep;
        } else {
            SitRepEntry sitrep(
                UserStringNop("SITREP_OWN_FLEET_ARRIVED_AT_DESTINATION"),
                CurrentTurn() + 1,
                "icons/sitrep/fleet_arrived.png",
                UserStringNop("SITREP_OWN_FLEET_ARRIVED_AT_DESTINATION_LABEL"), true);
            sitrep.AddVariable(VarText::SYSTEM_ID_TAG,  std::to_string(system_id));
            sitrep.AddVariable(VarText::FLEET_ID_TAG,   std::to_string(fleet_id));
            sitrep.AddVariable(VarText::EMPIRE_ID_TAG,  std::to_string(fleet->Owner()));
            sitrep.AddVariable(VarText::RAW_TEXT_TAG,   std::to_string(fleet->NumShips()));
            return sitrep;
        }
    } else {
        if (fleet->NumShips() == 1) {
            SitRepEntry sitrep(
                UserStringNop("SITREP_FOREIGN_SHIP_ARRIVED_AT_DESTINATION"),
                CurrentTurn() + 1,
                "icons/sitrep/fleet_arrived.png",
                UserStringNop("SITREP_FOREIGN_SHIP_ARRIVED_AT_DESTINATION_LABEL"), true);
            sitrep.AddVariable(VarText::SYSTEM_ID_TAG,     std::to_string(system_id));
            sitrep.AddVariable(VarText::FLEET_ID_TAG,      std::to_string(fleet_id));
            sitrep.AddVariable(VarText::EMPIRE_ID_TAG,     std::to_string(fleet->Owner()));
            int ship_id = *fleet->ShipIDs().begin();
            sitrep.AddVariable(VarText::SHIP_ID_TAG,       std::to_string(ship_id));
            if (auto ship = Objects().get<Ship>(ship_id))
                sitrep.AddVariable(VarText::DESIGN_ID_TAG, std::to_string(ship->DesignID()));
            return sitrep;
        } else {
            SitRepEntry sitrep(
                UserStringNop("SITREP_FOREIGN_FLEET_ARRIVED_AT_DESTINATION"),
                CurrentTurn() + 1,
                "icons/sitrep/fleet_arrived.png",
                UserStringNop("SITREP_FOREIGN_FLEET_ARRIVED_AT_DESTINATION_LABEL"), true);
            sitrep.AddVariable(VarText::SYSTEM_ID_TAG,  std::to_string(system_id));
            sitrep.AddVariable(VarText::FLEET_ID_TAG,   std::to_string(fleet_id));
            sitrep.AddVariable(VarText::EMPIRE_ID_TAG,  std::to_string(fleet->Owner()));
            sitrep.AddVariable(VarText::RAW_TEXT_TAG,   std::to_string(fleet->NumShips()));
            return sitrep;
        }
    }
}

SitRepEntry CreateEmpireEliminatedSitRep(int empire_id) {
    SitRepEntry sitrep(
        UserStringNop("SITREP_EMPIRE_ELIMINATED"),
        CurrentTurn() + 1,
        "icons/sitrep/empire_eliminated.png",
        UserStringNop("SITREP_EMPIRE_ELIMINATED_LABEL"), true);
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG, std::to_string(empire_id));
    return sitrep;
}

SitRepEntry CreateVictorySitRep(const std::string& reason_string, int empire_id) {
    SitRepEntry sitrep(reason_string.c_str(), CurrentTurn() + 1,
                       "icons/sitrep/victory.png", UserStringNop("SITREP_VICTORY_LABEL"), true);
    sitrep.AddVariable(VarText::EMPIRE_ID_TAG, std::to_string(empire_id));
    return sitrep;
}

SitRepEntry CreateSitRep(const std::string& template_string, int turn, const std::string& icon,
                         std::vector<std::pair<std::string, std::string>> parameters,
                         const std::string& label, bool stringtable_lookup)
{
    SitRepEntry sitrep(template_string.c_str(), turn, icon.c_str(),
                       label.c_str(), stringtable_lookup);
    sitrep.AddVariables(std::move(parameters));
    return sitrep;
}
