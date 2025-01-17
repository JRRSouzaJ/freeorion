//! GiGi - A GUI for OpenGL
//!
//!  Copyright (C) 2007 T. Zachary Laine <whatwasthataddress@gmail.com>
//!  Copyright (C) 2013-2020 The FreeOrion Project
//!
//! Released under the GNU Lesser General Public License 2.1 or later.
//! Some Rights Reserved.  See COPYING file or https://www.gnu.org/licenses/lgpl-2.1.txt
//! SPDX-License-Identifier: LGPL-2.1-or-later

//! @file GG/Flags.h
//!
//! Contains Flags and related classes, used to ensure typesafety when using
//! bitflags.

#ifndef _GG_Flags_h_
#define _GG_Flags_h_


#include <cassert>
#include <iosfwd>
#include <map>
#include <set>
#include <type_traits>
#include <GG/Exception.h>


namespace GG {

namespace detail {
    constexpr inline std::size_t OneBits(unsigned int num)
    {
        std::size_t retval = 0;
        const std::size_t NUM_BITS = sizeof(num) * 8;
        for (std::size_t i = 0; i < NUM_BITS; ++i) {
            if (num & 1)
                ++retval;
            num >>= 1;
        }
        return retval;
    }
}


/** \brief Metafunction predicate that evaluates as true iff \a T is a GG flag
    type, declared by using GG_FLAG_TYPE. */
template <typename T>
struct is_flag_type : std::false_type {};
template <class T>
inline constexpr bool is_flag_type_v = is_flag_type<T>::value;

/** \brief Defines a new type \a name that is usable as a bit-flag type that
    can be used by Flags and FlagSpec.

    The resulting code defines a specialization for is_flag_type, the flag
    class itself, streaming operators for the flag type, and the forward
    declaration of FlagSpec::instance() for the flag type.  The user must
    define FlagSpec::instance(). */
#define GG_FLAG_TYPE(name)                                              \
    class name;                                                         \
                                                                        \
    template <>                                                         \
    struct is_flag_type<name> : std::true_type {};                      \
                                                                        \
    class GG_API name                                                   \
    {                                                                   \
    public:                                                             \
        constexpr name() noexcept = default;                            \
        constexpr explicit name(unsigned int value) :                   \
            m_value(value)                                              \
        {                                                               \
            if (1u < detail::OneBits(value))                            \
                throw std::invalid_argument(                            \
                    "Non-bitflag passed to " #name " constructor");     \
        }                                                               \
        constexpr bool operator==(name rhs) const                       \
        { return m_value == rhs.m_value; }                              \
        constexpr bool operator!=(name rhs) const                       \
        { return m_value != rhs.m_value; }                              \
        constexpr bool operator<(name rhs) const                        \
        { return m_value < rhs.m_value; }                               \
    private:                                                            \
        unsigned int m_value = 0;                                       \
        friend class Flags<name>;                                       \
    };                                                                  \
                                                                        \
    template <>                                                         \
    FlagSpec<name>& FlagSpec<name>::instance();                         \
                                                                        \
    inline std::ostream& operator<<(std::ostream& os, name n)           \
    {                                                                   \
        os << FlagSpec<name>::instance().ToString(n);                   \
        return os;                                                      \
    }                                                                   \
                                                                        \
    inline std::istream& operator>>(std::istream& is, name& n)          \
    {                                                                   \
        std::string str;                                                \
        is >> str;                                                      \
        n = FlagSpec<name>::instance().FromString(str);                 \
        return is;                                                      \
    }


/** Defines the implementation of FlagSpec::instance() for the flag type \a
    name. */
#define GG_FLAGSPEC_IMPL(name)                          \
    template <>                                         \
    FlagSpec<name>& FlagSpec<name>::instance()          \
    {                                                   \
        static FlagSpec retval;                         \
        return retval;                                  \
    }


/** \brief A singleton that encapsulates the set of known flags of type \a
    FlagType.

    New user-defined flags must be registered with FlagSpec in order to be
    used in Flags objects for operator~ to work properly with flags of type \a
    FlagType.  FlagSpec is designed to be extensible.  That is, it is
    understood that the flags used by GG may be insufficient for all
    subclasses that users may write, and FlagSpec allows authors of GG-derived
    classes to add flags.  For instance, a subclass of Wnd may want to add a
    MINIMIZABLE flag.  Doing so is as simple as declaring it and registering
    it with \verbatim FlagSpec<WndFlag>::instance.insert(MINIMIZABLE,
    "MINIMIZABLE") \endverbatim.  If user-defined subclasses and their
    associated user-defined flags are loaded in a runtime-loaded library,
    users should take care to erase them from the FlagSpec when the library is
    unloaded.  \note All user-instantiated FlagSpecs must provide their own
    implementations of the instance() static function (all the GG-provided
    FlagSpec instantiations provide such implementations already). */
template <typename FlagType>
class GG_API FlagSpec
{
public:
    // If you have received an error message directing you to the line below,
    // it means you probably have tried to use this class with a FlagsType
    // that is not a type generated by GG_FLAG_TYPE.  Use that to generate new
    // flag types.
    static_assert(is_flag_type_v<FlagType>, "Using FlagsType without GG_FLAG_TYPE macro");

