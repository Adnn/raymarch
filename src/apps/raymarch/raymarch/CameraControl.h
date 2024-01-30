#pragma once

#include <math/Angle.h>
#include <math/Spherical.h>
#include <math/Vector.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>

namespace math = ad::math;


struct CameraControl
{
    void callbackCursorPosition(double xpos, double ypos);
    void callbackMouseButton(int button, int action, int mods);
    void callbackScroll(double xoffset, double yoffset);

    enum class ControlMode
    {
        None,
        Orbit,
    };

    math::Spherical<float> mPose{3.f};
    ControlMode mControlMode = ControlMode::None;
    math::Position<2, float> mPreviousDragPosition;
    static constexpr math::Vec<2, float> gMouseControlFactor = {1.f/400, 1.f/400};
    static constexpr float gScrollFactor = 0.01f;
};


void CameraControl::callbackCursorPosition(double xpos, double ypos)
{
    using Radian = math::Radian<float>;
    // top-left corner origin
    math::Position<2, float> cursorPosition{(float)xpos, (float)ypos};

    switch (mControlMode)
    {
        case ControlMode::Orbit:
        {
            auto angularIncrements = (cursorPosition - mPreviousDragPosition).cwMul(gMouseControlFactor);
            // The viewed object should turn in the direction of the mouse,
            // so the camera angles are changed in the opposite direction (hence the substractions).
            mPose.azimuthal() += Radian{-angularIncrements.x()};
            mPose.polar() += Radian{-angularIncrements.y()};
            mPose.polar() = std::max(Radian{0},
                                     std::min(Radian{math::pi<float>}, mPose.polar()));

            break;
        }
        default:
        {
            // pass
        }
    }

    mPreviousDragPosition = cursorPosition;
}


void CameraControl::callbackMouseButton(int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                mControlMode = ControlMode::Orbit;
                break;
        }
    }
    else if ((button == GLFW_MOUSE_BUTTON_LEFT) && action == GLFW_RELEASE)
    {
        mControlMode = ControlMode::None;
    }
}


void CameraControl::callbackScroll(double xoffset, double yoffset)
{
    mPose.radius() *= 1 - ((float)yoffset * gScrollFactor); 
}