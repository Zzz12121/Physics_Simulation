#include "Enemy/Assassin.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include <cmath>

Assassin::Assassin(float x, float y,
                   float speed,
                   float hp,
                   int money,
                   float triggerRadius,
                   float boostMultiplier)
    : Enemy("play/assassin.png", x, y, /*radius=*/20, speed, hp, money),
      normalSpeed(speed),
      boostMultiplier(boostMultiplier),
      triggerRadius(triggerRadius)
{
    
    Tint.a = 128;
}

void Assassin::Update(float deltaTime) {
  
    auto scene = getPlayScene();
    bool nearTower = false;
    for (auto obj : scene->TowerGroup->GetObjects()) {
        Turret* t = dynamic_cast<Turret*>(obj);
        if (!t) continue;
        float dx = t->Position.x - Position.x;
        float dy = t->Position.y - Position.y;
        if (dx*dx + dy*dy <= triggerRadius * triggerRadius*2) {
            nearTower = true;
            break;
        }
    }

  
    speed = nearTower ? normalSpeed * 10*1.5 : normalSpeed*2;

    Tint.a = nearTower ? 255 : 128;


    Enemy::Update(deltaTime);
}

void Assassin::Draw() const {

    Enemy::Draw();

}

