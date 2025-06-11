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

/*
#ifndef StageSelectScene_HPP
#define StageSelectScene_HPP

#include <vector>

// 軽量 Vec2 用於物理運算
struct Vec2 { float x, y; };
Vec2 operator+(Vec2 a, Vec2 b);
Vec2 operator-(Vec2 a, Vec2 b);
Vec2 operator*(Vec2 a, float s);
float dot(const Vec2& a, const Vec2& b);
float len(const Vec2& v);
Vec2 normalize(const Vec2& v);

// HexagonBounce: 模擬旋轉六邊形內小球彈跳，可滑鼠拖曳
class HexagonBounce {
public:
    explicit StageSelectScene() = default;
    void Initialize();            // 資源載入 (如有)
    void Update(float deltaTime); // 物理更新 + 拖曳
    void Draw() const;            // 繪製六邊形與球

private:
    void computeHexagon();        // 計算頂點 + 法線
    void handleCollision();       // 球與牆反彈
    Vec2 toLocal(const Vec2& m) const; // 全局滑鼠 -> 本地座標

    int W, H;
    float gravity, restitution, friction;
    float ballRadius, hexRadius;
    float angle, angularVel;
    Vec2  ballPos, ballVel;
    bool  dragging;
    std::vector<Vec2> verts, normals;
};

#endif // HEXAGONBOUNCE_HPP
*/