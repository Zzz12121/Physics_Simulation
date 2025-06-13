#ifndef MANYBALLSCENE_HPP
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