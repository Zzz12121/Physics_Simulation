#include "Scene/cutBagScene.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cmath>
#include <algorithm>
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include <iomanip>

float SPRING_STIFFNESS = 20.0f;

void cutBagScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    cuttingMode = false;
    simulationSpeed = 1.0f;
    draggedBall = nullptr;
    ballTargetVxs.clear(); // 初始化目標速度陣列

    // 初始化布料
    initializeCloth();

    // 初始化球體
    initializeBalls();

    // 初始化風力場
    wind.strength = WIND_STRENGTH;
    wind.frequency = 0.05f;
    wind.direction = 0.0f;
    wind.time = 0.0f;

    // 初始化字體
    al_init_font_addon();
    al_init_ttf_addon();
    font = al_create_builtin_font();

    int centerX = w / 2;
    int bottomY = h - 250;


    // 顯示 "STIFFNESS"
    AddNewObject(new Engine::Label("STIFFNESS", "pirulen.ttf", 24,
        centerX, bottomY, 255, 255, 255, 255, 0.5, 0.5));
    
    // 顯示數值
    stiffnessLabel = new Engine::Label(std::to_string((int)SPRING_STIFFNESS),
    "pirulen.ttf", 24, centerX, bottomY + 40, 255, 255, 0, 255, 0.5, 0.5);
    AddNewObject(stiffnessLabel);

    // "+" 按鈕
    auto incBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        centerX + 60, bottomY + 30, 40, 40);
    incBtn->SetOnClickCallback([this]() {
        SPRING_STIFFNESS = std::min(100.0f, SPRING_STIFFNESS + 5.0f);
        stiffnessLabel->Text = std::to_string((int)SPRING_STIFFNESS);
    });
    AddNewControlObject(incBtn);
    AddNewObject(new Engine::Label("+", "pirulen.ttf", 24,
        centerX + 80, bottomY + 50, 0, 0, 0, 255, 0.5, 0.5));

    // "-" 按鈕
    auto decBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        centerX - 100, bottomY + 30, 40, 40);
    decBtn->SetOnClickCallback([this]() {
        SPRING_STIFFNESS = std::max(5.0f, SPRING_STIFFNESS - 5.0f);
        stiffnessLabel->Text = std::to_string((int)SPRING_STIFFNESS);
    });
    AddNewControlObject(decBtn);
    AddNewObject(new Engine::Label("-", "pirulen.ttf", 24,
        centerX - 80, bottomY + 50, 0, 0, 0, 255, 0.5, 0.5));


    // 建立返回按鈕
    Engine::ImageButton *btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 430, halfH - 350, 400, 100);
    btn->SetOnClickCallback(std::bind(&cutBagScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 48, halfW + 600, halfH - 300, 0, 0, 0, 255, 0.5, 0.5));
    //AddNewObject(new Engine::Label("Press R to reset, C to toggle cut mode", "pirulen.ttf", 24, halfW + 630, halfH - 230, 255, 255, 255, 255, 0.5, 0.5));
}

void cutBagScene::Terminate()
{   
    // 如果你有用 new 建立的 label，要記得 delete
    if (stiffnessLabel) {
        delete stiffnessLabel;
        stiffnessLabel = nullptr;
    }

    points.clear();
    springs.clear();
    balls.clear();
    ballTargetVxs.clear();
    al_destroy_font(font);
}

void cutBagScene::Update(float deltaTime)
{
    IScene::Update(deltaTime); // 更新控制元件

    // 更新風力場
    wind.time += 0.01f;
    wind.direction = sin(wind.time * wind.frequency) * ALLEGRO_PI;

    // 更新球體
    updateBalls(deltaTime);

    // 應用物理
    applyPhysics(deltaTime);

    // 應用約束
    for (int i = 0; i < CONSTRAINT_ITERATIONS; i++)
    {
        applyConstraints();
    }
}

