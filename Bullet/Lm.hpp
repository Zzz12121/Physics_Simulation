#ifndef Lm_HPP
#define Lm_HPP

#include "Bullet/Bullet.hpp"
#include "Engine/Sprite.hpp"
#include <allegro5/allegro_audio.h>

class Lm : public Engine::Sprite {
private:
    float armTime;        // 引信时间（秒）
    float timer;          // 当前计时
    float triggerRadius;  // 触发半径
    int damage;           // 爆炸伤害
    bool armed;           // 是否已引信完毕

public:
    PlayScene *getPlayScene();
    // x, y 放置位置；armTime=2s；triggerRadius=32px；damage=50
    Lm(float x, float y,
             float armTime = 2.0f,
             float triggerRadius = 32.0f,
             int damage = 50);

    // 每帧更新：计时、检查触发条件
    void Update(float deltaTime) override;

    // 绘制：静态地雷贴图
    void Draw() const override;

private:
    // 执行爆炸
    void Explode();
};

#endif // Lm_HPP