//
// Created by Hsuan on 2024/4/10.
//

#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"
#include "waterBallScene.hpp"
#include "cutBagScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO HACKATHON-2 (1/3): You can imitate the 2 files: 'StartScene.hpp', 'StartScene.cpp' to implement your SettingsScene.
void StartScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton* btn;

    AddNewObject(new Engine::Label("Physics Simulator", "pirulen.ttf", 120,
        halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        halfW - 200, halfH / 2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48,
        halfW, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));


    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        halfW + 300, halfH / 2 + 100, 400, 100);  // 根據你想放的位置調整座標
    btn->SetOnClickCallback(std::bind(&StartScene::CutBagOnClick, this, 6));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("CutBag", "pirulen.ttf", 40,
        halfW + 500 , halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

/*
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        halfW - 200, halfH * 3 / 2 - 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48,
        halfW, halfH * 3 / 2 - 150, 0, 0, 0, 255, 0.5, 0.5));
*/
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        halfW - 200, halfH * 3 / 2 - 80, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::WaterBallOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("WaterBall", "pirulen.ttf", 40,
        halfW, halfH * 3 / 2 - 30, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        halfW - 200, halfH * 3 / 2 + 40, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::DoublePendulumOnClick, this, 4));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("DoublePendulum", "pirulen.ttf", 28,
        halfW, halfH * 3 / 2 + 90, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png",
        halfW - 700, halfH / 2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::ManyBallOnClick, this, 5));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Collision", "pirulen.ttf", 40,
        halfW-500, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

}

void StartScene::Terminate() {
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}/*
void StartScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}*/
void StartScene::WaterBallOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("waterBall");
}

void StartScene::DoublePendulumOnClick(int stage) {
   
    Engine::GameEngine::GetInstance().ChangeScene("pendulum");
}

void StartScene::ManyBallOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("many-ball");
}

void StartScene::CutBagOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("cut-bag");
}
