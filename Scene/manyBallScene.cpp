#include "Scene/manyBallScene.hpp"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <random>

#include "Engine/AudioHelper.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

constexpr int SCREEN_W = 1200;
constexpr int SCREEN_H = 700;
constexpr float MAX_SPEED = 1000.0f;
constexpr float MIN_VELOCITY = 0.5f;
constexpr float DRAG_SPEED_FACTOR = 10.0f;
constexpr float VEC_PX_PER_SPEED = 0.15f;

float friction = 0.99f;

void manyBallScene::Initialize()
{
    balls.clear();
    obstacles.clear();
    draggedBall = nullptr;
    draggedObs = nullptr;
    editMode = false;
    if (!miniMap)
        miniMap = al_create_bitmap(MINI_W, MINI_H);
    // load_map("map.txt");

    static std::random_device rd;
    static std::mt19937 gen(rd());

    if (obstacles.empty())
    {
        const int max_attempts = 100;
        for (int i = 0; i < 10; ++i)
        {
            Obstacle obs;
            int tries = 0;
            do
            {
                obs = generate_random_obstacle();
            } while (is_obstacle_overlapping(obs, obstacles) && ++tries < max_attempts);
            obstacles.push_back(obs);
        }
    }

    // 球體
    std::uniform_real_distribution<float> x_dist(50.0f, SCREEN_W - 50.0f);
    std::uniform_real_distribution<float> y_dist(50.0f, SCREEN_H - 50.0f);
    std::uniform_real_distribution<float> vel_dist(-100.0f, 100.0f);

    const float ball_radius = 20.0f;
    const int max_attempts = 100;
    for (int i = 0; i < 10; ++i)
    {
        Ball b;
        int tries = 0;
        do
        {
            b.x = x_dist(gen);
            b.y = y_dist(gen);
            b.radius = ball_radius;
        } while (is_ball_overlapping(b, balls, obstacles) && ++tries < max_attempts);
        b.vx = vel_dist(gen);
        b.vy = vel_dist(gen);
        b.angularVel = 0;
        b.isDragged = false;
        b.color = random_color();
        balls.push_back(b);
    }

    int w = SCREEN_W, h = SCREEN_H;
    int rightX = w - 200, topY = 100;

    AddNewObject(new Engine::Label("FRICTION", "pirulen.ttf", 28,
                                   rightX + 400, topY + 200, 255, 255, 255, 255, 0.5, 0.5));

    frictionLabel = new Engine::Label(std::to_string(friction).substr(0, 4),
                                      "pirulen.ttf", 24, rightX + 400, topY + 240,
                                      255, 255, 0, 255, 0.5, 0.5);
    AddNewObject(frictionLabel);

    auto incBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
                                          rightX + 450, topY + 230, 40, 40);
    incBtn->SetOnClickCallback([this]()
                               {
        friction = std::min(1.0f, friction + 0.01f);
        frictionLabel->Text = std::to_string(friction).substr(0, 4); });
    AddNewControlObject(incBtn);
    AddNewObject(new Engine::Label("+", "pirulen.ttf", 24,
                                   rightX + 470, topY + 250, 0, 0, 0, 255, 0.5, 0.5));

    auto decBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
                                          rightX + 310, topY + 230, 40, 40);
    decBtn->SetOnClickCallback([this]()
                               {
        friction = std::max(0.9f, friction - 0.01f);
        frictionLabel->Text = std::to_string(friction).substr(0, 4); });
    AddNewControlObject(decBtn);
    AddNewObject(new Engine::Label("-", "pirulen.ttf", 24,
                                   rightX + 330, topY + 250, 0, 0, 0, 255, 0.5, 0.5));

    int halfW = w / 2, halfH = h / 2;
    auto backBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
                                           halfW + 600, halfH - 350, 400, 100);
    backBtn->SetOnClickCallback(std::bind(&manyBallScene::BackOnClick, this, 2));
    AddNewControlObject(backBtn);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 48,
                                   halfW + 800, halfH - 300, 0, 0, 0, 255, 0.5, 0.5));

    miniMap = al_create_bitmap(MINI_W, MINI_H);
}

