#include "UI/Component/InputBox.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro.h>
#include "Engine/Resources.hpp"

namespace Engine {
    InputBox::InputBox(float x, float y, int width, int height, std::string hintText)
        : position(x, y), width(width), height(height), hintText(std::move(hintText)) {}

    void InputBox::Draw() const {
        al_draw_filled_rectangle(position.x, position.y, position.x + width, position.y + height,
                                 focused ? al_map_rgb(255, 255, 255) : al_map_rgb(220, 220, 220));

        al_draw_rectangle(position.x, position.y, position.x + width, position.y + height,
                          al_map_rgb(0, 0, 0), 2);

        std::string displayText = text.empty() && !focused ? hintText : text;

        ALLEGRO_FONT* font = Resources::GetInstance().GetFont("pirulen.ttf", 24).get();
        if (font) {
            al_draw_text(font, al_map_rgb(0, 0, 0),
                         position.x + 10, position.y + (height - al_get_font_line_height(font)) / 2,
                         0, displayText.c_str());
        }
    }

    void InputBox::OnKeyDown(int keyCode) {
        if (!focused) return;

        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z) {
            char c = 'a' + (keyCode - ALLEGRO_KEY_A);
            text += c;
        } else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
            char c = '0' + (keyCode - ALLEGRO_KEY_0);
            text += c;
        } else if (keyCode == ALLEGRO_KEY_SPACE) {
            text += ' ';
        } else if (keyCode == ALLEGRO_KEY_BACKSPACE && !text.empty()) {
            text.pop_back();
        }
    }

    void InputBox::OnMouseDown(int button, int mx, int my) {
        focused = (mx >= position.x && mx <= position.x + width &&
                   my >= position.y && my <= position.y + height);
    }

    std::string InputBox::GetText() const {
        return text;
    }

    void InputBox::SetText(const std::string& newText) {
        text = newText;
    }
}