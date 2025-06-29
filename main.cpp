// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/waterBallScene.hpp"
#include "Scene/doublePendulumScene.hpp"
#include "Scene/manyBallScene.hpp"
#include "Scene/cutBagScene.hpp"

int main(int argc, char **argv)
{
 Engine::LOG::SetConfig(true);
 Engine::GameEngine &game = Engine::GameEngine::GetInstance();

 // TODO HACKATHON-2 (2/3): Register Scenes here
 game.AddNewScene("start", new StartScene());
 game.AddNewScene("stage-select", new StageSelectScene());
 game.AddNewScene("waterBall", new waterBallScene());
 game.AddNewScene("pendulum", new doublePendulumScene());
 //game.AddNewScene("settings", new SettingsScene());
 game.AddNewScene("play", new PlayScene());
 game.AddNewScene("lose", new LoseScene());
 game.AddNewScene("win", new WinScene());
 game.AddNewScene("many-ball", new manyBallScene());
 game.AddNewScene("cut-bag", new cutBagScene());


 // TODO HACKATHON-1 (1/1): Change the start scene
 game.Start("start", 60, 1600, 832);
 return 0;
}