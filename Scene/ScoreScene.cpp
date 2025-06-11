/*#include <fstream>
#include <ctime>
#include <algorithm>
#include "ScoreScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

ScoreScene::ScoreScene(int score) : score(score) {}

void ScoreScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    
    AddNewObject(new Engine::Label("遊戲結束！", "pirulen.ttf", 48, halfW, halfH - 200, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("請輸入您的名字：", "pirulen.ttf", 24, halfW, halfH - 100, 255, 255, 255, 255, 0.5, 0.5));
    
    
    inputBox = new Engine::InputTextBox("pirulen.ttf", 24, halfW, halfH - 50, 300, 40);
    AddNewControlObject(inputBox);
    
    
    AddNewObject(new Engine::Label("您的分數：" + std::to_string(score), "pirulen.ttf", 24, halfW, halfH, 255, 255, 255, 255, 0.5, 0.5));
    
    
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 100, halfH + 300, 200, 80);
    btn->SetOnClickCallback([this]() {
        if (!inputBox->GetText().empty()) {
            // 保存分數
            playerName = inputBox->GetText();
            time_t now = time(0);
            char* dt = ctime(&now);
            std::string dateTime(dt);
            dateTime = dateTime.substr(0, dateTime.length()-1);
            
            highScores.push_back({playerName + " (" + dateTime + ")", score});
            std::sort(highScores.begin(), highScores.end(), 
                [](const auto& a, const auto& b) { return a.second > b.second; });
            
            if (highScores.size() > 10) {
                highScores.resize(10);
            }
            
            SaveHighScores();
        }
        Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    });
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("返回", "pirulen.ttf", 24, halfW, halfH + 340, 0, 0, 0, 255, 0.5, 0.5));
    
   
    LoadHighScores();
}

void ScoreScene::Terminate() {
    IScene::Terminate();
}

void ScoreScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
}

void ScoreScene::Draw() const {
    IScene::Draw();
    DrawHighScores();
}

void ScoreScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
}

void ScoreScene::LoadHighScores() {
    std::ifstream file("Scoreboard.txt");
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t lastSpace = line.find_last_of(' ');
        if (lastSpace != std::string::npos) {
            std::string name = line.substr(0, lastSpace);
            int score = std::stoi(line.substr(lastSpace + 1));
            highScores.push_back({name, score});
        }
    }
    file.close();
}

void ScoreScene::SaveHighScores() {
    std::ofstream file("Scoreboard.txt");
    for (const auto& score : highScores) {
        file << score.first << " " << score.second << "\n";
    }
    file.close();
}

void ScoreScene::DrawHighScores() const {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    
   
    Engine::Label("高分榜", "pirulen.ttf", 32, halfW, halfH + 100, 255, 255, 255, 255, 0.5, 0.5).Draw();
    
   
    for (size_t i = 0; i < highScores.size() && i < 10; i++) {
        std::string text = std::to_string(i + 1) + ". " + highScores[i].first + " - " + std::to_string(highScores[i].second);
        Engine::Label(text, "pirulen.ttf", 20, halfW, halfH + 150 + i * 30, 255, 255, 255, 255, 0.5, 0.5).Draw();
    }
} */