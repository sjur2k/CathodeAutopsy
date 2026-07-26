#pragma once

class Window;
class InputManager;

struct GLFWUserContext {
    Window* window = nullptr;
    InputManager* input_manager = nullptr;
};