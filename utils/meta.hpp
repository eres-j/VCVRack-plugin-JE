#pragma once

// Update if different =========================================================

template<typename T>
bool updateIfDifferent (T& lhs, const T& rhs)
{
    if (lhs == rhs)
        return false;

    lhs = rhs;

    return true;
}

template<typename T>
bool updateIfDifferent (T& lhs, const T&& rhs)
{
    if (lhs == rhs)
        return false;

    lhs = rhs;

    return true;
}
