#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <ctime>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

ScoreboardScene::ScoreboardScene()
    : currentPage(0)
    , entriesPerPage(5)

    , pageInfoLabel(nullptr)
{
}

void ScoreboardScene::LoadEntries() {
    entries.clear();
    //std::ifstream ifs("Resource/scoreboard.txt");
    std::ifstream ifs("C:/NTHU/I2P/2025_I2P2_TowerDefense-main/2025_I2P2_TowerDefense-main/Resource/scoreboard.txt");

    if (!ifs.is_open()) return;
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream ss(line);
        Entry e;
        if (ss >> e.name >> e.score) {
            std::getline(ss, e.datetime);
            //if (!e.datetime.empty() && e.datetime[0] == ' ')
              //  e.datetime.erase(0,1);
            entries.push_back(e);
        }
    }
}
void ScoreboardScene::SortEntries() {
    std::sort(entries.begin(), entries.end(),
        [](auto& a, auto& b){
            return a.score > b.score;
        });
}

void ScoreboardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
//back
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
    //next
        btn = new Engine::ImageButton(
        "stage-select/dirt.png","stage-select/floor.png",
        halfW + 200, halfH + 220, 200,80
    );
    btn->SetOnClickCallback([&](){
        int totalPages = (entries.size() + entriesPerPage - 1) / entriesPerPage;
        if (currentPage < totalPages - 1) {
            currentPage++;
            //Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
            RefreshPage();
        }
    });
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(
        "NEXT PAGE","pirulen.ttf",23,
        halfW + 300, halfH + 260,0,0,0,255,0.5f,0.5f
    ));
    //prev
    btn = new Engine::ImageButton(
        "stage-select/dirt.png","stage-select/floor.png",
        halfW - 400, halfH + 220, 200,80
    );
    btn->SetOnClickCallback([&](){
        if (currentPage > 0) {
            --currentPage;
            //Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
            RefreshPage();
        }
    });
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(
        "PREV PAGE","pirulen.ttf",23,
        halfW - 300, halfH + 260,0,0,0,255,0.5f,0.5f
    ));
    //NAME
    AddNewObject(new Engine::Label(
        "SCOREBOARD", "pirulen.ttf", 64,
        halfW, halfH/4, 0,255,0,255, 0.5f,0.5f
    ));
    entryNameLabels .clear();
    entryScoreLabels.clear();
    LoadEntries();
    SortEntries();
    RefreshPage();
}
void ScoreboardScene::RefreshPage(){
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
    int start = currentPage * entriesPerPage;
    int end   = std::min(start + entriesPerPage, (int)entries.size());
    for(int i = start; i<end;i++){
        AddNewObject(new Engine::Label(entries[i].name, "pirulen.ttf", 48,
            halfW - 250, halfH/2 + 80 + (i-start) * 60,
            0,255,0,255, 1.0f,0.5f));
        AddNewObject(new Engine::Label(std::to_string(entries[i].score), "pirulen.ttf", 48,
            halfW + 200, halfH/2 + 80 + (i-start) * 60,
            0,255,0,255, 1.0f,0.5f));
    }

}
void ScoreboardScene::Terminate() {
    IScene::Terminate();
}
void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-start");
}
void ScoreboardScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void ScoreboardScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}