    /** Iterator over all known flags. */
    typedef typename std::set<FlagType>::iterator iterator;
    /** Const iterator over all known flags. */
    typedef typename std::set<FlagType>::const_iterator const_iterator;

    /** The base class for FlagSpec exceptions. */
    GG_ABSTRACT_EXCEPTION(Exception);

    /** Thrown when a flag-to-string conversion is requested for an unknown flag. */
    GG_CONCRETE_EXCEPTION(UnknownFlag, GG::FlagSpec, Exception);

    /** Thrown when a string-to-flag conversion is requested for an unknown string. */
    GG_CONCRETE_EXCEPTION(UnknownString, GG::FlagSpec, Exception);

    /** Returns the singelton instance of this class. */
    static FlagSpec& instance();

    /** Returns true iff FlagSpec contains \a flag. */
    bool contains(FlagType flag) const
    { return m_flags.count(flag) != 0; }
    /** Returns true iff \a flag is a "permanent" flag -- a flag used
        internally by the GG library, as opposed to a user-added flag. */
    bool permanent(FlagType flag) const
    { return m_permanent.count(flag) != 0; }
    /** Returns an iterator to \a flag, if flag is in the FlagSpec, or end()
        otherwise. */
    const_iterator find(FlagType flag) const
    { return m_flags.find(flag); }
    /** Returns an iterator to the first flag in the FlagSpec. */
    const_iterator begin() const
    { return m_flags.begin(); }
    /** Returns an iterator to one past the last flag in the FlagSpec. */
    const_iterator end() const
    { return m_flags.end(); }
    /** Returns the stringification of \a flag provided when \a flag was added
        to the FlagSpec.  \throw Throws GG::FlagSpec::UnknownFlag if an
        unknown flag's stringification is requested. */
    const std::string& ToString(FlagType flag) const
    {
        auto it = m_strings.find(flag);
        if (it == m_strings.end())
            throw UnknownFlag("Could not find string corresponding to unknown flag");
        return it->second;
    }
    /** Returns the flag whose stringification is \a str.  \throw Throws
        GG::FlagSpec::UnknownString if an unknown string is provided. */
    FlagType FromString(const std::string& str) const
    {
        for (const auto& string : m_strings) {
            if (string.second == str)
                return string.first;
        }
        throw UnknownString("Could not find flag corresponding to unknown string");
    }

    /** Adds \a flag, with stringification string \a name, to the FlagSpec.
        If \a permanent is true, this flag becomes non-removable.  Alls flags
        added by GG are added as permanent flags.  User-added flags should not
        be added as permanent. */
    template<typename S>
    void insert(FlagType flag, S&& name, bool permanent = false)
    {
#ifndef NDEBUG
        std::pair<typename std::set<FlagType>::iterator, bool> result =
#endif
        m_flags.emplace(flag);
#ifndef NDEBUG
        assert(result.second);
#endif
        if (permanent)
            m_permanent.emplace(flag);
        m_strings.emplace(flag, std::forward<S>(name));
    }
    /** Removes \a flag from the FlagSpec, returning whether the flag was
        actually removed or not.  Permanent flags are not removed.  The
        removal of flags will probably only be necessary in cases where flags
        were added for classes in a runtime-loaded DLL/shared library at
        DLL/shared library unload-time. */
    bool erase(FlagType flag)
    {
        bool retval = true;
        if (permanent(flag)) {
            retval = false;
        } else {
            m_flags.erase(flag);
            m_permanent.erase(flag);
            m_strings.erase(flag);
        }
        return retval;
    }

private:
    FlagSpec() {}

