/*#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "InputTextBox.hpp"
#include "GameEngine.hpp"
#include "Resources.hpp"

namespace Engine {
    InputTextBox::InputTextBox(const std::string& font, int fontSize, float x, float y, int width, int height)
        : font(font), fontSize(fontSize), width(width), height(height), isFocused(false) {
        Position.x = x;
        Position.y = y;
        textColor = al_map_rgb(0, 0, 0);
        backgroundColor = al_map_rgb(255, 255, 255);
        borderColor = al_map_rgb(100, 100, 100);
        fontObj = al_load_ttf_font(font.c_str(), fontSize, 0);
    }

    void InputTextBox::Draw() const {
        // 繪製背景
        al_draw_filled_rectangle(Position.x - width/2, Position.y - height/2,
                                Position.x + width/2, Position.y + height/2,
                                backgroundColor);
        
        // 繪製邊框
        al_draw_rectangle(Position.x - width/2, Position.y - height/2,
                         Position.x + width/2, Position.y + height/2,
                         isFocused ? al_map_rgb(0, 0, 255) : borderColor, 2);

        // 繪製文字
        if (!text.empty()) {
            al_draw_text(fontObj, textColor,
                        Position.x, Position.y - fontSize/2,
                        ALLEGRO_ALIGN_CENTER, text.c_str());
        }

        // 如果獲得焦點，繪製游標
        if (isFocused) {
            static float cursorTime = 0;
            cursorTime += 0.5f;
            if (static_cast<int>(cursorTime) % 2 == 0) {
                float textWidth = al_get_text_width(fontObj, text.c_str());
                al_draw_line(Position.x + textWidth/2 + 2, Position.y - height/2 + 5,
                            Position.x + textWidth/2 + 2, Position.y + height/2 - 5,
                            textColor, 2);
            }
        }
    }

    void InputTextBox::Update(float deltaTime) {
        // 更新游標閃爍
    }

    void InputTextBox::OnMouseDown(int button, int mx, int my) {
        if (button == 1) { // 左鍵點擊
            isFocused = (mx >= Position.x - width/2 && mx <= Position.x + width/2 &&
                        my >= Position.y - height/2 && my <= Position.y + height/2);
        }
    }

    void InputTextBox::OnKeyDown(int keyCode) {
        if (!isFocused) return;

        if (keyCode == ALLEGRO_KEY_BACKSPACE) {
            if (!text.empty()) {
                text.pop_back();
            }
        }
        else if (keyCode == ALLEGRO_KEY_ENTER) {
            isFocused = false;
        }
        else {
            // 檢查是否為可打印字符
            if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z) {
                char c = 'a' + (keyCode - ALLEGRO_KEY_A);
                if (GameEngine::GetInstance().IsKeyDown(ALLEGRO_KEY_LSHIFT) ||
                    GameEngine::GetInstance().IsKeyDown(ALLEGRO_KEY_RSHIFT)) {
                    c = toupper(c);
                }
                text += c;
            }
            else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
                text += '0' + (keyCode - ALLEGRO_KEY_0);
            }
            else if (keyCode == ALLEGRO_KEY_SPACE) {
                text += ' ';
            }
        }
    }
} */