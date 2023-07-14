#pragma once
#include "Worker.h"

GraphVector<Point>& Worker::getUsageValues() {
    return points;
}

GraphVector<Point>& Worker::getFramePoints() {
    return frame;
}

Color Worker::getUsageColor(float usage) {
    Color color;
    if (usage >= 0.75) {
        color.r = 1.0f;
    }
    else if (usage >= 0.5) {
        color.r = 1.0f;
        color.g = 1.0f;
    }
    else {
        color.g = 1.0f;
    }
    return color;
}
