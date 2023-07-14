#pragma once
#include "Worker.h"
#include <cmath>
#include <thread>

class Core : public Worker {
private:
    int coreNumber;
    int columnPosition;
    int rowPosition;
    float columnWidth;
    float rowStep;
    float pointsDiff;
    int countRows;
    int countColumns;
public:
    Core(int number);
    ~Core();
    void calculateUsage() override;
};

