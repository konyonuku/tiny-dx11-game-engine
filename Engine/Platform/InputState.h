#pragma once

#include <cstdint>
#include <cstddef>
#include <array>


enum class Key : uint8_t {
    Unknown = 0, 
    Up, Left, Down, Right, 
    // W, A, S, D,
    R,
    Num1, Num2, Num3,
    Space, Enter, Escape,
    Count
};

class InputState {
public:
    void OnKeyDown(Key key);
    void OnKeyUp(Key key);
    void OnFocusLost();
    void BeginFrame();
    bool IsDown(Key key) const; //GetKey
    bool WasPressed(Key key) const; //GetKeyDown; prev Released -> curr Pressed 
    bool WasReleased(Key key) const; //GetKeyUp;   prev Pressed  -> curr Released 

private:
    std::array<bool, static_cast<size_t>(Key::Count)> mCurrent {};
    std::array<bool, static_cast<size_t>(Key::Count)> mPrevious {};
};
