#include "Sequencer.hpp"
#include <algorithm>

Sequencer::Sequencer() : notes(STEPS, -1), lengths(STEPS, 0.25f), currentStep(0), timer(0.0f), playing(false) {}
void Sequencer::start() { playing = true; currentStep = 0; timer = 0.0f; }
void Sequencer::stop() { playing = false; }
void Sequencer::update(float dt) {
    if (!playing) return;
    timer += dt;
    if (timer >= lengths[currentStep]) {
        timer = 0.0f;
        currentStep = (currentStep + 1) % STEPS;
    }
}
