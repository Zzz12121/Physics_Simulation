#include <allegro5/base.h>
#include <cmath>
#include <string>
#include "Engine/Group.hpp"
#include "Bullet/LaserBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "LaserTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Spike.hpp"
#include "Engine/Collider.hpp"
const int LaserTurret::Price = 200;
LaserTurret::LaserTurret(float x, float y) : Turret("play/tower-base.png", "play/turret-2.png", x, y, 300, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void LaserTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
    AudioHelper::PlayAudio("laser.wav");

}
/*
int LaserTurret::sppike(){
    for (auto it = Group*PlayScene::EnemyGroup->GetObjects().begin(); it != EnemyGroup->GetObjects().end(); it++){
        auto t = dynamic_cast<Enemy *>(*it);
        if(Engine::Collider::IsCircleOverlap(t->Position, 5, this->Position, 5)){
            return 0;
        }
        return 1;
    }
    
    
}
int LaserTurret::sppike(){
    PlayScene *scene = getPlayScene();
    
    for(auto &it : scene->EnemyGroup->GetObjects()){
        auto t = dynamic_cast<Enemy *>(it);
        if(t->)
            Engine::Point diff = t->Position - Position;
            if(diff.x<2 || diff.y < 2){
                return 0;
            }
        return 1;
    }
}
    for (auto it = Engine::Group *PlayScene::EnemyGroup->GetObjects().begin(); it != Group * PlayScene::EnemyGroup->GetObjects().end(); it++){
        auto t = dynamic_cast<Enemy *>(*it);
        if(Engine::Collider::IsCircleOverlap(t->Position, 5, this->Position, 5)){
            return 0;
        }
        return 1;
    }*/