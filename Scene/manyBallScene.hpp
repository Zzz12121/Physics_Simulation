/*#ifndef MANYBALLSCENE_HPP
#define MANYBALLSCENE_HPP

#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <vector>
#include "UI/Component/Label.hpp"

struct Ball
{
    float x, y;
    float vx, vy;
    float radius;
    float angularVel;
    bool isDragged;
    ALLEGRO_COLOR color;
};

struct Obstacle
{
    float x, y;
    float width, height;
    ALLEGRO_COLOR color;
};

class manyBallScene : public Engine::IScene
{
public:
    explicit manyBallScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;

    void OnMouseDown(int button, int x, int y) override;
    void OnMouseUp(int button, int x, int y) override;
    void OnMouseMove(int x, int y) override;
    void BackOnClick(int stage);

private:
    std::vector<Ball> balls;
    std::vector<Obstacle> obstacles;
    Engine::Label* frictionLabel = nullptr;
    Ball *draggedBall = nullptr;
    float mouseX = 0, mouseY = 0;

    void limit_velocity(Ball &ball) const;
    bool ball_obstacle_collision(const Ball &ball, const Obstacle &obs, float &penetrationX, float &penetrationY) const;
    void resolve_ball_obstacle_collision(Ball &ball, const Obstacle &obs);
    void resolve_ball_collision(Ball &b1, Ball &b2);
    ALLEGRO_COLOR random_color() const;
    Obstacle generate_random_obstacle() const;
    bool is_obstacle_overlapping(const Obstacle &new_obs, const std::vector<Obstacle> &existing) const;
    bool is_ball_overlapping(const Ball &new_ball, const std::vector<Ball> &existing_balls, const std::vector<Obstacle> &obstacles) const;
};

#endif // MANYBALLSCENE_HPP
*/
#ifndef MANYBALLSCENE_HPP
#define MANYBALLSCENE_HPP

#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <vector>
#include <string>
#include "UI/Component/Label.hpp"

struct Ball
{
    float x, y;
    float vx, vy;
    float radius;
    float angularVel;
    bool isDragged;
    ALLEGRO_COLOR color;
};

struct Obstacle
{
    float x, y;
    float width, height;
    ALLEGRO_COLOR color;
};

class manyBallScene : public Engine::IScene
{
public:
    explicit manyBallScene() = default;

    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;

    void OnMouseDown(int button, int x, int y) override;
    void OnMouseUp(int button, int x, int y) override;
    void OnMouseMove(int x, int y) override;
    void OnKeyDown(int keycode) override; // ★ 新增

    void BackOnClick(int stage);

private:
    // 物件
    ALLEGRO_BITMAP *miniMap = nullptr; // 小地圖緩衝
    static constexpr int MINI_W = 240; // 小地圖寬
    static constexpr int MINI_H = 140; // 小地圖高
    std::vector<Ball> balls;
    std::vector<Obstacle> obstacles;

    // 操作狀態
    bool editMode = false;          // 是否為地圖編輯模式 (E 切換)
    Ball *draggedBall = nullptr;    // 被拖曳的球
    Obstacle *draggedObs = nullptr; // 被拖曳的牆
    float mouseX = 0, mouseY = 0;

    // 介面元素
    Engine::Label *frictionLabel = nullptr;

    // 功能函式
    void limit_velocity(Ball &ball) const;
    bool ball_obstacle_collision(const Ball &ball, const Obstacle &obs,
                                 float &penetrationX, float &penetrationY) const;
    void resolve_ball_obstacle_collision(Ball &ball, const Obstacle &obs);
    void resolve_ball_collision(Ball &b1, Ball &b2);

    ALLEGRO_COLOR random_color() const;
    Obstacle generate_random_obstacle() const;
    bool is_obstacle_overlapping(const Obstacle &new_obs,
                                 const std::vector<Obstacle> &existing) const;
    bool is_ball_overlapping(const Ball &new_ball,
                             const std::vector<Ball> &existing_balls,
                             const std::vector<Obstacle> &obstacles) const;

    // 地圖 I/O
    void load_map(const std::string &path);
    void save_map(const std::string &path) const;

    // Hit-test
    Obstacle *hit_test_obstacle(float px, float py);
};

#endif // MANYBALLSCENE_HPP