void cutBagScene::Draw() const
{
    al_clear_to_color(al_map_rgb(20, 20, 30));
    IScene::Draw();

    // 繪製彈簧
    for (const auto &s : springs)
    {
        if (s.active && s.p1->active && s.p2->active)
        {
            float tension = distance(s.p1->x, s.p1->y, s.p2->x, s.p2->y) / s.restLength;
            ALLEGRO_COLOR color;
            if (tension < 1.0f)
            {
                int blue = 255;
                int green = 150 - static_cast<int>(150 * (1.0f - tension));
                color = al_map_rgb(50, green, blue);
            }
            else
            {
                int red = 150 + static_cast<int>(105 * (tension - 1.0f));
                red = std::min(red, 255);
                color = al_map_rgb(red, 50, 50);
            }
            al_draw_line(s.p1->x, s.p1->y, s.p2->x, s.p2->y, color, 1.5f);
        }
    }

    // 繪製點
    for (const auto &p : points)
    {
        if (p.active)
        {
            ALLEGRO_COLOR color = p.fixed ? al_map_rgb(255, 255, 0) : al_map_rgb(200, 200, 255);
            al_draw_filled_circle(p.x, p.y, 2.0f, color);
        }
    }

    // 繪製球體
    for (const auto &ball : balls)
    {
        al_draw_filled_circle(ball.x, ball.y, ball.radius, ball.color);
        al_draw_circle(ball.x, ball.y, ball.radius, al_map_rgb(0, 0, 0), 2.0f);
    }

    // 繪製地面
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    al_draw_line(0, Engine::GameEngine::GetInstance().GetScreenSize().y - 50, w, Engine::GameEngine::GetInstance().GetScreenSize().y - 50, al_map_rgb(100, 100, 100), 2.0f);

    // 繪製UI
    //drawUI();
}

void cutBagScene::OnKeyDown(int keyCode)
{
    if (keyCode == ALLEGRO_KEY_R)
    {
        Initialize();
    }
    else if (keyCode == ALLEGRO_KEY_C)
    {
        cuttingMode = !cuttingMode;
    }
    else if (keyCode == ALLEGRO_KEY_1)
    {
        simulationSpeed = 0.5f;
    }
    else if (keyCode == ALLEGRO_KEY_2)
    {
        simulationSpeed = 1.0f;
    }
    else if (keyCode == ALLEGRO_KEY_3)
    {
        simulationSpeed = 2.0f;
    }
}

void cutBagScene::OnMouseDown(int button, int x, int y)
{
    IScene::OnMouseDown(button, x, y);
    if (button == 1)
    {
        for (auto &ball : balls)
        {
            float dx = ball.x - x;
            float dy = ball.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < ball.radius)
            {
                draggedBall = &ball;
                break;
            }
        }
    }
    else if (button == 2 && cuttingMode)
    {
        cutCloth(x, y, 20.0f);
    }
}

void cutBagScene::OnMouseMove(int x, int y)
{
    IScene::OnMouseMove(x, y);
    if (draggedBall)
    {
        draggedBall->x = x;
        draggedBall->y = y;
        draggedBall->vx = 0.0f; // 重置速度以避免拖曳後突然移動
        draggedBall->vy = 0.0f;
    }
}

void cutBagScene::OnMouseUp(int button, int x, int y)
{
    IScene::OnMouseUp(button, x, y);
    if (button == 1)
    {
        draggedBall = nullptr;
    }
}

void cutBagScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void cutBagScene::initializeCloth()
{
    points.clear();
    springs.clear();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;

    // 創建布料點
    for (int y = 0; y < CLOTH_HEIGHT; y++)
    {
        for (int x = 0; x < CLOTH_WIDTH; x++)
        {
            Point p;
            p.x = w / 2 - (CLOTH_WIDTH * GRID_SPACING) / 2 + x * GRID_SPACING;
            p.y = 100 + y * GRID_SPACING;
            p.px = p.x;
            p.py = p.y;
            p.vx = 0;
            p.vy = 0;
            p.mass = 1.0f;
            p.fixed = (y == 0 && (x == 0 || x == CLOTH_WIDTH - 1));
            p.active = true;
            points.push_back(p);
        }
    }

    // 創建彈簧
    for (int y = 0; y < CLOTH_HEIGHT; y++)
    {
        for (int x = 0; x < CLOTH_WIDTH; x++)
        {
            int idx = y * CLOTH_WIDTH + x;
            if (x < CLOTH_WIDTH - 1)
            {
                addSpring(idx, y * CLOTH_WIDTH + (x + 1));
            }
            if (y < CLOTH_HEIGHT - 1)
            {
                addSpring(idx, (y + 1) * CLOTH_WIDTH + x);
            }
            if (x < CLOTH_WIDTH - 1 && y < CLOTH_HEIGHT - 1)
            {
                addSpring(idx, (y + 1) * CLOTH_WIDTH + (x + 1));
            }
            if (x > 0 && y < CLOTH_HEIGHT - 1)
            {
                addSpring(idx, (y + 1) * CLOTH_WIDTH + (x - 1));
            }
            if (x < CLOTH_WIDTH - 2)
            {
                addSpring(idx, y * CLOTH_WIDTH + (x + 2));
            }
            if (y < CLOTH_HEIGHT - 2)
            {
                addSpring(idx, (y + 2) * CLOTH_WIDTH + x);
            }
        }
    }
}

