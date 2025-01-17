#include "Special.h"

#include <boost/filesystem/fstream.hpp>
#include "Condition.h"
#include "Effect.h"
#include "UniverseObject.h"
#include "ValueRef.h"
#include "../util/AppInterface.h"
#include "../util/CheckSums.h"
#include "../util/Logger.h"
#include "../util/OptionsDB.h"
#include "../util/ScopedTimer.h"
#include "../util/i18n.h"


#define CHECK_COND_VREF_MEMBER(m_ptr) { if (m_ptr == rhs.m_ptr) {           \
                                            /* check next member */         \
                                        } else if (!m_ptr || !rhs.m_ptr) {  \
                                            return false;                   \
                                        } else {                            \
                                            if (*m_ptr != *(rhs.m_ptr))     \
                                                return false;               \
                                        }   }

std::vector<std::string_view> SpecialsManager::SpecialNames() const {
    CheckPendingSpecialsTypes();
    std::vector<std::string_view> retval;
    retval.reserve(m_specials.size());
    for (const auto& entry : m_specials)
        retval.emplace_back(entry.first);
    return retval;
}

const Special* SpecialsManager::GetSpecial(const std::string& name) const {
    CheckPendingSpecialsTypes();
    auto it = m_specials.find(name);
    return it != m_specials.end() ? it->second.get() : nullptr;
}

unsigned int SpecialsManager::GetCheckSum() const {
    CheckPendingSpecialsTypes();
    unsigned int retval{0};
    for (auto const& name_type_pair : m_specials)
        CheckSums::CheckSumCombine(retval, name_type_pair);
    CheckSums::CheckSumCombine(retval, m_specials.size());
    DebugLogger() << "SpecialsManager checksum: " << retval;
    return retval;
}

void SpecialsManager::SetSpecialsTypes(Pending::Pending<SpecialsTypeMap>&& future)
{ m_pending_types = std::move(future); }

void SpecialsManager::CheckPendingSpecialsTypes() const {
    if (!m_pending_types)
        return;

    Pending::SwapPending(m_pending_types, m_specials);
}

SpecialsManager& GetSpecialsManager() {
    static SpecialsManager special_manager;
    return special_manager;
}

/////////////////////////////////////////////////
// Special                                     //
/////////////////////////////////////////////////
Special::Special(std::string&& name, std::string&& description,
                 std::unique_ptr<ValueRef::ValueRef<double>>&& stealth,
                 std::vector<std::unique_ptr<Effect::EffectsGroup>>&& effects,
                 double spawn_rate, int spawn_limit,
                 std::unique_ptr<ValueRef::ValueRef<double>>&& initial_capaicty,
                 std::unique_ptr<Condition::Condition>&& location,
                 const std::string& graphic) :
    m_name(std::move(name)),
    m_description(std::move(description)),
    m_stealth(std::move(stealth)),
    m_spawn_rate(spawn_rate),
    m_spawn_limit(spawn_limit),
    m_initial_capacity(std::move(initial_capaicty)),
    m_location(std::move(location)),
    m_graphic(graphic)
{
    for (auto&& effect : effects)
        m_effects.push_back(std::move(effect));

    Init();
}

Special::~Special() = default;

bool Special::operator==(const Special& rhs) const {
    if (&rhs == this)
        return true;

    if (m_name != rhs.m_name ||
        m_description != rhs.m_description ||
        m_spawn_rate != rhs.m_spawn_rate ||
        m_spawn_limit != rhs.m_spawn_limit ||
        m_graphic != rhs.m_graphic)
    { return false; }

    CHECK_COND_VREF_MEMBER(m_stealth)
    CHECK_COND_VREF_MEMBER(m_initial_capacity)
    CHECK_COND_VREF_MEMBER(m_location)

    if (m_effects.size() != rhs.m_effects.size())
        return false;
    try {
        for (std::size_t idx = 0; idx < m_effects.size(); ++idx) {
            const auto& my_op = m_effects.at(idx);
            const auto& rhs_op = rhs.m_effects.at(idx);

            if (my_op == rhs_op)
                continue;
            if (!my_op || !rhs_op)
                return false;
            if (*my_op != *rhs_op)
                return false;
        }
    } catch (...) {
        return false;
    }

    return true;
}

std::string Special::Description() const {
    std::stringstream result;

    result << UserString(m_description) << "\n";

    for (auto& effect : m_effects) {
        const std::string& description = effect->GetDescription();

        if (!description.empty()) {
            result << "\n" << UserString(description) << "\n";
        }
    }

    return result.str();
}

void Special::Init() {
    if (m_stealth)
        m_stealth->SetTopLevelContent(m_name);
    for (auto& effect : m_effects) {
        effect->SetTopLevelContent(m_name);
    }
    if (m_initial_capacity)
        m_initial_capacity->SetTopLevelContent(m_name);
    if (m_location)
        m_location->SetTopLevelContent(m_name);
}

std::string Special::Dump(unsigned short ntabs) const {
    std::string retval = DumpIndent(ntabs) + "Special\n";
    retval += DumpIndent(ntabs+1) + "name = \"" + m_name + "\"\n";
    retval += DumpIndent(ntabs+1) + "description = \"" + m_description + "\"\n";

    if (m_stealth)
        retval += DumpIndent(ntabs+1) + "stealth = " + m_stealth->Dump(ntabs+1) + "\n";

    retval += DumpIndent(ntabs+1) + "spawnrate = " + std::to_string(m_spawn_rate) + "\n"
           +  DumpIndent(ntabs+1) + "spawnlimit = " + std::to_string(m_spawn_limit) + "\n";

    if (m_initial_capacity) {
        retval += DumpIndent(ntabs+1) + "initialcapacity = ";
        retval += m_initial_capacity->Dump(ntabs+2);
    }

    if (m_location) {
        retval += DumpIndent(ntabs+1) + "location =\n";
        retval += m_location->Dump(ntabs+2);
    }

    if (m_effects.size() == 1) {
        retval += DumpIndent(ntabs+1) + "effectsgroups =\n";
        retval += m_effects[0]->Dump(ntabs+2);
    } else {
        retval += DumpIndent(ntabs+1) + "effectsgroups = [\n";
        for (auto& effect : m_effects)
            retval += effect->Dump(ntabs+2);
        retval += DumpIndent(ntabs+1) + "]\n";
    }
    retval += DumpIndent(ntabs+1) + "graphic = \"" + m_graphic + "\"\n";
    return retval;
}

float Special::InitialCapacity(int object_id) const {
    if (!m_initial_capacity)
        return 0.0f;

    auto obj = Objects().get(object_id);    // TODO: pass ScriptingContext and use here...
    if (!obj)
        return 0.0f;

    return m_initial_capacity->Eval(ScriptingContext(std::move(obj)));
}

unsigned int Special::GetCheckSum() const {
    unsigned int retval{0};

    CheckSums::CheckSumCombine(retval, m_name);
    CheckSums::CheckSumCombine(retval, m_description);
    CheckSums::CheckSumCombine(retval, m_stealth);
    CheckSums::CheckSumCombine(retval, m_effects);
    CheckSums::CheckSumCombine(retval, m_initial_capacity);
    CheckSums::CheckSumCombine(retval, m_location);
    CheckSums::CheckSumCombine(retval, m_graphic);

    return retval;
}

const Special* GetSpecial(const std::string& name)
{ return GetSpecialsManager().GetSpecial(name); }

const Special* GetSpecial(std::string_view name)
{ return GetSpecialsManager().GetSpecial(std::string{name}); }

std::vector<std::string_view> SpecialNames()
{ return GetSpecialsManager().SpecialNames(); }
