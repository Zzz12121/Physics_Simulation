#ifndef CUT_BAG_SCENE_HPP
#define CUT_BAG_SCENE_HPP

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

using Vec2 = Engine::Point;

struct Point
{
    float x, y;   // 位置
    float px, py; // 前一刻位置 (Verlet積分)
    float vx, vy; // 速度
    float mass;   // 質量
    bool fixed;   // 是否固定
    bool active;  // 是否活躍
};

struct Spring
{
    Point *p1;        // 點1
    Point *p2;        // 點2
    float restLength; // 自然長度
    bool active;      // 是否活躍
};

struct CutBagBall
{
    float x, y;          // 位置
    float vx, vy;        // 速度
    float radius;        // 半徑
    float mass;          // 質量
    ALLEGRO_COLOR color; // 顏色
};

struct WindField
{
    float strength;  // 風力強度
    float frequency; // 頻率
    float direction; // 方向 (弧度)
    float time;      // 時間累積器
};

class cutBagScene final : public Engine::IScene
{
private:
    // 物理參數
    static constexpr float GRAVITY = 0.5f;
    static constexpr float SPRING_DAMPING = 0.02f;
    static constexpr float AIR_RESISTANCE = 0.995f;
    static constexpr float WIND_STRENGTH = 0.8f;
    static constexpr float TIME_STEP = 0.5f;
    static constexpr int CONSTRAINT_ITERATIONS = 3;

    // 模擬參數
    static constexpr int CLOTH_WIDTH = 50;
    static constexpr int CLOTH_HEIGHT = 40;
    static constexpr int GRID_SPACING = 8;
    static constexpr float BALL_RADIUS = 30.0f;
    static constexpr float BALL_MASS = 10.0f;

    std::vector<Point> points;
    std::vector<Spring> springs;
    std::vector<CutBagBall> balls;
    std::vector<float> ballTargetVxs; // 儲存每個球體的目標水平速度
    WindField wind;
    bool cuttingMode;
    float simulationSpeed;
    ALLEGRO_FONT *font;
    CutBagBall *draggedBall;
    Engine::Label* stiffnessLabel = nullptr;


public:
    explicit cutBagScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;

    void OnKeyDown(int keyCode) override;
    void OnMouseDown(int button, int x, int y) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseUp(int button, int x, int y) override;

    void BackOnClick(int stage);

private:
    void initializeCloth();
    void initializeBalls();
    void addSpring(int idx1, int idx2);
    float distance(float x1, float y1, float x2, float y2) const;
    void updateBalls(float deltaTime);
    void applyPhysics(float deltaTime);
    void applyConstraints();
    void cutCloth(int mouseX, int mouseY, float radius);
    void drawUI() const;
    void drawWindIndicator() const;
};

#endif // CUT_BAG_SCENE_HPP