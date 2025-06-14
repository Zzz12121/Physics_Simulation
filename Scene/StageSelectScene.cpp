#include "StageSelectScene.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <cmath>

#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

inline Vec2 Normalize(const Vec2 &v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len == 0)
        return Vec2(0, 0);
    return Vec2(v.x / len, v.y / len);
}

inline float Dot(const Vec2 &a, const Vec2 &b)
{
    return a.x * b.x + a.y * b.y;
}

void StageSelectScene::Initialize()
{
    // 初始化球的位置與速度
    ballPos = Vec2{0, -150};
    ballVel = Vec2{110, 0};
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 500, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 48, halfW, halfH / 2 + 550, 0, 0, 0, 255, 0.5, 0.5));
    // 播放背景音樂
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    // 初始化其他 UI（你已註解掉的可視需求再開啟）

    auto incBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 300, halfH / 2 + 470, 100, 60);
    incBtn->SetOnClickCallback([this]() {
        if (polygonSides < 20) polygonSides++;
    });
    AddNewControlObject(incBtn);
    AddNewObject(new Engine::Label("+", "pirulen.ttf", 36, halfW + 350, halfH / 2 + 500, 0, 0, 0, 255, 0.5, 0.5));

    auto decBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 400, halfH / 2 + 470, 100, 60);
    decBtn->SetOnClickCallback([this]() {
        if (polygonSides > 3) polygonSides--;
    });
    AddNewControlObject(decBtn);
    AddNewObject(new Engine::Label("-", "pirulen.ttf", 36, halfW - 350, halfH / 2 + 500, 0, 0, 0, 255, 0.5, 0.5));
    }

void StageSelectScene::Update(float deltaTime)
{   
    // --- 處理滑鼠拖曳小球 ---
    ALLEGRO_MOUSE_STATE ms;
    al_get_mouse_state(&ms);

    // 六邊形圓心 (以畫面中心為原點)
    Engine::Point screenCenter = Engine::GameEngine::GetInstance().GetScreenSize() / 2;
    Vec2 localMouse(
        static_cast<float>(ms.x - screenCenter.x),
        static_cast<float>(ms.y - screenCenter.y )
    );

    // 左鍵按下：判定是否開始拖曳
    if (ms.buttons & 1) {
        if (!dragging) {
            float dx = localMouse.x - ballPos.x;
            float dy = localMouse.y - ballPos.y;
            // 半徑 1.5 倍內才啟動
            if (dx*dx + dy*dy < (ballRadius*1.5f)*(ballRadius*1.5f)) {
                dragging = true;
            }
        }
    }
    else {
        // 放開左鍵：結束拖曳
        dragging = false;
    }

    // 拖曳中：直接把小球放在滑鼠位置，並清零速度
    if (dragging) {
        ballPos = localMouse;
        ballVel = Vec2{0, 0};
    }
    // 否則走原本的重力＋彈跳
    else {
        angle     += angularVelocity * deltaTime;
        ballVel.y += gravity * deltaTime;
        ballPos.x += ballVel.x * deltaTime;
        ballPos.y += ballVel.y * deltaTime;
    }

    angle += angularVelocity * deltaTime;
    ballVel.y += gravity * deltaTime;
    ballPos = ballPos + ballVel * deltaTime;

    Vec2 center = Vec2{
    Engine::GameEngine::GetInstance().GetScreenSize().x / 2.0f,
    Engine::GameEngine::GetInstance().GetScreenSize().y / 2.0f
};

    verts.clear();
    norms.clear();
    for (int i = 0; i < polygonSides; ++i) {
        float a0 = angle + i * 2 * ALLEGRO_PI / polygonSides;
        float a1 = angle + (i + 1) * 2 * ALLEGRO_PI / polygonSides;
        Vec2 vi = Vec2{hexRadius * std::cos(a0), hexRadius * std::sin(a0)};
        Vec2 vj = Vec2{hexRadius * std::cos(a1), hexRadius * std::sin(a1)};
        Vec2 edge = vj - vi;
        Vec2 normal = Normalize(Vec2{edge.y, -edge.x});

        verts.push_back(vi);
        norms.push_back(normal);
    }

    for (int i = 0; i < polygonSides; ++i) {
        Vec2 a = verts[i] + center;
        Vec2 n = norms[i];
        Vec2 relBallPos = ballPos + center;

        float dist = Dot(relBallPos - a, n);
        if (dist > -ballRadius) {
            relBallPos = relBallPos - n * (dist + ballRadius);
            ballPos = relBallPos - center;

            float vn = Dot(ballVel, n);
            if (vn > 0) {
                Vec2 vt = ballVel - n * vn;
                vt = vt * (1.0f - friction);
                ballVel = vt - n * vn * restitution;
            }
        }
    }
}


void StageSelectScene::Draw() const
{
    IScene::Draw();

    Engine::Point center = Engine::GameEngine::GetInstance().GetScreenSize() / 2;

    for (int i = 0; i < polygonSides; ++i) {
        int j = (i + 1) % polygonSides;
        Vec2 vi = verts[i], vj = verts[j];
        al_draw_line(center.x + vi.x, center.y + vi.y,
                     center.x + vj.x, center.y + vj.y,
                     al_map_rgb(200, 200, 200), 2.0f);
    }

    al_draw_filled_circle(center.x + ballPos.x, center.y + ballPos.y,
                          ballRadius, al_map_rgb(255, 80, 80));
}


void StageSelectScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void StageSelectScene::Terminate()
{
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = nullptr;
    IScene::Terminate();
}