#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Bullet/Bullet.hpp"
#include "Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"

#include "Engine/Resources.hpp" 
#include <allegro5/allegro_font.h>    
#include <allegro5/allegro_ttf.h>     

PlayScene *Enemy::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Enemy::OnExplode() {
    getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
    for (int i = 0; i < 10; i++) {
        // Random add 10 dirty effects.
        getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
    }
}
Enemy::Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money) : Engine::Sprite(img, x, y), speed(speed), hp(hp), money(money) {
    CollisionRadius = radius;
    reachEndTime = 0;
}
void Enemy::Hit(float damage) {
    hp -= damage;
    if (hp <= 0) {
        OnExplode();
        // Remove all turret's reference to target.
        for (auto &it : lockedTurrets)
            it->Target = nullptr;
        for (auto &it : lockedBullets)
            it->Target = nullptr;
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}
void Enemy::UpdatePath(const std::vector<std::vector<int>> &mapDistance) {
    int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    if (x < 0) x = 0;
    if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
    if (y < 0) y = 0;
    if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
    Engine::Point pos(x, y);
    int num = mapDistance[y][x];
    if (num == -1) {
        num = 0;
        Engine::LOG(Engine::ERROR) << "Enemy path finding error";
    }
    path = std::vector<Engine::Point>(num + 1);
    while (num != 0) {
        std::vector<Engine::Point> nextHops;
        for (auto &dir : PlayScene::directions) {
            int x = pos.x + dir.x;
            int y = pos.y + dir.y;
            if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
                continue;
            nextHops.emplace_back(x, y);
        }
        // Choose arbitrary one.
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
        pos = nextHops[dist(rng)];
        path[num] = pos;
        num--;
    }
    path[0] = PlayScene::EndGridPoint;
}
void Enemy::Update(float deltaTime) {
    // Pre-calculate the velocity.
    float remainSpeed = speed * deltaTime;
    while (remainSpeed != 0) {
        if (path.empty()) {
            // Reach end point.
            Hit(hp);
            getPlayScene()->Hit();
            reachEndTime = 0;
            return;
        }
        Engine::Point target = path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
        Engine::Point vec = target - Position;
        // Add up the distances:
        // 1. to path.back()
        // 2. path.back() to border
        // 3. All intermediate block size
        // 4. to end point
        reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
        Engine::Point normalized = vec.Normalize();
        if (remainSpeed - vec.Magnitude() > 0) {
            Position = target;
            path.pop_back();
            remainSpeed -= vec.Magnitude();
        } else {
            Velocity = normalized * remainSpeed / deltaTime;
            remainSpeed = 0;
        }
    }
    Rotation = atan2(Velocity.y, Velocity.x);
    Sprite::Update(deltaTime);
}
/*
void Enemy::Draw() const {
    // 1) 先把精灵本体画出来
    Sprite::Draw();
    if (PlayScene::DebugMode) {
        // Draw collision radius.
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    }
    // 2) 构造要显示的血量文本（只显示当前 hp）
    std::string hpText = std::to_string(static_cast<int>(hp));
    ALLEGRO_FONT* font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 16).get();
    // 3) 从 Resources 拿字体（跟 InputBox 里一样）
    //ALLEGRO_FONT* font = Resources::GetInstance().GetFont("pirulen.ttf", 16).get();
    if (!font) return;

    // 4) 文字要画的位置：X 轴就用精灵中心，Y 轴在碰撞半径上方留 5px
    float px = Position.x;
    float py = Position.y - CollisionRadius;

    // 5) 画文字（红色，水平居中对齐）
    al_draw_text(
        font,
        al_map_rgb(255, 0, 0),
        px, py-20,
        ALLEGRO_ALIGN_CENTER,
        hpText.c_str()
    );
}*/


void Enemy::Draw() const {
   
    Sprite::Draw();

    
    float hpRatio = hp / max_hp;
    if (hpRatio < 0) hpRatio = 0;
    if (hpRatio > 1) hpRatio = 1;


    const float barW = 40.0f;
    const float barH = 6.0f;
    const float offsetY = CollisionRadius + 8.0f;

    float cx = Position.x;  
    float cy = Position.y;  

 
    float bx = cx - barW * 0.5f;
    float by = cy - offsetY - barH;


    al_draw_filled_rectangle(
        bx,       by,
        bx + barW,by + barH,
        al_map_rgb(100, 100, 100)
    );

  
    al_draw_filled_rectangle(
        bx,          by,
        bx + barW * hpRatio,
        by + barH,
        al_map_rgb(0, 200, 0)
    );

   
    al_draw_rectangle(
        bx,       by,
        bx + barW,by + barH,
        al_map_rgb(0, 0, 0),
        1.0f
    );

   
    std::string hpText = std::to_string(static_cast<int>(hp));
    ALLEGRO_FONT* font = Engine::Resources::GetInstance()
                             .GetFont("pirulen.ttf", 14)
                             .get();
    if (font) {
        al_draw_text(
            font,
            al_map_rgb(255, 255, 255),
            cx, by - 2,                   
            ALLEGRO_ALIGN_CENTER,
            hpText.c_str()
        );
    }
}