void cutBagScene::initializeBalls()
{
    balls.clear();
    ballTargetVxs.clear();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

    CutBagBall ball1;
    ball1.x = w / 3;
    ball1.y = h / 2;
    ball1.vx = 100.0f; // 初始水平速度
    ball1.vy = 0.0f;
    ball1.radius = BALL_RADIUS;
    ball1.mass = BALL_MASS;
    ball1.color = al_map_rgb(255, 100, 100);
    balls.push_back(ball1);
    ballTargetVxs.push_back(100.0f); // 目標水平速度（向右）

    CutBagBall ball2;
    ball2.x = 2 * w / 3;
    ball2.y = h / 2;
    ball2.vx = -100.0f; // 初始水平速度
    ball2.vy = 0.0f;
    ball2.radius = BALL_RADIUS;
    ball2.mass = BALL_MASS;
    ball2.color = al_map_rgb(100, 255, 100);
    balls.push_back(ball2);
    ballTargetVxs.push_back(-100.0f); // 目標水平速度（向左）
}

void cutBagScene::addSpring(int idx1, int idx2)
{
    Spring s;
    s.p1 = &points[idx1];
    s.p2 = &points[idx2];
    s.restLength = distance(s.p1->x, s.p1->y, s.p2->x, s.p2->y);
    s.active = true;
    springs.push_back(s);
}

float cutBagScene::distance(float x1, float y1, float x2, float y2) const
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

void cutBagScene::updateBalls(float deltaTime)
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

    for (size_t i = 0; i < balls.size(); ++i)
    {
        auto &ball = balls[i];
        // 僅當球體未被拖曳時應用自動移動
        if (&ball != draggedBall)
        {
            // 檢查是否接近邊界並反轉方向
            if (ball.x - ball.radius < 0)
            {
                ball.x = ball.radius;
                ballTargetVxs[i] = std::abs(ballTargetVxs[i]); // 向右移動
            }
            else if (ball.x + ball.radius > w)
            {
                ball.x = w - ball.radius;
                ballTargetVxs[i] = -std::abs(ballTargetVxs[i]); // 向左移動
            }

            // 設置水平速度
            ball.vx = ballTargetVxs[i];

            // 應用重力到垂直速度
            ball.vy += GRAVITY * simulationSpeed * deltaTime;

            // 垂直邊界碰撞
            if (ball.y - ball.radius < 0)
            {
                ball.y = ball.radius;
                ball.vy = -ball.vy * 0.8f; // 加入阻尼
            }
            else if (ball.y + ball.radius > h)
            {
                ball.y = h - ball.radius;
                ball.vy = -ball.vy * 0.8f; // 加入阻尼
            }

            // 更新位置
            ball.x += ball.vx * simulationSpeed * deltaTime;
            ball.y += ball.vy * simulationSpeed * deltaTime;
        }
    }
}

void cutBagScene::applyPhysics(float deltaTime)
{
    for (auto &p : points)
    {
        if (!p.active || p.fixed)
            continue;
        p.px = p.x;
        p.py = p.y;
        p.vx = (p.x - p.px) * AIR_RESISTANCE;
        p.vy = (p.y - p.py) * AIR_RESISTANCE;
        p.vy += GRAVITY * simulationSpeed * deltaTime;
        float windForce = wind.strength * sin(wind.time) * simulationSpeed * deltaTime;
        p.vx += windForce * cos(wind.direction);
        p.vy += windForce * sin(wind.direction);
        p.x += p.vx * TIME_STEP * simulationSpeed * deltaTime;
        p.y += p.vy * TIME_STEP * simulationSpeed * deltaTime;
    }

    for (auto &s : springs)
    {
        if (!s.active)
            continue;
        Point &p1 = *s.p1;
        Point &p2 = *s.p2;
        if (!p1.active || !p2.active)
            continue;
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0)
        {
            float f = SPRING_STIFFNESS * (dist - s.restLength);
            float dfx = (dx / dist) * f;
            float dfy = (dy / dist) * f;
            if (!p1.fixed)
            {
                p1.x += dfx * 0.5f * TIME_STEP * simulationSpeed * deltaTime;
                p1.y += dfy * 0.5f * TIME_STEP * simulationSpeed * deltaTime;
            }
            if (!p2.fixed)
            {
                p2.x -= dfx * 0.5f * TIME_STEP * simulationSpeed * deltaTime;
                p2.y -= dfy * 0.5f * TIME_STEP * simulationSpeed * deltaTime;
            }
            float damping = SPRING_DAMPING;
            p1.vx *= (1 - damping);
            p1.vy *= (1 - damping);
            p2.vx *= (1 - damping);
            p2.vy *= (1 - damping);
        }
    }
}

