#include "Cpu.h"

Cpu::Cpu() {
    counterPath = L"\\Processor(_Total)\\% Processor Time";
    PdhOpenQuery(NULL, 0, &query);
    PdhAddEnglishCounter(query, counterPath.c_str(), 0, &counter);
    PdhCollectQueryData(query);

    totalUsagePoints = 50;
    pointsDiff = 1.6f / (float)totalUsagePoints;
    frame.pushBack(Point(-0.8f, 0.2f, Color(1.0f, 1.0f, 1.0f)));
    frame.pushBack(Point(-0.8f, 0.7f, Color(1.0f, 1.0f, 1.0f)));
    frame.pushBack(Point(0.8f, 0.7f, Color(1.0f, 1.0f, 1.0f)));
    frame.pushBack(Point(0.8f, 0.2f, Color(1.0f, 1.0f, 1.0f)));
}

Cpu::~Cpu() {
    if (query) {
        PdhCloseQuery(query);
    }
}

void Cpu::calculateUsage() {
    PDH_FMT_COUNTERVALUE counterValue;
    PdhCollectQueryData(query);
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterValue);

    float usage = counterValue.doubleValue / 100.0f;
    float x = -0.8f + (points.getElementsCount() % 50) * pointsDiff;
    Color color = getUsageColor(usage);
    Point p = Point(x, usage / 2 + 0.2, color);
    points.pushBack(p);
}
