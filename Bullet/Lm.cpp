/*#include "Bullet/Lm.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include <cmath>



#include "Engine/Point.hpp"

PlayScene *Lm::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Lm::Lm(float x, float y,
                   float armTime,
                   float triggerRadius,
                   int damage)
  : Engine::Sprite("play/tower-base.png", x, y),  
    armTime(armTime),
    timer(0),
    triggerRadius(triggerRadius),
    damage(damage),
    armed(false)
{

    Velocity = Engine::Point(0,0);
  
    getPlayScene()->GroundEffectGroup->AddNewObject(this);
}

void Lm::Update(float deltaTime) {

    if (!armed) {
        timer += deltaTime;
        if (timer >= armTime) {
            armed = true;

            //AudioHelper::PlayAudio("arm.wav");
        }
        return;
    }


    auto scene = getPlayScene();
    for (auto obj : scene->EnemyGroup->GetObjects()) {
        Enemy* e = dynamic_cast<Enemy*>(obj);
        if (!e) continue;
        float dx = e->Position.x - Position.x;
        float dy = e->Position.y - Position.y;
        if (dx*dx + dy*dy <= triggerRadius*triggerRadius) {
            Explode();
            return;  
        }
    }
}


void Lm::Draw() const {
    // 先取得底层的 bitmap
    ALLEGRO_BITMAP* bmp = Engine::GameEngine::GetInstance()
                             .GetBitmap("play/tower-base.png")
                             .get();  

    if (!armed) {
        // 引信未就绪——灰色调
        al_draw_tinted_bitmap(
            bmp,
            al_map_rgba_f(0.5f, 0.5f, 0.5f, 1.0f),
            Position.x - GetBitmapWidth()  / 2,
            Position.y - GetBitmapHeight() / 2,
            0
        );
    }
    else {
        // 引信就绪——按正常 Sprite 画法
        Sprite::Draw();
    }
}

void Lm::Explode() {
    auto scene = getPlayScene();

    scene->EffectGroup->AddNewObject(
        new ExplosionEffect(Position.x, Position.y)
    );
    AudioHelper::PlayAudio("explosion.wav");


    float r2 = triggerRadius * 1.5f;  
    float r2sq = r2 * r2;
    for (auto obj : scene->EnemyGroup->GetObjects()) {
        Enemy* e = dynamic_cast<Enemy*>(obj);
        if (!e) continue;
        float dx = e->Position.x - Position.x;
        float dy = e->Position.y - Position.y;
        if (dx*dx + dy*dy <= r2sq) {
            e->Hit(damage);
        }
    }

 
    scene->GroundEffectGroup->RemoveObject(objectIterator);
}*/


#include "Bullet/Lm.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "Engine/AudioHelper.hpp"
#include "UI/Animation/ExplosionEffect.hpp"  // 如果你想继续保留爆炸动画可以保留这行
#include <cmath>

#include "Bullet/Lm.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"

#include "Engine/GameEngine.hpp"
#include <cmath>



#include "Engine/Point.hpp"

PlayScene *Lm::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Lm::Lm(float x, float y,
       float armTime,
       float triggerRadius,
       int damage)
  : Engine::Sprite("play/landmine.png", x, y)  // 地雷贴图
  , armTime(armTime)
  , timer(0)
  , triggerRadius(triggerRadius)
  , damage(damage)
  , armed(false)
{
    // 放到地面效果组里
    getPlayScene()->GroundEffectGroup->AddNewObject(this);
}

void Lm::Update(float deltaTime) {
    // 1) 引信倒计时
    if (!armed) {
        timer += deltaTime;
        if (timer >= armTime) {
            armed = true;
            // 如果不需要任何音效，这里留空
        }
        return;
    }

    // 2) 已引信 → 检测周围敌人
    auto scene = getPlayScene();
    for (auto obj : scene->EnemyGroup->GetObjects()) {
        Enemy* e = dynamic_cast<Enemy*>(obj);
        if (!e) continue;
        float dx = e->Position.x - Position.x;
        float dy = e->Position.y - Position.y;
        if (dx*dx + dy*dy <= triggerRadius*triggerRadius) {
            Explode();
            return;
        }
    }
}

void Lm::Draw() const {
    // 简单绘制：不管 armed 与否，都只画贴图
    Sprite::Draw();
}

void Lm::Explode() {
    auto scene = getPlayScene();

    // 如果你想加一个简单的爆炸动画，可以在这里：
    // scene->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));

    // （可选）播放爆炸音效
    // Engine::AudioHelper::PlayAudio("explosion.wav");

    // 对范围内所有敌人造成伤害
    float r2 = triggerRadius * 1.5f;
    float r2sq = r2 * r2;
    for (auto obj : scene->EnemyGroup->GetObjects()) {
        Enemy* e = dynamic_cast<Enemy*>(obj);
        if (!e) continue;
        float dx = e->Position.x - Position.x;
        float dy = e->Position.y - Position.y;
        if (dx*dx + dy*dy <= r2sq) {
            e->Hit(damage);
        }
    }

    // 最后，把自己从 GroundEffectGroup 移除 → 下帧就不再 Draw/Update
    scene->GroundEffectGroup->RemoveObject(objectIterator);
}