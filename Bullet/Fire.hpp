#ifndef FIRE_HPP
#define FIRE_HPP

#include "Bullet2.hpp"    // ← 如果 Fire.hpp 在 Bullet/ 目录下，这里 ok
// 或者如果 Fire.hpp 在更上层：
// #include "Bullet/Bullet2.hpp"

class Enemy;
class Turret;

namespace Engine { struct Point; }

class Fire : public Bullet2 {
public:
    explicit Fire(Engine::Point position,
                  Engine::Point forwardDirection,
                  float rotation,
                  Turret *parent);

    void OnExplode(Enemy *enemy) override;
};

#endif // FIRE_HPP
