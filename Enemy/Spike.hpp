#ifndef Spike_HPP
#define Spike_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class Spike : public Enemy {
private:
    Sprite head;
    float targetRotation;

public:
    Spike(int x, int y);
    void Draw() const override;
    void Update(float deltaTime) override;
};
#endif   // Spike_HPP
