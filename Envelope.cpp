#include "Envelope.hpp"

Envelope::Envelope(float a, float d, float s, float r)
    : attack(a), decay(d), sustain(s), release(r), value(0.0f), time(0.0f), state(0) {}

void Envelope::noteOn() {
    state = 1;
    time = 0.0f;
}
void Envelope::noteOff() {
    state = 4;
    time = 0.0f;
}
float Envelope::process(float dt) {
    switch (state) {
        case 0: value = 0.0f; break;
        case 1: value += dt / attack; if (value >= 1.0f) { value = 1.0f; state = 2; } break;
        case 2: value -= dt * (1.0f - sustain) / decay; if (value <= sustain) { value = sustain; state = 3; } break;
        case 3: value = sustain; break;
        case 4: value -= dt * sustain / release; if (value <= 0.0f) { value = 0.0f; state = 0; } break;
    }
    return value;
}
