#include "Scene/doublePendulumScene.hpp"
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <allegro5/allegro_audio.h>
#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void doublePendulumScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    origin = Vec2(halfW, halfH);
    trail.clear();

    // 建立返回按鈕
    Engine::ImageButton *btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 430, halfH + -350, 400, 100);
    btn->SetOnClickCallback(std::bind(&doublePendulumScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 48, halfW + 600, halfH - 300, 0, 0, 0, 255, 0.5, 0.5));
    //AddNewObject(new Engine::Label("Press R to reset", "pirulen.ttf", 24, halfW + 630, halfH - 230, 255, 255, 255, 255, 0.5, 0.5));

    // 初始角度與速度
    angle1 = angle2 = 3.14159f / 2;
    angleVel1 = angleVel2 = 0;

    // 物理參數
    len1 = 200.0f;
    len2 = 150.0f;
    mass1 = 20.0f;
    mass2 = 15.0f;
    gravity = 980.0f;
    dragging = false;

        int baseX = halfW + 430;
        int baseY = halfH - 200;
        int spacing = 150;

    auto AddControlSet = [baseX, baseY, spacing, this](const std::string& name, int index, float* var, float step, float minVal, float maxVal, Engine::Label** valueLabelPtr) {
        int y = baseY + index * spacing;

        // 名稱 Label
        AddNewObject(new Engine::Label(name, "pirulen.ttf", 24,
            baseX + 100, y - 25, 255, 255, 255, 255, 0.5, 0.5));

        // - 按鈕
        auto* decBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
            baseX, y, 50, 50);
        decBtn->SetOnClickCallback([var, step, minVal]() {
            *var = std::max(*var - step, minVal);
        });
        AddNewControlObject(decBtn);
        AddNewObject(new Engine::Label("-", "pirulen.ttf", 32,
            baseX + 25, y + 25, 0, 0, 0, 255, 0.5, 0.5));

        // + 按鈕
        auto* incBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
            baseX + 160, y, 50, 50);
        incBtn->SetOnClickCallback([var, step, maxVal]() {
            *var = std::min(*var + step, maxVal);
        });
        AddNewControlObject(incBtn);
        AddNewObject(new Engine::Label("+", "pirulen.ttf", 32,
            baseX + 185, y + 25, 0, 0, 0, 255, 0.5, 0.5));

        // 數值顯示 Label
        *valueLabelPtr = new Engine::Label("", "pirulen.ttf", 24,
            baseX + 100, y + 60, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(*valueLabelPtr);
    };

    // 四組變數控制：gravity, len1, mass1, mass2
    AddControlSet("Gravity", 0, &gravity, 50, 0, 2000, &gravityValueLabel);
    AddControlSet("Length", 1, &len1, 10, 0, 400, &len1ValueLabel);
    AddControlSet("Mass1", 2, &mass1, 5, 0, 100, &mass1ValueLabel);
    AddControlSet("Mass2", 3, &mass2, 5, 0, 100, &mass2ValueLabel);



}

void doublePendulumScene::Terminate()
{
    trail.clear();
}

