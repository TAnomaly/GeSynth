#pragma once
class Envelope {
public:
    float attack, decay, sustain, release, value, time;
    int state;
    Envelope(float a = 0.01f, float d = 0.1f, float s = 0.8f, float r = 0.2f);
    void noteOn();
    void noteOff();
    float process(float dt);
};
