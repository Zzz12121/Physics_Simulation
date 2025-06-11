#ifndef INPUTBOX_HPP
#define INPUTBOX_HPP

#include <string>
#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Engine/IControl.hpp"
namespace Engine {
    class InputBox : public IControl,public IObject {
    protected:
        Point position;
        int width, height;
        std::string text;
        std::string hintText;
        bool focused = false;

    public:
        InputBox(float x, float y, int width, int height, std::string hintText = "");

        void Draw() const override;

        // 支援滑鼠和鍵盤事件（雖然不在 IObject，但你仍可手動呼叫）
        void OnKeyDown(int keyCode);
        void OnMouseDown(int button, int mx, int my);

        std::string GetText() const;
        void SetText(const std::string& newText);
    };
}
#endif // INPUTBOX_HPP