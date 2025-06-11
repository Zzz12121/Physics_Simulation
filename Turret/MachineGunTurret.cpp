#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "MachineGunTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Spike.hpp"
#include "Engine/Group.hpp"
const int MachineGunTurret::Price = 50;
MachineGunTurret::MachineGunTurret(float x, float y)
    : Turret("play/tower-base.png", "play/turret-1.png", x, y, 200, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void MachineGunTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("gun.wav");
}/*
int MachineGunTurret::sppike(){
    PlayScene *scene = getPlayScene();
    
    for(auto &it : scene->EnemyGroup->GetObjects()){
        Engine::Point diff = it->Position - Position;
        if(diff.x<2 || diff.y < 2){
            return 0;
        }
        return 1;
    }
}*/