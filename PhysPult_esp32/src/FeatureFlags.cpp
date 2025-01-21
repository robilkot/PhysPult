#include "FeatureFlags.h"

bool has_flag(FeatureFlags flags, FeatureFlags target)
{
    return (flags & target) == target;
}