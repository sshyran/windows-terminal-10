// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "../inc/SizeAdjustment.h"

SizeAdjustment SizeAdjustment::FromString(const wchar_t* str)
{
    float amount = 0;
    Unit unit = Unit::None;
    ReferenceFrame referenceFrame = ReferenceFrame::Absolute;

    const auto firstChar = *str;

    if (firstChar == '+')
    {
        referenceFrame = ReferenceFrame::Relative;
        // std::from_chars can't deal with numbers like +12.34.
        str++;
    }
    else if (firstChar == '-')
    {
        referenceFrame = ReferenceFrame::Relative;
    }

    auto& errnoRef = errno; // Nonzero cost, pay it once.
    errnoRef = 0;

    wchar_t* end;
    amount = std::wcstof(str, &end);

    if (str == end || errnoRef == ERANGE)
    {
        return {};
    }

    if (*end)
    {
        if (wcscmp(end, L"%") == 0)
        {
            unit = Unit::Percent;
        }
        else if (wcscmp(end, L"px") == 0)
        {
            unit = Unit::Pixel;
        }
        else if (wcscmp(end, L"pt") == 0)
        {
            unit = Unit::Point;
        }
        else
        {
            return {};
        }
    }

    return { amount, unit, referenceFrame };
}

float SizeAdjustment::AdjustValue(float value, float dpi) const noexcept
{
    if (!amount)
    {
        return value;
    }

    auto result = 0.0f;

    switch (unit)
    {
    case Unit::None:
        result = value * amount;
        break;
    case Unit::Percent:
        result = value * amount / 100.0f;
        break;
    case Unit::Pixel:
        result = amount / 96.0f * dpi;
        break;
    case Unit::Point:
        result = amount / 72.0f * dpi;
        break;
    }

    if (referenceFrame == ReferenceFrame::Relative)
    {
        result += value;
    }

    return result;
}
