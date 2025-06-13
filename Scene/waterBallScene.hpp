#ifndef WATERBALLSCENE_HPP
#define WATERBALLSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <vector>
#include <memory>
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/Slider.hpp"
#include "UI/Component/Label.hpp"

using Vec2 = Engine::Point;

struct JellyPoint
{
    Vec2 pos, prev;
};

class waterBallScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::shared_ptr<Slider> gravitySlider;
    std::shared_ptr<Slider> restSlider;

    std::vector<JellyPoint> pts;
    std::vector<std::pair<int, int>> edges;
    std::vector<Vec2> norms;

    Vec2 centerPos = Vec2(600, 300); // 中心座標
    float R0 = 90.0f;
    float edgeLen;
    float gravity = 980.0f;
    float rest = 10.0f;
    float floorY = 600 - 40.0f;
    float leftWall = 80.0f;
    float rightWall = 800 - 80.0f;
    float initArea = 0;

    // 拖曳控制
    bool dragWhole = false, dragNode = false;
    int dragIdx = -1;
    Vec2 dragOff = Vec2(0, 0);
    Vec2 prevMouse = Vec2(0, 0);
    Engine::Label* gravityLabel;
    Engine::Label* restLabel;


public:
    explicit waterBallScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;

    void BackOnClick(int stage);
};



#endif // WATERBALLSCENE_HPP