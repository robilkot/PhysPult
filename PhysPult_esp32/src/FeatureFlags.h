#pragma once

enum class FeatureFlags {
    Controller = 0,
    Reverser = 1,
    Crane = 2,
    InputRegisters = 4,
};

inline FeatureFlags operator|(FeatureFlags a, FeatureFlags b)
{
    return static_cast<FeatureFlags>(static_cast<int>(a) | static_cast<int>(b));
}
inline FeatureFlags operator&(FeatureFlags a, FeatureFlags b)
{
    return static_cast<FeatureFlags>(static_cast<int>(a) & static_cast<int>(b));
}
inline FeatureFlags operator~(FeatureFlags a)
{
    return static_cast<FeatureFlags>(~static_cast<int>(a));
}

bool has_flag(FeatureFlags flags, FeatureFlags target);