/*#pragma once
#include <string>
#include <vector>
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/InputTextBox.hpp"

class ScoreScene : public Engine::IScene {
private:
    int score;
    std::string playerName;
    Engine::InputTextBox* inputBox;
    std::vector<std::pair<std::string, int>> highScores;
    void LoadHighScores();
    void SaveHighScores();
    void DrawHighScores();
public:
    ScoreScene(int score);
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
}; */