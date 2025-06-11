#include <allegro5/base.h>
#include <random>
#include <string>
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Enemy/Spike.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/SplashTurret.hpp"
#include "Turret/UpGradeButton.hpp"
#include "Turret/Turret.hpp"
//
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>


#include <vector>

#include "Bullet/Bullet.hpp"
#include "Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"

#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"

#include "Engine/Resources.hpp" 
#include <allegro5/allegro_font.h>    // 一定要加
#include <allegro5/allegro_ttf.h> 
//
int enemywinn =0;

Spike::Spike(int x, int y)
    : Enemy("play/enemy-3.png", x, y, 20, 20, 10000, 50),
      head("play/Spike.png", x, y,64,64), targetRotation(0) {
}
void Spike::Draw() const {
    Enemy::Draw();
    head.Draw();
}
void Spike::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    head.Position = Position;
    // Choose arbitrary one.
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist(0.0f, 4.0f);
    float rnd = dist(rng);
    if (rnd < deltaTime) {
        // Head arbitrary rotation.
        std::uniform_real_distribution<> distRadian(-ALLEGRO_PI, ALLEGRO_PI);
        targetRotation = distRadian(rng);
    }
    head.Rotation = (head.Rotation + deltaTime * targetRotation) / (1 + deltaTime);

    //
    auto scene = getPlayScene();
    float hitRadius = 80.0f;


    auto towerSnapshot = scene->TowerGroup->GetObjects();

    for (auto obj : towerSnapshot) {
        Turret* turret = dynamic_cast<Turret*>(obj);
        if (!turret) continue;

        float dx = turret->Position.x - Position.x;
        float dy = turret->Position.y - Position.y;
        float dist = std::sqrt(dx*dx + dy*dy);

        if (dist <= hitRadius) {
            bool dead = turret->TakeDamage(1);
            if (dead) {
                
                int gx = int(turret->Position.x / PlayScene::BlockSize);
                int gy = int(turret->Position.y / PlayScene::BlockSize);
                scene->mapState[gy][gx] = PlayScene::TILE_DIRT;

    
                auto objIter = turret->GetObjectIterator();
                scene->TowerGroup->RemoveObject(objIter);
                //Turret::OnExplode();
                getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(turret->Position.x, turret->Position.y));
                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
                std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
                for (int i = 0; i < 10; i++) {
                    // Random add 10 dirty effects.
                    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
                }
                enemywinn++;
                if(enemywinn==5)
                    Engine::GameEngine::GetInstance().ChangeScene("lose");
            }
        }
    }
    /*
    //
    auto scene = getPlayScene();

    float destroyRadius = this->CollisionRadius;


    for (auto it = scene->TowerGroup->GetObjects().begin();
         it != scene->TowerGroup->GetObjects().end(); ) {
        Turret *turret = dynamic_cast<Turret*>(*it);
        ++it;  
        if (!turret) continue;

        
        int gx = int(turret->Position.x / PlayScene::BlockSize);
        int gy = int(turret->Position.y / PlayScene::BlockSize);

       
        float dist = (turret->Position - this->Position).Magnitude();
        if (dist <= destroyRadius + turret->CollisionRadius) {
            
            scene->mapState[gy][gx] = PlayScene::TILE_DIRT;
           
            auto objIter = turret->GetObjectIterator();  
            scene->TowerGroup->RemoveObject(objIter);
          
        }
    }*/

}