void manyBallScene::Terminate()
{
    balls.clear();
    obstacles.clear();
    draggedBall = nullptr;
    draggedObs = nullptr;
    IScene::Terminate();
    if (miniMap)
    {
        al_destroy_bitmap(miniMap);
        miniMap = nullptr;
    }
    IScene::Terminate();
}

void manyBallScene::Update(float deltaTime)
{
    for (auto &ball : balls)
    {
        if (!ball.isDragged)
        {
            ball.x += ball.vx * deltaTime;
            ball.y += ball.vy * deltaTime;
            ball.vx *= friction;
            ball.vy *= friction;
        }
        else
        {

            ball.vx = (mouseX - ball.x) * DRAG_SPEED_FACTOR;
            ball.vy = (mouseY - ball.y) * DRAG_SPEED_FACTOR;
            ball.x = mouseX;
            ball.y = mouseY;
        }
        // 邊界反彈
        if (ball.x - ball.radius < 0)
        {
            ball.x = ball.radius;
            ball.vx = -ball.vx;
        }
        if (ball.x + ball.radius > SCREEN_W)
        {
            ball.x = SCREEN_W - ball.radius;
            ball.vx = -ball.vx;
        }
        if (ball.y - ball.radius < 0)
        {
            ball.y = ball.radius;
            ball.vy = -ball.vy;
        }
        if (ball.y + ball.radius > SCREEN_H)
        {
            ball.y = SCREEN_H - ball.radius;
            ball.vy = -ball.vy;
        }
        // 與牆碰撞
        for (const auto &obs : obstacles)
            resolve_ball_obstacle_collision(ball, obs);
    }
    // 球與球碰撞
    for (size_t i = 0; i < balls.size(); ++i)
        for (size_t j = i + 1; j < balls.size(); ++j)
            resolve_ball_collision(balls[i], balls[j]);
    IScene::Update(deltaTime);
}
void manyBallScene::Draw() const
{

    al_clear_to_color(al_map_rgb(0, 0, 0));
    IScene::Draw();

    // 外框
    al_draw_rectangle(0, 0, SCREEN_W, SCREEN_H,
                      al_map_rgb(255, 255, 255), 3);

    // 牆
    for (const auto &obs : obstacles)
    {
        al_draw_filled_rectangle(obs.x, obs.y,
                                 obs.x + obs.width, obs.y + obs.height,
                                 obs.color);
        if (editMode)
        {
            al_draw_rectangle(obs.x, obs.y,
                              obs.x + obs.width, obs.y + obs.height,
                              al_map_rgb(255, 255, 0), 2);
        }
    }

    // 球
    for (const auto &ball : balls)
    {
        al_draw_filled_circle(ball.x, ball.y, ball.radius, ball.color);
        al_draw_circle(ball.x, ball.y, ball.radius,
                       al_map_rgb(255, 255, 255), 2);

        float speed = std::sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
        if (speed > MIN_VELOCITY)
        {
            float angle = std::atan2(ball.vy, ball.vx);

            // 依速度決定線長：speed × 比例，並限制上限
            float vecLen = speed * VEC_PX_PER_SPEED;
            vecLen = std::min(vecLen, ball.radius * 4.0f); // 最長=4倍半徑

            float endX = ball.x + vecLen * std::cos(angle);
            float endY = ball.y + vecLen * std::sin(angle);

            al_draw_line(ball.x, ball.y, endX, endY,
                         al_map_rgb(255, 255, 255), 2.0f);
        }
        char buf[16];
        sprintf(buf, "%.1f", speed);
        al_draw_text(Engine::Resources::GetInstance()
                         .GetFont("pirulen.ttf", 16)
                         .get(),
                     al_map_rgb(255, 255, 0),
                     ball.x, ball.y - ball.radius - 16,
                     ALLEGRO_ALIGN_CENTER, buf);
    }

    // 小地圖繪製邏輯
    ALLEGRO_DISPLAY *display = al_get_current_display();
    ALLEGRO_BITMAP *backbuffer = al_get_backbuffer(display);

    // 切換繪製目標到 miniMap
    al_set_target_bitmap(miniMap);
    al_clear_to_color(al_map_rgb(0, 0, 0)); // 黑底小地圖

    // 繪製障礙物到小地圖
    for (const auto &obs : obstacles)
    {
        float scaleX = MINI_W / static_cast<float>(SCREEN_W);
        float scaleY = MINI_H / static_cast<float>(SCREEN_H);
        al_draw_filled_rectangle(
            obs.x * scaleX,
            obs.y * scaleY,
            (obs.x + obs.width) * scaleX,
            (obs.y + obs.height) * scaleY,
            obs.color);
    }

    // 繪製球到小地圖
    for (const auto &ball : balls)
    {
        float scaleX = MINI_W / static_cast<float>(SCREEN_W);
        float scaleY = MINI_H / static_cast<float>(SCREEN_H);
        al_draw_filled_circle(
            ball.x * scaleX,
            ball.y * scaleY,
            ball.radius * 0.5f * std::min(scaleX, scaleY), // 縮小畫
            ball.color);
    }

    // 切回主畫面
    al_set_target_bitmap(backbuffer);

    // 把小地圖畫到左下角
    int displayW = al_get_display_width(al_get_current_display());
    int displayH = al_get_display_height(al_get_current_display());
    int miniX = displayW - MINI_W - 10;
    int miniY = displayH - MINI_H - 10;
    al_draw_bitmap(miniMap, miniX, miniY, 0);
    al_draw_rectangle(miniX, miniY, miniX + MINI_W, miniY + MINI_H, al_map_rgb(255, 255, 255), 2);
    // al_clear_to_color(al_map_rgb(0, 0, 0));
    /*
        // ② 先畫世界 (邊框、牆、球)──這段就是舊版 Draw 裡的世界迴圈
        al_draw_rectangle(0, 0, SCREEN_W, SCREEN_H,
                          al_map_rgb(255, 255, 255), 3);

        for (const auto &obs : obstacles) {
            al_draw_filled_rectangle(obs.x, obs.y,
                                     obs.x + obs.width, obs.y + obs.height,
                                     obs.color);
        }
        for (const auto &ball : balls) {
            al_draw_filled_circle(ball.x, ball.y, ball.radius, ball.color);
            al_draw_circle(ball.x, ball.y, ball.radius,
                           al_map_rgb(255, 255, 255), 2);

            float speed  = std::sqrt(ball.vx*ball.vx + ball.vy*ball.vy);
            if (speed > MIN_VELOCITY) {
                float angle  = std::atan2(ball.vy, ball.vx);
                float vecLen = std::min(speed*0.15f, ball.radius*4.0f);
                float endX   = ball.x + vecLen * std::cos(angle);
                float endY   = ball.y + vecLen * std::sin(angle);
                al_draw_line(ball.x, ball.y, endX, endY,
                             al_map_rgb(255, 255, 255), 2);
            }
            char buf[16]; sprintf(buf, "%.1f", speed);
            al_draw_text(Engine::Resources::GetInstance()
                            .GetFont("pirulen.ttf", 16).get(),
                         al_map_rgb(255, 255, 0),
                         ball.x, ball.y - ball.radius - 16,
                         ALLEGRO_ALIGN_CENTER, buf);
        }

        // ③ 抓縮圖到 miniMap
        al_set_target_bitmap(miniMap);
        al_clear_to_color(al_map_rgb(0,0,0));
        ALLEGRO_DISPLAY* disp = al_get_current_display();
        al_draw_scaled_bitmap(al_get_backbuffer(disp),
                              0, 0, SCREEN_W, SCREEN_H,         // src
                              0, 0, MINI_W, MINI_H, 0);         // dst
        al_set_target_backbuffer(disp);   // 切回螢幕

        // ④ 畫 UI (Label / Button 等)
        IScene::Draw();

        // ⑤ 將小地圖貼到指定位置
        int rightX = SCREEN_W - 200;  // 跟 Initialize 裏相同
        int topY   = 100;
        int miniX  = rightX + 300;    // 依需求可微調
        int miniY  = topY   + 300;

        al_draw_bitmap(miniMap, miniX, miniY, 0);
        al_draw_rectangle(miniX, miniY,
                          miniX + MINI_W, miniY + MINI_H,
                          al_map_rgb(255,255,255), 2);*/
}

