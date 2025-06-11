/*#pragma once
#include <string>
#include "IObject.hpp"
#include "Point.hpp"

namespace Engine {
    class InputTextBox : public IObject {
    private:
        std::string text;
        std::string font;
        int fontSize;
        int width;
        int height;
        bool isFocused;
        ALLEGRO_COLOR textColor;
        ALLEGRO_COLOR backgroundColor;
        ALLEGRO_COLOR borderColor;
        ALLEGRO_FONT* fontObj;
    public:
        InputTextBox(const std::string& font, int fontSize, float x, float y, int width, int height);
        void Draw() const override;
        void Update(float deltaTime) override;
        void OnMouseDown(int button, int mx, int my) override;
        void OnKeyDown(int keyCode) override;
        std::string GetText() const { return text; }
        void SetText(const std::string& newText) { text = newText; }
        bool IsFocused() const { return isFocused; }
        void SetFocused(bool focused) { isFocused = focused; }
    };
} */