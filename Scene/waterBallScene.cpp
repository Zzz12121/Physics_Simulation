#include "waterBallScene.hpp"
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
void waterBallScene::Initialize() {
    const float offsetX = 0;
    const float offsetY = 50;

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // Jelly 在左半邊
    centerPos = Vec2(w / 4, halfH + offsetY);

    int N = 24;
    pts.resize(N);
    edgeLen = 2 * ALLEGRO_PI * R0 / N;
    for (int i = 0; i < N; ++i) {
        float a = 2 * ALLEGRO_PI * i / N;
        Vec2 p = Vec2(centerPos.x + R0 * cos(a), centerPos.y + R0 * sin(a));
        pts[i].pos = pts[i].prev = p;
        edges.emplace_back(i, (i + 1) % N);
    }

    initArea = 0;
    for (int i = 0; i < N; ++i) {
        int j = (i + 1) % N;
        initArea += pts[i].pos.x * pts[j].pos.y - pts[j].pos.x * pts[i].pos.y;
    }
    initArea = fabs(initArea) * 0.5f;

    

        // 右側置中區域：重設基準點（例如靠右 1/4 中心）
int centerX = (w * 3) / 4;
int centerY = halfH;

// 放大用的尺寸
int fontSize = 36;
int btnSize = 60;

// GRAVITY 標題
AddNewObject(new Engine::Label("GRAVITY", "pirulen.ttf", fontSize,
    centerX, centerY - 100, 255, 255, 255, 255, 0.5, 0.5));

// 數值 Label
gravityLabel = new Engine::Label(std::to_string((int)gravity), "pirulen.ttf", fontSize,
    centerX, centerY, 255, 255, 255, 255, 0.5, 0.5);
AddNewObject(gravityLabel);

// - 按鈕
auto gravityDec = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
    centerX - 140, centerY - btnSize / 2, btnSize, btnSize);
gravityDec->SetOnClickCallback([this]() {
    gravity = std::max(100.0f, gravity - 50.0f);
    gravityLabel->Text = std::to_string((int)gravity);
});
AddNewControlObject(gravityDec);
AddNewObject(new Engine::Label("-", "pirulen.ttf", fontSize,
    centerX - 110, centerY + 10, 0, 0, 0, 255, 0.5, 0.5));

// + 按鈕
auto gravityInc = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
    centerX + 80, centerY - btnSize / 2, btnSize, btnSize);
gravityInc->SetOnClickCallback([this]() {
    gravity = std::min(2000.0f, gravity + 50.0f);
    gravityLabel->Text = std::to_string((int)gravity);
});
AddNewControlObject(gravityInc);
AddNewObject(new Engine::Label("+", "pirulen.ttf", fontSize,
    centerX + 110, centerY + 10, 0, 0, 0, 255, 0.5, 0.5));

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 500, 400, 100);
    btn->SetOnClickCallback(std::bind(&waterBallScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 48, halfW, halfH / 2 + 550, 0, 0, 0, 255, 0.5, 0.5));

}

