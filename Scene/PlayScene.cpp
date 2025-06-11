#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <allegro5/allegro_primitives.h>

#include "Enemy/Spike.hpp"
#include "Turret/Turret.hpp" 

#include "Enemy/Enemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/SplashTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/UpGradeButton.hpp"
#include "Bullet/Lm.hpp"  
#include "Enemy/Spike.hpp"    
#include "Enemy/Assassin.hpp"  
extern int enemywinn;  
// TODO HACKATHON-4 (1/3): Trace how the game handles keyboard input.
// TODO HACKATHON-4 (2/3): Find the cheat code sequence in this file.
// TODO HACKATHON-4 (3/3): When the cheat code is entered, a plane should be spawned and added to the scene.
// TODO HACKATHON-5 (1/4): There's a bug in this file, which crashes the game when you win. Try to find it.
// TODO HACKATHON-5 (2/4): The "LIFE" label are not updated when you lose a life. Try to fix it.
//my
/*static const int CheatCode[12] = {
    ALLEGRO_KEY_UP,ALLEGRO_KEY_UP,
    ALLEGRO_KEY_DOWN,ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT,ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_LEFT,ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B,ALLEGRO_KEY_A,
    ALLEGRO_KEY_LSHIFT,ALLEGRO_KEY_ENTER
};*/
//my
bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEY_B, ALLEGRO_KEY_ENTER
};
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 5;
    money = 5000;
    SpeedMult = 1;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    //my

    //my
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
    
    //my
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton(
        "stage-select/dirt.png","stage-select/floor.png",
        halfW + 500, halfH + 220, 200,80
    );
    btn->SetOnClickCallback([&](){

    });
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("shovel","pirulen.ttf",23,halfW + 600, halfH + 260,0,0,0,255,0.5f,0.5f));
    //AddNewObject(new Engine::Image("play/shovel.png", halfW + 600, halfH + 260, 0.5f, 0.5f,0.5f,0.5f));
    //my
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    // Reference: Bullet-Through-Paper
    if (SpeedMult == 0)
        deathCountDown = -1;
    else if (deathCountDown != -1)
        SpeedMult = 1;
    // Calculate danger zone.
    std::vector<float> reachEndTimes;
    for (auto &it : EnemyGroup->GetObjects()) {
        reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
    }
    // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto &it : reachEndTimes) {
        if (it <= DangerTime) {
            danger--;
            if (danger <= 0) {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown) {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                        deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
                }
                float alpha = pos / DangerTime;
                alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0) {
        AudioHelper::StopSample(deathBGMInstance);
        dangerIndicator->Tint.a = 0;
    }
    if (SpeedMult == 0)
        deathCountDown = -1;
    for (int i = 0; i < SpeedMult; i++) {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        if (enemyWaveData.empty()) {
            if (EnemyGroup->GetObjects().empty()) {
                // 切換到分數場景
                Engine::GameEngine::GetInstance().ChangeScene("win");
                return;
            }
            continue;
        }
        /*if (enemyWaveData.empty()) {
            if (EnemyGroup->GetObjects().empty()) {
                // Win: 切換到 WinScene 實例，直接 new 出場景
                Engine::GameEngine::GetInstance().ChangeScene(
                    new WinScene(&Engine::GameEngine::GetInstance())
                );
                return;  // 切換後立刻結束本次 Update，避免後續操作導致 crash
            }
            continue;
        }*/
        auto current = enemyWaveData.front();
        if (ticks < current.second)
            continue;
        ticks -= current.second;    
        enemyWaveData.pop_front();
        const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
        Enemy *enemy;
        switch (current.first) {
            case 1:
                EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            // TODO HACKATHON-3 (2/3): Add your new enemy here.
            case 2:
                EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            //     ...
            case 3:
                EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 4:
                EnemyGroup->AddNewObject(enemy = new Spike(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 5:
                EnemyGroup->AddNewObject(enemy = new Assassin(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            default:
                continue;
        }
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
    }
    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
    //my
    /*
            for (auto it = TowerGroup->GetObjects().begin(); it != TowerGroup->GetObjects().end(); ++it) {
                auto t = dynamic_cast<Turret *>(*it);
                if (t->sppike()) {
                    int tx = floor(t->Position.x / BlockSize);
                    int ty = floor(t->Position.y / BlockSize);
                    
                        
                        t->GetObjectIterator()->first = false;     
                        TowerGroup->RemoveObject(t->GetObjectIterator());
                        mapState[t->Position.y / BlockSize][t->Position.x/ BlockSize] = TILE_DIRT;             
                        break;
                    
                }
            }*/
    //my
    if (ruinedTurretLabel) {
        ruinedTurretLabel->Text = std::string("Ruined Turret: ")
                                + std::to_string(enemywinn) + std::string(" / 5");
    }
}
void PlayScene::Draw() const {
    IScene::Draw();
    //my
    if (hoveredTurret) {
     
        al_draw_circle(
            hoveredTurret->Position.x,
            hoveredTurret->Position.y,
            hoveredTurret->CollisionRadius,
            al_map_rgba(255, 255, 255, 128),  
            2.0f                               
        );
    }
    //my
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1) {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    //my
    if (button & 2 ) { 
        int gx = mx / BlockSize;
        int gy = my / BlockSize;
        if (gx >= 0 && gx < MapWidth && gy >= 0 && gy < MapHeight && mapState[gy][gx] == TILE_OCCUPIED) {
            
            for (auto it = TowerGroup->GetObjects().begin(); it != TowerGroup->GetObjects().end(); ++it) {
                auto t = dynamic_cast<Turret *>(*it);
                if (t) {
                    int tx = floor(t->Position.x / BlockSize);
                    int ty = floor(t->Position.y / BlockSize);
                    if (tx == gx && ty == gy) {
                        
                        t->GetObjectIterator()->first = false;     
                        TowerGroup->RemoveObject(t->GetObjectIterator());
                        mapState[gy][gx] = TILE_DIRT;             
                        break;
                    }
                }
            }
        }
    }
    

    
    //my
    if ((button & 1) && !imgTarget->Visible && preview) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        //return;
    }else{
        imgTarget->Visible = true;
        imgTarget->Position.x = x * BlockSize;
        imgTarget->Position.y = y * BlockSize;
    }
    //MY
    hoveredTurret = nullptr;
    for (auto obj : TowerGroup->GetObjects()) {
        Turret* t = dynamic_cast<Turret*>(obj);
        if (!t) continue;
        float dx = mx - t->Position.x;
        float dy = my - t->Position.y;
        if (dx*dx + dy*dy <= t->CollisionRadius * t->CollisionRadius/100) {
            hoveredTurret = t;
            break;
        }
    }
    //my
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1) {
        if (mapState[y][x] != TILE_OCCUPIED) {
            if (!preview)
                return;
            // Check if valid.
            if (!CheckSpaceValid(x, y)) {
                Engine::Sprite *sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            EarnMoney(-preview->GetPrice());
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            TowerGroup->AddNewObject(preview);
            // To keep responding when paused.
            preview->Update(0);
            // Remove Preview.
            preview = nullptr;

            mapState[y][x] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        }
    }
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    } else {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > code.size())
            keyStrokes.erase(keyStrokes.begin());
        //my
      


        // === TODO HACKATHON-4 (3/3) ===
        
        if (keyStrokes.size() == code.size() &&
            std::equal(keyStrokes.begin(), keyStrokes.end(), code.begin())) {
  
            
            /*Engine::Point spawn = Engine::Point(
               SpawnGridPoint.x * BlockSize + BlockSize/2,
               SpawnGridPoint.y * BlockSize + BlockSize/2
            );
            
            Enemy* enemy = new PlaneEnemy(spawn.x, spawn.y);
            EnemyGroup->AddNewObject(enemy);
           
            enemy->UpdatePath(mapDistance);*/

            
            EarnMoney(10000);
            EffectGroup->AddNewObject(new Plane());
            //Plane::getPlayScene()->EffectGroup->AddNewObject(new Plane());
            
            keyStrokes.clear();
        }
        //my
    }
    if (keyCode == ALLEGRO_KEY_Q) {
        // Hotkey for MachineGunTurret.
        UIBtnClicked(0);
    } else if (keyCode == ALLEGRO_KEY_W) {
        // Hotkey for LaserTurret.
        UIBtnClicked(1);
    }
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    }
}
void PlayScene::Hit() {
    lives--;
    if (UILives) {
        UILives->Text = std::string("Life ") + std::to_string(lives);
    }
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap() {
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c) {
        switch (c) {
            case '0': mapData.push_back(false); break;
            case '1': mapData.push_back(true); break;
            case '\n':
            case '\r':
                if (static_cast<int>(mapData.size()) / MapWidth != 0)
                    throw std::ios_base::failure("Map data is corrupted.");
                break;
            default: throw std::ios_base::failure("Map data is corrupted.");
        }
    }
    fin.close();
    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data is corrupted.");
    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void PlayScene::ReadEnemyWave() {
    
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat) {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::ConstructUI() {
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
    UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
    UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88));
    TurretButton *btn;
    // Button 1
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1294, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", 1294, 136 - 8, 0, 0, 0, 0), 1294, 136, MachineGunTurret::Price);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    // Button 2
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1370, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", 1370, 136 - 8, 0, 0, 0, 0), 1370, 136, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
 //button 3
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1446, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-fire.png", 1446, 136 - 8, 0, 0, 0, 0), 1446, 136, SplashTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
//button 4
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1522, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-3.png", 1522, 136 - 8, 0, 0, 0, 0), 1522, 136, UpGradeButton::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);
//button 5
    // 1. 构造一个 ImageButton，off/on 用舞台上已有的按钮贴图
    Engine::ImageButton* mineBtn = new Engine::ImageButton(
        "stage-select/dirt.png",   // 未按下时
        "stage-select/floor.png",  // 按下时
        1294 , 212,            // X, Y （紧挨第4个按钮之后）
        76, 76                     // 宽高，与其他按钮保持一致
    );

    // 2. 绑定回调，确保捕获 this
    mineBtn->SetOnClickCallback([this]() {
        // 算格子
        auto mp = Engine::GameEngine::GetInstance().GetMousePosition();
        int gx = int(mp.x / BlockSize);
        int gy = int(mp.y / BlockSize);
        if (gx < 0 || gx >= MapWidth || gy < 0 || gy >= MapHeight) return;
        // 算中心
        float px = gx * BlockSize + BlockSize / 2.0f;
        float py = gy * BlockSize + BlockSize / 2.0f;
        // 放地雷
        GroundEffectGroup->AddNewObject(new Lm(px, py));
    });

    // 3. **跟炮塔按钮一样** 加到 UIGroup 的 control list
    UIGroup->AddNewControlObject(mineBtn);

    // 4. （可选）给它加个 Label
    UIGroup->AddNewObject(new Engine::Label(
        "MINE", "pirulen.ttf", 16,
        1294 + 76/2, 212 + 76 + 12,
        255,255,255,255, 0.5f, 0.5f
    ));

//my
//my
    ruinedTurretLabel = new Engine::Label(
        "Ruined Turret: 0",     // 初始文字
        "pirulen.ttf", 16,      // 字体和字号
        1410,            // x 坐标（可按需微调）
        212 + 76 + 12 + 80,     // y 坐标（往下再加一些）
        255, 0, 0, 255,         // 红色不透明
        0.5f, 0.5f              // 文本居中
    );
    UIGroup->AddNewObject(ruinedTurretLabel);
//my
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int shift = 135 + 25;
    dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::UIBtnClicked(int id) {
    if (preview)
        UIGroup->RemoveObject(preview->GetObjectIterator());
    if (id == 0 && money >= MachineGunTurret::Price)
        preview = new MachineGunTurret(0, 0);
    else if (id == 1 && money >= LaserTurret::Price)
        preview = new LaserTurret(0, 0);
    else if (id == 2 && money >= SplashTurret::Price)
        preview = new SplashTurret(0, 0);
    else if (id == 3 && money >= UpGradeButton::Price)
        preview = new UpGradeButton(0, 0);
    /*else if (id == 4 )
        preview = new UpGradeButton(0, 0);*/
    if (!preview)
        return;
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[0][0] == -1)
        return false;
    for (auto &it : EnemyGroup->GetObjects()) {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0) pnt.x = 0;
        if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
        if (pnt.y < 0) pnt.y = 0;
        if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
        if (map[pnt.y][pnt.x] == -1)
            return false;
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto &it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;
    //my
    const int dx[4] = { 1, -1, 0,  0 };
    const int dy[4] = { 0,  0, 1, -1 };
    //my
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();
        // TODO PROJECT-1 (1/1): Implement a BFS starting from the most right-bottom block in the map.
        //               For each step you should assign the corresponding distance to the most right-bottom block.
        //               mapState[y][x] is TILE_DIRT if it is empty.
        // my
        
        int d = map[p.y][p.x];

        for (int dir = 0; dir < 4; dir++) {
            int nx = p.x + dx[dir];
            int ny = p.y + dy[dir];
            
            if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
                continue;
            
            if (mapState[ny][nx] == TILE_DIRT && map[ny][nx] == -1) {
                map[ny][nx] = d + 1;
                que.push(Engine::Point(nx, ny));
            }
        }
        //my
    }
    return map;
}
