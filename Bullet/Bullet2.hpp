#ifndef BULLET2_HPP
#define BULLET2_HPP
#include <string>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet2 : public Engine::Sprite {
protected:
    float speed;
    float damage;
    Turret *parent;
    PlayScene *getPlayScene();
    virtual void OnExplode(Enemy *enemy);

public:
    Enemy *Target = nullptr;
    explicit Bullet2(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void Update(float deltaTime) override;
};
#endif   // BULLET_HPP
