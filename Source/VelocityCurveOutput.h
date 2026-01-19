// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <sstream>

#include "UnitTypes.h"
#include "Vector3.h"

namespace CurveLib
{
    // Used to store the output of a single velocity curve, or the output of multiple curves combined.
    struct VelocityCurveOutput
    {
        Float3 mDirection;
        Velocity mVelocity;
        Velocity mAngularVelocity;
        MetersPerSecond mSpeed{};
        Position mPosition;
        Position mPositionDelta;
        EulerAngles mRotation;
        EulerAngles mRotationDelta;
        bool mHasEverUpdated = false;

        std::string ToString() const
        {
            std::stringstream ss;
            ss << "VelocityCurveOutput:" << std::endl
                << "Direction: " << mDirection.ToString() << std::endl
                << "Velocity: " << mVelocity.ToString() << std::endl
                << "Speed: " << mSpeed << std::endl
                << "Position: " << mPosition.ToString() << std::endl
                << "Position delta: " << mPositionDelta.ToString() << std::endl
                << "Rotation: " << mRotation.ToString() << std::endl
                << "Rotation delta: " << mRotationDelta.ToString();
            return ss.str();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const VelocityCurveOutput& output)
    {
        os << output.ToString();
        return os;
    }
}