void manyBallScene::OnKeyDown(int keycode)
{
    IScene::OnKeyDown(keycode);

    if (keycode == ALLEGRO_KEY_E)
    { // 編輯模式開關
        editMode = !editMode;
        draggedObs = nullptr;
        draggedBall = nullptr;
    }
    else if (keycode == ALLEGRO_KEY_S && editMode)
    {
        save_map("map.txt");
    }
}

void manyBallScene::OnMouseDown(int button, int x, int y)
{
    IScene::OnMouseDown(button, x, y);
    mouseX = x;
    mouseY = y;

    if (button != 1)
        return;

    if (editMode)
    { // 編輯牆
        draggedObs = hit_test_obstacle(x, y);
    }
    else
    { // 拖球
        for (auto &ball : balls)
        {
            float dx = x - ball.x, dy = y - ball.y;
            if (dx * dx + dy * dy <= ball.radius * ball.radius)
            {
                draggedBall = &ball;
                ball.isDragged = true;
                break;
            }
        }
    }
}
void manyBallScene::OnMouseUp(int button, int x, int y)
{
    IScene::OnMouseUp(button, x, y);
    if (button != 1)
        return;

    if (draggedObs)
        draggedObs = nullptr;
    if (draggedBall)
    {
        draggedBall->isDragged = false;
        draggedBall = nullptr;
    }
}
void manyBallScene::OnMouseMove(int x, int y)
{
    IScene::OnMouseMove(x, y);
    float dx = x - mouseX, dy = y - mouseY;
    mouseX = x;
    mouseY = y;

    if (editMode && draggedObs)
    { // 拖曳牆
        draggedObs->x += dx;
        draggedObs->y += dy;
    } /*
     if (draggedBall) {              // 拖曳球（保持現有邏輯）
         draggedBall->x = x;
         draggedBall->y = y;
     }*/
}

