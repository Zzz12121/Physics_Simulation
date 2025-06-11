#include <allegro5/base.h>
#include <cmath>
#include <string>
#include "Engine/Group.hpp"
#include "Bullet/Landmine.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "UpGradeButton.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Spike.hpp"
const int UpGradeButton::Price = 10;
UpGradeButton::UpGradeButton(float x, float y) : Turret("play/tower-base.png", "play/turret-3.png", x, y, 100, Price, 100) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    hp = 5;
}
void UpGradeButton::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    Engine::Point diff2 = Engine::Point(-cos(Rotation - ALLEGRO_PI / 2), -sin(Rotation - ALLEGRO_PI / 2));
    Engine::Point diff3 = Engine::Point(cos(Rotation - ALLEGRO_PI ), sin(Rotation - ALLEGRO_PI));
    Engine::Point diff4 = Engine::Point(-cos(Rotation - ALLEGRO_PI ), -sin(Rotation - ALLEGRO_PI));


    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new Landmine(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new Landmine(Position + normalized * 36 + normal * 6, diff2, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new Landmine(Position + normalized * 36 + normal * 6, diff3, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new Landmine(Position + normalized * 36 + normal * 6, diff4, rotation, this));
    AudioHelper::PlayAudio("explosion.wav");
}/*
int UpGradeButton::sppike(){
    PlayScene *scene = getPlayScene();
    
    for(auto &it : scene->EnemyGroup->GetObjects()){
        if(it->){    
        Engine::Point diff = it->Position - Position;
            if(diff.x<2 || diff.y < 2){
                return 0;
            }
        }
        return 1;
    }
}*/