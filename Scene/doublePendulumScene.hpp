#ifndef DOUBLE_PENDULUM_SCENE_HPP
#define DOUBLE_PENDULUM_SCENE_HPP

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include <vector>
#include "UI/Component/Label.hpp"

using Vec2 = Engine::Point;

class doublePendulumScene final : public Engine::IScene
{
private:
    // 位置與參數
    Vec2 origin;
    float len1 = 150.0f, len2 = 150.0f;
    float mass1 = 10.0f, mass2 = 10.0f;
    float angle1 = 3.14159f / 2, angle2 = 3.14159f / 2;
    float angleVel1 = 0.0f, angleVel2 = 0.0f;
    float angleAcc1 = 0.0f, angleAcc2 = 0.0f;
    float gravity = 0.8f;

    // 拖曳控制
    bool dragging = false;
    bool draggingFirst = false;
    float dragOffsetAngle = 0.0f;

    // 殘影
    std::vector<Vec2> trail;
    Engine::Label* gravityValueLabel;
    Engine::Label* len1ValueLabel;
    Engine::Label* mass1ValueLabel;
    Engine::Label* mass2ValueLabel;

public:
    explicit doublePendulumScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;

    void OnKeyDown(int keyCode) override;
    void OnMouseDown(int button, int x, int y) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseUp(int button, int x, int y) override;

    void BackOnClick(int stage);
};

#endif // DOUBLE_PENDULUM_SCENE_HPP