    std::set<FlagType>              m_flags;
    std::set<FlagType>              m_permanent;
    std::map<FlagType, std::string> m_strings;
};


template <typename FlagType>
class Flags;

template <typename FlagType>
std::ostream& operator<<(std::ostream& os, Flags<FlagType> flags);

/** \brief A set of flags of the same type.

    Individual flags and sets of flags can be passed as parameters and/or be
    stored as member variables in Flags objects. */
template <typename FlagType>
class Flags
{
private:
    struct ConvertibleToBoolDummy {int _;};

public:
    // If you have received an error message directing you to the line below,
    // it means you probably have tried to use this class with a FlagsType
    // that is not a type generated by GG_FLAG_TYPE.  Use that to generate new
    // flag types.
    static_assert(is_flag_type_v<FlagType>, "Using Flags without GG_FLAG_TYPE macro");

    /** The base class for Flags exceptions. */
    GG_ABSTRACT_EXCEPTION(Exception);

    /** Thrown when an unknown flag is used to construct a Flags. */
    GG_CONCRETE_EXCEPTION(UnknownFlag, GG::Flags, Exception);

    Flags() :
        m_flags(0)
    {}

    /** Ctor.  Note that this ctor allows implicit conversions from FlagType
        to Flags.  \throw Throws GG::Flags::UnknownFlag if \a flag is not
        found in FlagSpec<FlagType>::instance(). */
    Flags(FlagType flag) :
        m_flags(flag.m_value)
    {
        if (!FlagSpec<FlagType>::instance().contains(flag))
            throw UnknownFlag("Invalid flag with value " + std::to_string(flag.m_value));
    }

    /** Conversion to bool, so that a Flags object can be used as a boolean
        test.  It is convertible to true when it contains one or more flags,
        and convertible to false otherwise. */
    operator int ConvertibleToBoolDummy::* () const
    { return m_flags ? &ConvertibleToBoolDummy::_ : 0; }
    /** Returns true iff *this contains the same flags as \a rhs. */
    bool operator==(Flags<FlagType> rhs) const
    { return m_flags == rhs.m_flags; }
    /** Returns true iff *this does not contain the same flags as \a rhs. */
    bool operator!=(Flags<FlagType> rhs) const
    { return m_flags != rhs.m_flags; }
    /** Returns true iff the underlying storage of *this is less than the
        underlying storage of \a rhs.  Note that this is here for use in
        associative containers only; it is otherwise meaningless. */
    bool operator<(Flags<FlagType> rhs) const
    { return m_flags < rhs.m_flags; }

    /** Performs a bitwise-or of *this and \a rhs, placing the result in *this. */
    Flags<FlagType>& operator|=(Flags<FlagType> rhs)
    {
        m_flags |= rhs.m_flags;
        return *this;
    }
    /** Performs a bitwise-and of *this and \a rhs, placing the result in *this. */
    Flags<FlagType>& operator&=(Flags<FlagType> rhs)
    {
        m_flags &= rhs.m_flags;
        return *this;
    }
    /** Performs a bitwise-xor of *this and \a rhs, placing the result in *this. */
    Flags<FlagType>& operator^=(Flags<FlagType> rhs)
    {
        m_flags ^= rhs.m_flags;
        return *this;
    }

private:
    unsigned int m_flags;

