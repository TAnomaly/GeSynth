#pragma once
#include <vector>
class Sequencer {
public:
    static constexpr int STEPS = 16;
    std::vector<int> notes;
    std::vector<float> lengths;
    int currentStep;
    float timer;
    bool playing;
    Sequencer();
    void start();
    void stop();
    void update(float dt);
};
