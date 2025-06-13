#ifndef STAGESELECTSCENE_HPP
#define STAGESELECTSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

// 自定義 Vec2（如果你還沒定義）
using Vec2 = Engine::Point;

class StageSelectScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

    // 加入動畫成員變數
    Vec2 ballPos, ballVel;
    std::vector<Vec2> verts, norms;
    float ballRadius = 12.0f;
    float hexRadius = 250.0f;
    float gravity = 900.0f;
    float restitution = 0.8f;
    float friction = 0.1f;
    float angle = 0;
    float angularVelocity = ALLEGRO_PI / 8;
    int polygonSides = 6; // 預設為六邊形
    bool dragging = false;

public:
    explicit StageSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;

    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void BackOnClick(int stage);
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
};

#endif // STAGESELECTSCENE_HPP