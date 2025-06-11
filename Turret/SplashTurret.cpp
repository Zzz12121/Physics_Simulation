#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/Fire.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "SplashTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Spike.hpp"
#include "Engine/Group.hpp"
const int SplashTurret::Price = 100;
SplashTurret::SplashTurret(float x, float y) : Turret("play/tower-base.png", "play/turret-fire.png", x, y, 600, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void SplashTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    Engine::Point diff2 = Engine::Point(-cos(Rotation - ALLEGRO_PI / 2), -sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new Fire(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new Fire(Position + normalized * 36 + normal * 6, diff2, rotation, this));
    AudioHelper::PlayAudio("missile.wav");
}/*
int SplashTurret::sppike(){
    PlayScene *scene = getPlayScene();
    
    for(auto &it : scene->EnemyGroup->GetObjects()){
        Engine::Point diff = it->Position - Position;
        if(diff.x<2 || diff.y < 2){
            return 0;
        }
        return 1;
    }
}*/