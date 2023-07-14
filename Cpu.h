#pragma once
#include "Worker.h"

class Cpu : public Worker {
private:
    float pointsDiff;
public:
    Cpu();
    ~Cpu();
    void calculateUsage() override;
};

