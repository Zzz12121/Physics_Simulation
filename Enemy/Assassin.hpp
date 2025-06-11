#ifndef ASSASSIN_HPP
#define ASSASSIN_HPP

#include "Enemy.hpp"
#include <vector>

class Assassin : public Enemy {
private:
    float normalSpeed;       
    float boostMultiplier;   
    float triggerRadius;     

public:

    Assassin(float x, float y,
             float speed = 60.0f,
             float hp = 50.0f,
             int money = 15,
             float triggerRadius = 100.0f,
             float boostMultiplier = 2.0f);

    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif // ASSASSIN_HPP
