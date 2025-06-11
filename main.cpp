
// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"

int main(int argc, char **argv)
{
    Engine::LOG::SetConfig(true);
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();

    // TODO HACKATHON-2 (2/3): Register Scenes here
    game.AddNewScene("start", new StartScene());
    game.AddNewScene("stage-select", new StageSelectScene());
    game.AddNewScene("scoreboard", new ScoreboardScene());
    game.AddNewScene("settings", new SettingsScene());
    game.AddNewScene("play", new PlayScene());
    game.AddNewScene("lose", new LoseScene());
    game.AddNewScene("win", new WinScene());
	

    // TODO HACKATHON-1 (1/1): Change the start scene
    game.Start("start", 60, 1600, 832);
    return 0;
}

// soft_jelly.cpp – Allegro 5 demo: squishy jelly with floor + walls (robust)
// -----------------------------------------------------------------------------
// 功能一覽
// • N = 24 軟體果凍（質點 + 彈簧）
// • 地板 + 左右牆（WALL_MARGIN 可調）
// • 左鍵拖曳：
//     – 點圓心 → 整顆拖動
//     – 點外框節點 → 單點拉動
// • 放開滑鼠後恢復正常物理；避免 prev/pos 不同步導致爆衝或消失
// -----------------------------------------------------------------------------
// double_pendulum.cpp – Allegro 5 demo: chaotic double pendulum (mouse‑draggable, robust)
// -----------------------------------------------------------------------------
//  • 960×720 @ 120 FPS, RK4 积分
//  • 左鍵點近節點 → 拖曳該端點，鬆手恢復模擬
//  • R 鍵重置；可拖曳過程中即時看到擺長線更新
//  • 向量軌跡 (TRACE_LEN) 保留殘影
// -----------------------------------------------------------------------------
// hexagon_bounce.cpp – ball inside spinning hexagon - with mouse-drag
// cloth_sim.cpp – Allegro 5 demo: spring-mass cloth (flag)
// -----------------------------------------------------------------------------
//  • 50 × 30 點 (寬 × 高) 網格，用質點 + 彈簧模型
//  • 固定左上、右上兩點當支架，其餘受重力
//  • Verlet integration + 約束迭代 (結構彈簧 + 剪刀-斜向彈簧)
//  • 鼠標左鍵按住可拖曳最近點
// -----------------------------------------------------------------------------
/*
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <vector>
#include <cmath>

struct Vec2{ float x,y; };
static inline Vec2 operator+(Vec2 a,Vec2 b){ return {a.x+b.x,a.y+b.y}; }
static inline Vec2 operator-(Vec2 a,Vec2 b){ return {a.x-b.x,a.y-b.y}; }
static inline Vec2 operator*(Vec2 a,float s){ return {a.x*s,a.y*s}; }
static inline float dot(Vec2 a,Vec2 b){ return a.x*b.x+a.y*b.y; }
static inline float len(Vec2 v){ return std::sqrt(dot(v,v)); }
static inline Vec2 norm(Vec2 v){ float l=len(v); return l? v*(1.f/l):Vec2{0,0}; }

struct Node{ Vec2 pos, prev; bool pinned=false; };
struct Spring{ int a,b; float rest; };

int main(){
    const int W=900,H=600; const float FPS=120.f;
    if(!al_init()) return -1;
    al_install_mouse(); al_init_primitives_addon();

    ALLEGRO_DISPLAY* d=al_create_display(W,H);
    ALLEGRO_TIMER*   tim=al_create_timer(1.0/FPS);
    ALLEGRO_EVENT_QUEUE*q=al_create_event_queue();
    al_register_event_source(q,al_get_display_event_source(d));
    al_register_event_source(q,al_get_timer_event_source(tim));
    al_register_event_source(q,al_get_mouse_event_source());

    // ------- cloth parameters ---------------------------------------------
    const int NX=50, NY=30;           // grid size
    const float SPACING = 10.f;       // pixel distance between nodes
    const Vec2 ORIGIN{150.f, 50.f};   // top-left position
    const float g = 980.f;            // gravity

    std::vector<Node> nodes(NX*NY);
    std::vector<Spring> springs;

    auto idx=[&](int x,int y){ return y*NX+x; };

    // create nodes
    for(int y=0;y<NY;++y){
        for(int x=0;x<NX;++x){
            Vec2 p{ ORIGIN.x + x*SPACING, ORIGIN.y + y*SPACING };
            nodes[idx(x,y)].pos = nodes[idx(x,y)].prev = p;
        }
    }
    // pin top row corners
    nodes[idx(0,0)].pinned = true;
    nodes[idx(NX-1,0)].pinned = true;

    // create springs (structural + shear)
    for(int y=0;y<NY;++y){
        for(int x=0;x<NX;++x){
            if(x<NX-1) springs.push_back({idx(x,y), idx(x+1,y), SPACING});       // horizontal
            if(y<NY-1) springs.push_back({idx(x,y), idx(x,y+1), SPACING});       // vertical
            if(x<NX-1 && y<NY-1){                                                // shear
                springs.push_back({idx(x,y),   idx(x+1,y+1), SPACING*std::sqrt(2.f)});
                springs.push_back({idx(x+1,y), idx(x,y+1),   SPACING*std::sqrt(2.f)});
            }
        }
    }

    // drag state
    bool dragging=false; int dragIdx=-1; Vec2 dragOff{0,0};

    auto satisfySpring=[&](Spring&s){
        Node &A=nodes[s.a], &B=nodes[s.b];
        Vec2 v = B.pos - A.pos; float d=len(v); if(d==0) return;
        Vec2 corr = v * (0.5f*(d - s.rest)/d);
        if(!A.pinned) A.pos = A.pos + corr;
        if(!B.pinned) B.pos = B.pos - corr;
    };

    al_start_timer(tim); bool run=true, redraw=true; double prev=al_get_time();
    while(run){
        ALLEGRO_EVENT ev; al_wait_for_event(q,&ev);
        if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE) break;

        if(ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button==1){
            Vec2 m{(float)ev.mouse.x,(float)ev.mouse.y};
            float best=1e9; int pick=-1;
            for(size_t i=0;i<nodes.size();++i){ float d=len(nodes[i].pos - m); if(d<best){best=d; pick=i;} }
            if(best<15){ dragging=true; dragIdx=pick; dragOff = nodes[pick].pos - m; }
        }
        if(ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP && ev.mouse.button==1){ dragging=false; dragIdx=-1; }

        if(ev.type==ALLEGRO_EVENT_TIMER){
            double now=al_get_time(); float dt=float(now-prev); prev=now;
            // integrate
            for(auto &n:nodes){ if(n.pinned) continue; Vec2 cur=n.pos; Vec2 acc{0,g}; n.pos = n.pos + (n.pos - n.prev) + acc*dt*dt; n.prev=cur; }
            // drag
            if(dragging && dragIdx!=-1){ Vec2 m{(float)ev.mouse.x,(float)ev.mouse.y}; nodes[dragIdx].pos = m + dragOff; nodes[dragIdx].prev = nodes[dragIdx].pos; }
            // constraints
            for(int iter=0;iter<5;++iter){ for(auto &s:springs) satisfySpring(s); }
            redraw=true;
        }

        if(redraw && al_is_event_queue_empty(q)){
            redraw=false; al_clear_to_color(al_map_rgb(20,22,30));
            // draw springs
            for(auto&s:springs){ auto &A=nodes[s.a], &B=nodes[s.b]; al_draw_line(A.pos.x,A.pos.y,B.pos.x,B.pos.y, al_map_rgb(200,230,255),1); }
            // draw nodes (optional small dots)
            for(auto &n:nodes) al_draw_filled_circle(n.pos.x,n.pos.y,2, al_map_rgb(255,120,120));
            al_flip_display();
        }
    }
    al_destroy_timer(tim); al_destroy_event_queue(q); al_destroy_display(d);
    return 0;
}*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

