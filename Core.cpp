#include "Core.h"

Core::Core(int number) : coreNumber(number) {
    counterPath = L"\\Processor(*)\\% Processor Time";
    std::wstring replaceFrom = L"*";
    int pos = counterPath.find(replaceFrom);
    std::wstring replaceTo = std::to_wstring(coreNumber);
    counterPath.replace(pos, replaceFrom.size(), replaceTo);
    PdhOpenQuery(NULL, 0, &query);
    PdhAddEnglishCounter(query, counterPath.c_str(), 0, &counter);
    PdhCollectQueryData(query);

    totalUsagePoints = 50;
    countColumns = 5;
    int countCores = std::thread::hardware_concurrency();
    countRows = ceil(countCores / (float)countColumns);
    rowStep = 1.0 / (float)countRows;
    columnPosition = coreNumber % 5;
    rowPosition = countRows - coreNumber / 5 - 1;

    columnWidth = 1.6f / (float)countColumns;
    pointsDiff = 0.95 * columnWidth / (float)totalUsagePoints;

    frame.pushBack(Point(-0.8f + columnWidth * columnPosition, rowStep + rowPosition * rowStep - 0.9, Color(1.0f, 1.0f, 1.0f)));
    frame.pushBack(Point(-0.8f + columnWidth * columnPosition + columnWidth, rowStep + rowPosition * rowStep - 0.9, Color(1.0f, 1.0f, 1.0f)));
    frame.pushBack(Point(-0.8f + columnWidth * columnPosition + columnWidth, rowPosition * rowStep - 0.9, Color(1.0f, 1.0f, 1.0f)));
    frame.pushBack(Point(-0.8f + columnWidth * columnPosition, rowPosition * rowStep - 0.9, Color(1.0f, 1.0f, 1.0f)));
}

Core::~Core() {
    if (query) {
        PdhCloseQuery(query);
    }
}

void Core::calculateUsage() {
    PDH_FMT_COUNTERVALUE counterValue;
    PdhCollectQueryData(query);
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterValue);

    float x = -0.8f + columnWidth * columnPosition + (points.getElementsCount() % 50) * pointsDiff;
    float usage = counterValue.doubleValue / 100.0f;
    Color color = getUsageColor(usage);
    Point p = Point(x, usage / ((float)countRows + 0.1) + rowPosition * rowStep - 0.9, color);
    points.pushBack(p);
}