void cutBagScene::applyConstraints()
{
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    for (auto &p : points)
    {
        if (!p.active || p.fixed)
            continue;
        for (const auto &ball : balls)
        {
            float dx = p.x - ball.x;
            float dy = p.y - ball.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < ball.radius)
            {
                if (dist > 0)
                {
                    float overlap = ball.radius - dist;
                    p.x += (dx / dist) * overlap * 0.5f;
                    p.y += (dy / dist) * overlap * 0.5f;
                    p.vx += ball.vx * 0.1f;
                    p.vy += ball.vy * 0.1f;
                }
            }
        }
        if (p.y > h - 50)
        {
            p.y = h - 50;
            p.vy = -p.vy * 0.5f;
        }
        if (p.x < 0)
        {
            p.x = 0;
            p.vx = -p.vx * 0.5f;
        }
        else if (p.x > w)
        {
            p.x = w;
            p.vx = -p.vx * 0.5f;
        }
        if (p.y < 0)
        {
            p.y = 0;
            p.vy = -p.vy * 0.5f;
        }
    }
}

void cutBagScene::cutCloth(int mouseX, int mouseY, float radius)
{
    if (!cuttingMode)
        return;
    for (auto &s : springs)
    {
        if (!s.active)
            continue;
        float midX = (s.p1->x + s.p2->x) * 0.5f;
        float midY = (s.p1->y + s.p2->y) * 0.5f;
        float dx = midX - mouseX;
        float dy = midY - mouseY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < radius)
        {
            s.active = false;
        }
    }
    for (int i = 0; i < points.size(); i++)
    {
        if (points[i].fixed)
            continue;
        bool hasActiveSpring = false;
        for (const auto &s : springs)
        {
            if (!s.active)
                continue;
            if ((s.p1 == &points[i] || s.p2 == &points[i]) &&
                s.p1->active && s.p2->active)
            {
                hasActiveSpring = true;
                break;
            }
        }
        if (!hasActiveSpring)
        {
            points[i].active = false;
        }
    }
}

void cutBagScene::drawUI() const
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    al_draw_text(font, al_map_rgb(255, 255, 255), w / 2, 10,
                 ALLEGRO_ALIGN_CENTRE, "布料模擬 (Cut Bag Simulation)");

    // 繪製操作說明
    al_draw_text(font, al_map_rgb(200, 200, 200), 20, 20, 0,
                 "操作說明:");
    al_draw_textf(font, al_map_rgb(180, 220, 255), 20, 40, 0,
                  "1-3: 調整模擬速度 (當前: %.1fx)", simulationSpeed);
    al_draw_text(font, al_map_rgb(180, 220, 255), 20, 60, 0,
                 "R鍵: 重置模擬");
    al_draw_textf(font, al_map_rgb(180, 220, 255), 20, 80, 0,
                  "C鍵: 切換剪刀模式 (當前: %s)", cuttingMode ? "開啟" : "關閉");
    al_draw_text(font, al_map_rgb(180, 220, 255), 20, 100, 0,
                 "滑鼠左鍵: 拖動球體");
    al_draw_text(font, al_map_rgb(180, 220, 255), 20, 120, 0,
                 "滑鼠右鍵: 使用剪刀切割布料");

    int activePoints = 0;
    int activeSprings = 0;
    for (const auto &p : points)
        if (p.active)
            activePoints++;
    for (const auto &s : springs)
        if (s.active)
            activeSprings++;

    al_draw_textf(font, al_map_rgb(200, 255, 200), w - 20, 20,
                  ALLEGRO_ALIGN_RIGHT, "質點: %d/%d", activePoints, (int)points.size());
    al_draw_textf(font, al_map_rgb(200, 255, 200), w - 20, 40,
                  ALLEGRO_ALIGN_RIGHT, "彈簧: %d/%d", activeSprings, (int)springs.size());
    al_draw_textf(font, al_map_rgb(200, 255, 200), w - 20, 60,
                  ALLEGRO_ALIGN_RIGHT, "風力: %.1f", wind.strength * sin(wind.time));

    drawWindIndicator();
}

void cutBagScene::drawWindIndicator() const
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    const int centerX = w - 60;
    const int centerY = h - 60;
    const int radius = 40;

    al_draw_circle(centerX, centerY, radius, al_map_rgb(100, 100, 200), 2.0f);
    float arrowX = centerX + cos(wind.direction) * radius * 0.8f;
    float arrowY = centerY + sin(wind.direction) * radius * 0.8f;
    al_draw_line(centerX, centerY, arrowX, arrowY, al_map_rgb(255, 100, 100), 3.0f);
    al_draw_textf(font, al_map_rgb(200, 200, 255), centerX, centerY - 20,
                  ALLEGRO_ALIGN_CENTRE, "風向");
}