void manyBallScene::load_map(const std::string &path)
{
    std::ifstream fin(path);
    if (!fin)
        return;
    Obstacle o;
    while (fin >> o.x >> o.y >> o.width >> o.height)
    {
        o.color = random_color();
        obstacles.push_back(o);
    }
}
void manyBallScene::save_map(const std::string &path) const
{
    std::ofstream fout(path);
    for (const auto &o : obstacles)
        fout << o.x << ' ' << o.y << ' '
             << o.width << ' ' << o.height << '\n';
}

Obstacle *manyBallScene::hit_test_obstacle(float px, float py)
{
    for (auto &o : obstacles)
    {
        if (px >= o.x && px <= o.x + o.width &&
            py >= o.y && py <= o.y + o.height)
            return &o;
    }
    return nullptr;
}

ALLEGRO_COLOR manyBallScene::random_color() const
{
    return al_map_rgb(50 + rand() % 206, 50 + rand() % 206, 50 + rand() % 206);
}

Obstacle manyBallScene::generate_random_obstacle() const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> wdist(80.f, 160.f);
    std::uniform_real_distribution<float> hdist(80.f, 160.f);

    Obstacle o;
    o.width = wdist(gen);
    o.height = hdist(gen);
    std::uniform_real_distribution<float> xdist(0.f, SCREEN_W - o.width);
    std::uniform_real_distribution<float> ydist(0.f, SCREEN_H - o.height);
    o.x = xdist(gen);
    o.y = ydist(gen);
    o.color = random_color();
    return o;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void manyBallScene::limit_velocity(Ball &ball) const
{
    float speed = std::sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
    if (speed > MAX_SPEED)
    {
        ball.vx = ball.vx / speed * MAX_SPEED;
        ball.vy = ball.vy / speed * MAX_SPEED;
    }
}

