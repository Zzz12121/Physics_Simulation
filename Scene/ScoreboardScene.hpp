#ifndef ScoreboardScene_HPP
#define ScoreboardScene_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class ScoreboardScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    ScoreboardScene();
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
    void LoadEntries();
    void SortEntries();
    std::vector<Engine::Label*> entryNameLabels;
    std::vector<Engine::Label*> entryScoreLabels;
    Engine::Label* pageInfoLabel;
    struct Entry { 
        std::string name; 
        int score; 
        std::string datetime; 
    };
    std::vector<Entry> entries;
    void RefreshPage();
    int currentPage =0;
    int entriesPerPage = 5;
};

#endif   // ScoreboardScene_HPP
