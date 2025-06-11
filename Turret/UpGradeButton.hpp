#ifndef UPGRADEBUTTON_HPP
#define UPGRADEBUTTON_HPP
#include "Turret.hpp"

class UpGradeButton : public Turret {
public:
    static const int Price;
    UpGradeButton(float x, float y);
    void CreateBullet() override;
    //int sppike() override;
};
#endif   // LASERTURRET_HPP