void doublePendulumScene::Update(float deltaTime)
{
        char buffer[32];
    sprintf(buffer, "Value: %.1f", gravity);
    gravityValueLabel->Text = buffer;

    sprintf(buffer, "Value: %.1f", len1);
    len1ValueLabel->Text = buffer;

    sprintf(buffer, "Value: %.1f", mass1);
    mass1ValueLabel->Text = buffer;

    sprintf(buffer, "Value: %.1f", mass2);
    mass2ValueLabel->Text = buffer;

    IScene::Update(deltaTime); // 很重要！控制元件更新

    if (!dragging)
    {
        // 雙擺加速度計算
        float num1 = -gravity * (2 * mass1 + mass2) * sin(angle1);
        float num2 = -mass2 * gravity * sin(angle1 - 2 * angle2);
        float num3 = -2 * sin(angle1 - angle2) * mass2;
        float num4 = angleVel2 * angleVel2 * len2 + angleVel1 * angleVel1 * len1 * cos(angle1 - angle2);
        float den = len1 * (2 * mass1 + mass2 - mass2 * cos(2 * angle1 - 2 * angle2));
        angleAcc1 = (num1 + num2 + num3 * num4) / den;

        num1 = 2 * sin(angle1 - angle2);
        num2 = angleVel1 * angleVel1 * len1 * (mass1 + mass2);
        num3 = gravity * (mass1 + mass2) * cos(angle1);
        num4 = angleVel2 * angleVel2 * len2 * mass2 * cos(angle1 - angle2);
        den = len2 * (2 * mass1 + mass2 - mass2 * cos(2 * angle1 - 2 * angle2));
        angleAcc2 = (num1 * (num2 + num3 + num4)) / den;

        // 更新角速度與角度
        angleVel1 += angleAcc1 * deltaTime;
        angleVel2 += angleAcc2 * deltaTime;
        angle1 += angleVel1 * deltaTime;
        angle2 += angleVel2 * deltaTime;

        // 阻尼
        angleVel1 *= 0.999f;
        angleVel2 *= 0.999f;
    }

    // 殘影
    float x1 = origin.x + len1 * sin(angle1);
    float y1 = origin.y + len1 * cos(angle1);
    float x2 = x1 + len2 * sin(angle2);
    float y2 = y1 + len2 * cos(angle2);
    trail.emplace_back(x2, y2);
    if (trail.size() > 200)
        trail.erase(trail.begin());
}

void doublePendulumScene::Draw() const
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    IScene::Draw();

    Vec2 p1(origin.x + len1 * sin(angle1), origin.y + len1 * cos(angle1));
    Vec2 p2(p1.x + len2 * sin(angle2), p1.y + len2 * cos(angle2));

    al_draw_line(origin.x, origin.y, p1.x, p1.y, al_map_rgb(255, 255, 255), 3);
    al_draw_line(p1.x, p1.y, p2.x, p2.y, al_map_rgb(255, 255, 255), 3);

    al_draw_filled_circle(p1.x, p1.y, mass1, al_map_rgb(200, 0, 0));
    al_draw_filled_circle(p2.x, p2.y, mass2, al_map_rgb(0, 200, 0));

    for (size_t i = 1; i < trail.size(); ++i)
    {
        al_draw_line(
            trail[i - 1].x, trail[i - 1].y,
            trail[i].x, trail[i].y,
            al_map_rgba(100, 100, 255, 100), 2.0f);
    }
}

void doublePendulumScene::OnKeyDown(int keyCode)
{
    if (keyCode == ALLEGRO_KEY_R)
    {
        Initialize();
    }
}

void doublePendulumScene::OnMouseDown(int button, int x, int y)
{
    IScene::OnMouseDown(button, x, y); // 傳遞給控制元件！

    float x1 = origin.x + len1 * sin(angle1);
    float y1 = origin.y + len1 * cos(angle1);
    float x2 = x1 + len2 * sin(angle2);
    float y2 = y1 + len2 * cos(angle2);

    if (hypot(x - x2, y - y2) < 15)
    {
        dragging = true;
        draggingFirst = false;
        dragOffsetAngle = angle2 - atan2(-(y - y1), x - x1);
    }
    else if (hypot(x - x1, y - y1) < 15)
    {
        dragging = true;
        draggingFirst = true;
        dragOffsetAngle = angle1 - atan2(-(y - origin.y), x - origin.x);
    }
}

void doublePendulumScene::OnMouseMove(int x, int y)
{
    IScene::OnMouseMove(x, y);

    if (dragging)
    {
        if (draggingFirst)
        {
            angle1 = atan2(-(y - origin.y), x - origin.x) + dragOffsetAngle;
        }
        else
        {
            float x1 = origin.x + len1 * sin(angle1);
            float y1 = origin.y + len1 * cos(angle1);
            angle2 = atan2(-(y - y1), x - x1) + dragOffsetAngle;
        }
        angleVel1 = angleVel2 = 0;
    }
}

void doublePendulumScene::OnMouseUp(int button, int x, int y)
{
    IScene::OnMouseUp(button, x, y);
    dragging = false;
}

void doublePendulumScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