void waterBallScene::Update(float dt)
{
    ALLEGRO_MOUSE_STATE ms;
    al_get_mouse_state(&ms);
    Vec2 mouse{(float)ms.x, (float)ms.y};

    // Verlet
    for (auto &p : pts)
    {
        if (!dragWhole)
        {
            Vec2 cur = p.pos;
            Vec2 acc{0, gravity};
            p.pos = p.pos + (p.pos - p.prev) + acc * dt * dt;
            p.prev = cur;
        }
        else
        {
            p.prev = p.pos;
        }

        // 邊界
        float vx = p.pos.x - p.prev.x;
        float vy = p.pos.y - p.prev.y;
        if (p.pos.y > floorY)
        {
            p.pos.y = floorY;
            p.prev.y = p.pos.y + vy * (rest/10);
        }
        if (p.pos.x < leftWall)
        {
            p.pos.x = leftWall;
            p.prev.x = p.pos.x + vx * (rest/10);
        }
        if (p.pos.x > rightWall)
        {
            p.pos.x = rightWall;
            p.prev.x = p.pos.x + vx * (rest/10 );
        }
    }

    // 拖曳
    if (dragWhole)
    {
        Vec2 delta = mouse - prevMouse;
        for (auto &p : pts)
        {
            p.pos = p.pos + delta;
            p.prev = p.pos;
        }
        prevMouse = mouse;
    }
    if (dragNode && dragIdx != -1)
    {
        pts[dragIdx].pos = mouse + dragOff;
        pts[dragIdx].prev = pts[dragIdx].pos;
    }

    // 拘束 (邊長)
    for (int k = 0; k < 5; ++k)
    {
        for (auto [i, j] : edges)
        {
            Vec2 v = pts[j].pos - pts[i].pos;
            float d = std::sqrt(Dot(v, v));
            if (d == 0)
                continue;
            Vec2 corr = v * (0.5f * (d - edgeLen) / d);
            pts[i].pos = pts[i].pos + corr;
            pts[j].pos = pts[j].pos - corr;
        }

        // 面積
        float A = 0;
        for (int i = 0; i < (int)pts.size(); ++i)
        {
            int j = (i + 1) % pts.size();
            A += pts[i].pos.x * pts[j].pos.y - pts[j].pos.x * pts[i].pos.y;
        }
        A = std::fabs(A) * 0.5f;
        float diff = (A - initArea) / initArea;
        Vec2 cen{0, 0};
        for (auto &p : pts)
            cen = cen + p.pos;
        cen = cen * (1.f / pts.size());
        for (auto &p : pts)
        {
            Vec2 dir = Normalize(p.pos - cen);
            p.pos = p.pos - dir * diff * 2.f;
        }
    }

    // 滑鼠點擊判斷
    if (al_mouse_button_down(&ms, 1))
    {
        if (!dragWhole && !dragNode)
        {
            Vec2 cen{0, 0};
            for (auto &p : pts)
                cen = cen + p.pos;
            cen = cen * (1.f / pts.size());
            if (std::sqrt(Dot(mouse - cen, mouse - cen)) < R0 * 0.6f)
            {
                dragWhole = true;
                prevMouse = mouse;
            }
            else
            {
                float best = 1e9;
                int idx = -1;
                for (int i = 0; i < (int)pts.size(); ++i)
                {
                    float d = std::sqrt(Dot(pts[i].pos - mouse, pts[i].pos - mouse));
                    if (d < best)
                    {
                        best = d;
                        idx = i;
                    }
                }
                if (best < 25)
                {
                    dragNode = true;
                    dragIdx = idx;
                    dragOff = pts[idx].pos - mouse;
                }
            }
        }
    }
    else
    {
        dragWhole = dragNode = false;
        dragIdx = -1;
    }
}
void waterBallScene::Draw() const {
    al_clear_to_color(al_map_rgb(22, 22, 28));  // 先清背景

    al_draw_filled_circle(pts[0].pos.x, pts[0].pos.y, 10, al_map_rgb(255, 0, 0));

    IScene::Draw();

    // 畫 Jelly 主體
    std::vector<ALLEGRO_VERTEX> fan(pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        fan[i] = {pts[i].pos.x, pts[i].pos.y, 0, 0, 0, al_map_rgba(70, 200, 220, 150)};
    }
    al_draw_prim(fan.data(), nullptr, nullptr, 0, pts.size(), ALLEGRO_PRIM_TRIANGLE_FAN);

    // 畫邊框
    for (auto [i, j] : edges) {
        al_draw_line(pts[i].pos.x, pts[i].pos.y, pts[j].pos.x, pts[j].pos.y, al_map_rgb(220, 255, 255), 2);
    }

    // 畫邊界
    al_draw_line(leftWall, 0, leftWall, 600, al_map_rgb(180, 180, 180), 1);
    al_draw_line(rightWall, 0, rightWall, 600, al_map_rgb(180, 180, 180), 1);
    al_draw_line(0, floorY, 800, floorY, al_map_rgb(180, 180, 180), 1);

    // 最後畫 UI
    
}


void waterBallScene::BackOnClick(int stage)
{
    //cout << "BackOnClick triggered\n";
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void waterBallScene::Terminate()
{
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = nullptr;
    IScene::Terminate();
}
