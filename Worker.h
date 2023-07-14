#pragma once
#include "GraphVector.h"
#include "Pdh.h"
#include <string>

struct Color {
    Color() {
        r = 0.0f;
        g = 0.0f;
        b = 0.0f;
    };
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
    float r;
    float g;
    float b;
};

struct Point {
    Point() {};
    Point(float x, float y, Color color) : x(x), y(y), color(color) {};
    float x;
    float y;
    Color color;
};

class Worker {
public:
    virtual void calculateUsage() = 0;
    GraphVector<Point>& getFramePoints();
    GraphVector<Point>& getUsageValues();
    Color getUsageColor(float usage);
protected:
    int totalUsagePoints;
    GraphVector<Point> points;
    GraphVector<Point> frame;
    HQUERY query = NULL;
    HCOUNTER counter = NULL;
    std::wstring counterPath;
};