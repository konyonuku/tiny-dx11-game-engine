#include "InputState.h"

void InputState::OnKeyDown(Key key)
{
    if(key == Key::Unknown || key >= Key::Count) return;    
    mCurrent[static_cast<size_t>(key)] = true;
}

void InputState::OnKeyUp(Key key)
{
    if(key == Key::Unknown || key >= Key::Count) return;    
    mCurrent[static_cast<size_t>(key)] = false;
}

void InputState::OnFocusLost()
{
    for(auto& e : mCurrent) e = false;
}

void InputState::BeginFrame()
{
    mPrevious = mCurrent;
}

bool InputState::IsDown(Key key) const
{
    if(key == Key::Unknown || key >= Key::Count) return false;
    return mCurrent[static_cast<size_t>(key)];
}

bool InputState::WasPressed(Key key) const
{
    if(key == Key::Unknown || key >= Key::Count) return false;
    return !mPrevious[static_cast<size_t>(key)] && mCurrent[static_cast<size_t>(key)];
}

bool InputState::WasReleased(Key key) const
{
    if(key == Key::Unknown || key >= Key::Count) return false;
    return !mCurrent[static_cast<size_t>(key)] && mPrevious[static_cast<size_t>(key)];
}
