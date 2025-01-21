#pragma once

enum class FeatureFlags {
    None = 0,
    Controller = 1,
    Reverser = 2,
    Crane = 4,
    InputRegisters = 8,
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