    friend std::ostream& operator<<<>(std::ostream& os, Flags<FlagType> flags);
};

/** Writes \a flags to \a os in the format "flag1 | flag2 | ... flagn". */
template <typename FlagType>
std::ostream& operator<<(std::ostream& os, Flags<FlagType> flags)
{
    unsigned int flags_data = flags.m_flags;
    bool flag_printed = false;
    for (std::size_t i = 0; i < sizeof(flags_data) * 8; ++i) {
        if (flags_data & 1) {
            if (flag_printed)
                os << " | ";
            os << FlagSpec<FlagType>::instance().ToString(FlagType(1 << i));
            flag_printed = true;
        }
        flags_data >>= 1;
    }
    return os;
}

/** Returns a Flags object that consists of the bitwise-or of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator|(Flags<FlagType> lhs, Flags<FlagType> rhs)
{
    Flags<FlagType> retval(lhs);
    retval |= rhs;
    return retval;
}

/** Returns a Flags object that consists of the bitwise-or of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator|(Flags<FlagType> lhs, FlagType rhs)
{ return lhs | Flags<FlagType>(rhs); }

/** Returns a Flags object that consists of the bitwise-or of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator|(FlagType lhs, Flags<FlagType> rhs)
{ return Flags<FlagType>(lhs) | rhs; }

/** Returns a Flags object that consists of the bitwise-or of \a lhs and \a
    rhs. */
template <typename FlagType>
typename std::enable_if_t<
    is_flag_type_v<FlagType>,
    Flags<FlagType>
>
operator|(FlagType lhs, FlagType rhs)
{ return Flags<FlagType>(lhs) | Flags<FlagType>(rhs); }

/** Returns a Flags object that consists of the bitwise-and of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator&(Flags<FlagType> lhs, Flags<FlagType> rhs)
{
    Flags<FlagType> retval(lhs);
    retval &= rhs;
    return retval;
}

/** Returns a Flags object that consists of the bitwise-and of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator&(Flags<FlagType> lhs, FlagType rhs)
{ return lhs & Flags<FlagType>(rhs); }

/** Returns a Flags object that consists of the bitwise-and of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator&(FlagType lhs, Flags<FlagType> rhs)
{ return Flags<FlagType>(lhs) & rhs; }

/** Returns a Flags object that consists of the bitwise-and of \a lhs and \a
    rhs. */
template <typename FlagType>
typename std::enable_if_t<
    is_flag_type_v<FlagType>,
    Flags<FlagType>
>
operator&(FlagType lhs, FlagType rhs)
{ return Flags<FlagType>(lhs) & Flags<FlagType>(rhs); }

/** Returns a Flags object that consists of the bitwise-xor of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator^(Flags<FlagType> lhs, Flags<FlagType> rhs)
{
    Flags<FlagType> retval(lhs);
    retval ^= rhs;
    return retval;
}

/** Returns a Flags object that consists of the bitwise-xor of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator^(Flags<FlagType> lhs, FlagType rhs)
{ return lhs ^ Flags<FlagType>(rhs); }

/** Returns a Flags object that consists of the bitwise-xor of \a lhs and \a
    rhs. */
template <typename FlagType>
Flags<FlagType> operator^(FlagType lhs, Flags<FlagType> rhs)
{ return Flags<FlagType>(lhs) ^ rhs; }

/** Returns a Flags object that consists of the bitwise-xor of \a lhs and \a
    rhs. */
template <typename FlagType>
typename std::enable_if_t<
    is_flag_type_v<FlagType>,
    Flags<FlagType>
>
operator^(FlagType lhs, FlagType rhs)
{ return Flags<FlagType>(lhs) ^ Flags<FlagType>(rhs); }

/** Returns a Flags object that consists of all the flags known to
    FlagSpec<FlagType>::instance() except those in \a flags. */
template <typename FlagType>
Flags<FlagType> operator~(Flags<FlagType> flags)
{
    Flags<FlagType> retval;
    for (const FlagType& flag : FlagSpec<FlagType>::instance()) {
        if (!(flag & flags))
            retval |= flag;
    }
    return retval;
}

/** Returns a Flags object that consists of all the flags known to
    FlagSpec<FlagType>::instance() except \a flag. */
template <typename FlagType>
typename std::enable_if_t<
    is_flag_type_v<FlagType>,
    Flags<FlagType>
>
operator~(FlagType flag)
{ return ~Flags<FlagType>(flag); }

}


#endif