bool manyBallScene::ball_obstacle_collision(const Ball &ball, const Obstacle &obs,
                                            float &penetrationX, float &penetrationY) const
{
    float closestX = std::max(obs.x, std::min(ball.x, obs.x + obs.width));
    float closestY = std::max(obs.y, std::min(ball.y, obs.y + obs.height));
    float dx = ball.x - closestX;
    float dy = ball.y - closestY;
    float dist2 = dx * dx + dy * dy;

    if (dist2 < ball.radius * ball.radius)
    {
        float dist = std::sqrt(dist2);
        float penetration = ball.radius - dist;

        if (dist > 0)
        {
            penetrationX = dx / dist * penetration;
            penetrationY = dy / dist * penetration;
        }
        else
        {
            penetrationX = ((rand() % 2) ? penetration : -penetration);
            penetrationY = ((rand() % 2) ? penetration : -penetration);
        }

        return true;
    }

    return false;
}

void manyBallScene::resolve_ball_obstacle_collision(Ball &ball, const Obstacle &obs)
{
    float px = 0.0f, py = 0.0f;
    const float CORRECTION = 0.8f;
    if (ball_obstacle_collision(ball, obs, px, py))
    {
        ball.x += px * CORRECTION;
        ball.y += py * CORRECTION;

        float nx = px, ny = py;
        float len = std::sqrt(nx * nx + ny * ny);
        if (len > 0)
        {
            nx /= len;
            ny /= len;
            float dot = ball.vx * nx + ball.vy * ny;
            ball.vx -= 2.0f * dot * nx;
            ball.vy -= 2.0f * dot * ny;
            ball.angularVel += (ball.vx + ball.vy) * 0.1f;
            limit_velocity(ball);
        }
    }
}

void manyBallScene::resolve_ball_collision(Ball &b1, Ball &b2)
{
    float dx = b2.x - b1.x;
    float dy = b2.y - b1.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist == 0 || dist >= b1.radius + b2.radius)
        return;

    float depth = b1.radius + b2.radius - dist;
    float nx = dx / dist;
    float ny = dy / dist;

    float sepX = nx * depth * 0.5f;
    float sepY = ny * depth * 0.5f;

    if (!b1.isDragged)
    {
        b1.x -= sepX;
        b1.y -= sepY;
    }
    if (!b2.isDragged)
    {
        b2.x += sepX;
        b2.y += sepY;
    }

    float dvx = b2.vx - b1.vx;
    float dvy = b2.vy - b1.vy;
    float velNorm = dvx * nx + dvy * ny;
    if (velNorm > 0)
        return;

    float impulse = -2 * velNorm / 2;

    if (!b1.isDragged)
    {
        b1.vx -= impulse * nx;
        b1.vy -= impulse * ny;
        limit_velocity(b1);
    }
    if (!b2.isDragged)
    {
        b2.vx += impulse * nx;
        b2.vy += impulse * ny;
        limit_velocity(b2);
    }

    if (!b1.isDragged)
        b1.angularVel += (b1.vx + b1.vy) * 0.05f;
    if (!b2.isDragged)
        b2.angularVel += (b2.vx + b2.vy) * 0.05f;
}

bool manyBallScene::is_obstacle_overlapping(const Obstacle &new_obs, const std::vector<Obstacle> &existing) const
{
    for (const auto &obs : existing)
    {
        if (new_obs.x < obs.x + obs.width &&
            new_obs.x + new_obs.width > obs.x &&
            new_obs.y < obs.y + obs.height &&
            new_obs.y + new_obs.height > obs.y)
        {
            return true; // 重疊
        }
    }
    return false; // 不重疊
}
bool manyBallScene::is_ball_overlapping(const Ball &new_ball, const std::vector<Ball> &existing_balls, const std::vector<Obstacle> &obstacles) const
{
    // 檢查與其他球的重疊
    for (const auto &ball : existing_balls)
    {
        float dx = new_ball.x - ball.x;
        float dy = new_ball.y - ball.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < new_ball.radius + ball.radius)
        {
            return true; // 重疊
        }
    }
    // 檢查與障礙物的重疊
    for (const auto &obs : obstacles)
    {
        float closestX = std::max(obs.x, std::min(new_ball.x, obs.x + obs.width));
        float closestY = std::max(obs.y, std::min(new_ball.y, obs.y + obs.height));
        float dx = new_ball.x - closestX;
        float dy = new_ball.y - closestY;
        if (dx * dx + dy * dy < new_ball.radius * new_ball.radius)
        {
            return true; // 重疊
        }
    }
    return false; // 不重疊
}

void manyBallScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}