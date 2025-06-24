
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <portaudio.h>
#include <iostream>
#include <cmath>
#include "WaveForm.hpp"
#include "Piano.hpp"
#include "Envelope.hpp"
#include "Sequencer.hpp"
#include "UI.hpp"
#include "Synth.hpp"
#include "Filter.hpp"
#include "LFO.hpp"

#define SAMPLE_RATE 44100
#define TWO_PI (3.14159f * 2)
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 300

Synth synth;
int audioCallback(const void *, void *outputBuffer, unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags, void *)
{
    float *out = (float *)outputBuffer;
    for (unsigned long i = 0; i < framesPerBuffer; ++i)
    {
        float sample = synth.process(1.0f / SAMPLE_RATE);
        out[i * 2] = sample;
        out[i * 2 + 1] = sample;
    }
    return paContinue;
}

void drawControlLabel(SDL_Renderer *renderer, int x, int y, const std::string &label)
{
    // Futuristic neon label with HUD styling
    int labelWidth = label.length() * 8 + 16;
    SDL_Rect labelBg = {x - 8, y - 2, labelWidth, 24};

    // Multi-layer depth effect
    for (int i = 3; i >= 0; i--)
    {
        SDL_Rect depthRect = {labelBg.x + i, labelBg.y + i, labelBg.w - i * 2, labelBg.h - i * 2};
        int alpha = 40 - i * 8;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &depthRect);
    }

    // Main HUD background
    SDL_SetRenderDrawColor(renderer, 8, 12, 20, 235);
    SDL_RenderFillRect(renderer, &labelBg);

    // Neon border
    SDL_SetRenderDrawColor(renderer, 0, 180, 255, 180);
    SDL_RenderDrawRect(renderer, &labelBg);

    // Corner accents
    SDL_SetRenderDrawColor(renderer, 0, 255, 200, 255);
    // Top-left corner
    SDL_RenderDrawLine(renderer, labelBg.x, labelBg.y, labelBg.x + 6, labelBg.y);
    SDL_RenderDrawLine(renderer, labelBg.x, labelBg.y, labelBg.x, labelBg.y + 6);
    // Top-right corner
    SDL_RenderDrawLine(renderer, labelBg.x + labelBg.w - 6, labelBg.y,
                       labelBg.x + labelBg.w, labelBg.y);
    SDL_RenderDrawLine(renderer, labelBg.x + labelBg.w, labelBg.y,
                       labelBg.x + labelBg.w, labelBg.y + 6);

    // Inner holographic glow
    SDL_Rect innerGlow = {labelBg.x + 2, labelBg.y + 2, labelBg.w - 4, 4};
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 60);
    SDL_RenderFillRect(renderer, &innerGlow);

    // Enhanced neon text - double layer
    for (int layer = 1; layer >= 0; layer--)
    {
        SDL_Color textColor = layer == 0 ? SDL_Color{255, 255, 255, 255} : // Core white
                                  SDL_Color{0, 220, 255, 150};             // Cyan glow

        SDL_SetRenderDrawColor(renderer, textColor.r, textColor.g, textColor.b, textColor.a);

        for (int i = 0; i < (int)label.length(); i++)
        {
            char c = label[i];
            int charX = x + i * 8 - layer;
            int charY = y + 6 - layer;

            // Enhanced character rendering
            if (c >= 'A' && c <= 'Z')
            {
                // Capital letter - tech style bars
                SDL_Rect bars[4] = {
                    {charX, charY, 6, 1},     // Top
                    {charX, charY + 4, 4, 1}, // Middle
                    {charX, charY + 8, 6, 1}, // Bottom
                    {charX, charY, 1, 8}      // Left vertical
                };
                for (int j = 0; j < 4; j++)
                {
                    SDL_RenderFillRect(renderer, &bars[j]);
                }
            }
            else if (c >= 'a' && c <= 'z')
            {
                // Lowercase - smaller pattern
                SDL_Rect dots[3] = {
                    {charX, charY + 2, 4, 1},
                    {charX, charY + 5, 4, 1},
                    {charX, charY + 8, 4, 1}};
                for (int j = 0; j < 3; j++)
                {
                    SDL_RenderFillRect(renderer, &dots[j]);
                }
            }
            else if (c >= '0' && c <= '9')
            {
                // Numbers - digital style
                SDL_Rect digit = {charX, charY + 3, 5, 5};
                SDL_RenderDrawRect(renderer, &digit);
                SDL_Rect center = {charX + 2, charY + 5, 1, 1};
                SDL_RenderFillRect(renderer, &center);
            }
            else if (c == ' ')
            {
                // Space - skip
                continue;
            }
            else
            {
                // Special characters - dash
                SDL_Rect dash = {charX, charY + 4, 5, 1};
                SDL_RenderFillRect(renderer, &dash);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    Envelope env;
    Sequencer seq;
    // Daha organize layout - label'lar için yer bırakıyoruz
    int margin = 20;
    int topMargin = 40; // Label'lar için üst boşluk
    int sliderWidth = 160;
    int sliderHeight = 35;
    int spacing = 55; // Slider'lar arası boşluk

    // Sol sütun - Ana kontroller
    Slider volumeSlider(margin, topMargin, sliderWidth, sliderHeight, 0, 100, 50, "Volume");
    Slider filterSlider(margin, topMargin + spacing, sliderWidth, sliderHeight, 100, 5000, 1000, "Filter");
    WaveSelector waveSelector(margin + sliderWidth + 20, topMargin, 100, spacing + sliderHeight);

    // Orta sütun - LFO kontrolleri
    int midCol = WINDOW_WIDTH / 2 - 80;
    Slider lfoRateSlider(midCol, topMargin, sliderWidth, sliderHeight, 1, 20, 4, "LFO Rate");
    Slider lfoDepthSlider(midCol, topMargin + spacing, sliderWidth, sliderHeight, 0, 100, 30, "LFO Depth");
    WaveSelector lfoWaveSelector(midCol, topMargin + spacing * 2, 80, sliderHeight);
    LFOTargetSelector lfoTargetSelector(midCol + 90, topMargin + spacing * 2, 70, sliderHeight);

    // Sağ sütun - ADSR envelope kontrolleri
    int rightCol = WINDOW_WIDTH - 180;
    Slider attackSlider(rightCol, topMargin, sliderWidth, sliderHeight, 1, 500, 10, "Attack");
    Slider decaySlider(rightCol, topMargin + spacing, sliderWidth, sliderHeight, 1, 500, 100, "Decay");
    Slider sustainSlider(rightCol, topMargin + spacing * 2, sliderWidth, sliderHeight, 0, 100, 80, "Sustain");
    Slider releaseSlider(rightCol, topMargin + spacing * 3, sliderWidth, sliderHeight, 1, 500, 200, "Release");

    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cerr << "PortAudio init failed: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream,
                               0, 2,
                               paFloat32,
                               SAMPLE_RATE,
                               256,
                               audioCallback,
                               nullptr);
    if (err != paNoError)
    {
        std::cerr << "PortAudio open stream failed: " << Pa_GetErrorText(err) << "\n";
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio start stream failed: " << Pa_GetErrorText(err) << "\n";
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL init error: " << SDL_GetError() << "\n";
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Synth Frequency Control",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "SDL window error: " << SDL_GetError() << "\n";
        SDL_Quit();
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        std::cerr << "SDL renderer error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    float displayPhase = 0.0f; // Canlı dalga için faz
    int activeKey = -1;
    const float noteFreqs[14] = {
        261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f, 392.00f, 415.30f, 440.00f, 466.16f, 493.88f, 523.25f, 0.0f};

    // Piyano konumu - Alt kısımda, daha büyük
    int pianoX = margin;
    int pianoY = WINDOW_HEIGHT - 70;
    int pianoWidth = WINDOW_WIDTH - (margin * 2);
    int pianoHeight = 50;
    Piano piano;

    std::cout << "Sağ/Sol ok tuşları ile frekansı değiştir. ESC ile çık.\n";

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_RIGHT:
                    synth.baseFrequency += 10.0f;
                    if (synth.baseFrequency > 2000.0f)
                        synth.baseFrequency = 2000.0f;
                    synth.setFrequency(synth.baseFrequency);
                    std::cout << "Frekans: " << synth.baseFrequency << " Hz\n";
                    synth.env.noteOn();
                    break;
                case SDLK_LEFT:
                    synth.baseFrequency -= 10.0f;
                    if (synth.baseFrequency < 100.0f)
                        synth.baseFrequency = 100.0f;
                    synth.setFrequency(synth.baseFrequency);
                    std::cout << "Frekans: " << synth.baseFrequency << " Hz\n";
                    synth.env.noteOn();
                    break;
                default:
                    synth.env.noteOn();
                    break;
                }
            }
            if (event.type == SDL_KEYUP)
            {
                synth.env.noteOff();
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mx = event.button.x;
                int my = event.button.y;
                int key = piano.getKeyAtPosition(mx, my, pianoX, pianoY, pianoWidth, pianoHeight);
                if (key != -1)
                {
                    activeKey = key;
                    if (noteFreqs[key] > 0.0f)
                    {
                        synth.setFrequency(noteFreqs[key]);
                        std::cout << "Nota: " << key << " Frekans: " << synth.baseFrequency << " Hz\n";
                        synth.env.noteOn();
                    }
                }
                // UI kontrolleri - sadece ilk bulan handle etsin
                if (volumeSlider.handleEvent(event))
                {
                }
                else if (filterSlider.handleEvent(event))
                {
                }
                else if (waveSelector.handleEvent(event))
                {
                }
                else if (lfoRateSlider.handleEvent(event))
                {
                }
                else if (lfoDepthSlider.handleEvent(event))
                {
                }
                else if (lfoWaveSelector.handleEvent(event))
                {
                }
                else if (lfoTargetSelector.handleEvent(event))
                {
                }
                else if (attackSlider.handleEvent(event))
                {
                }
                else if (decaySlider.handleEvent(event))
                {
                }
                else if (sustainSlider.handleEvent(event))
                {
                }
                else if (releaseSlider.handleEvent(event))
                {
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP)
            {
                activeKey = -1;
                synth.env.noteOff();

                // Tüm slider'ları durdur
                volumeSlider.dragging = false;
                filterSlider.dragging = false;
                lfoRateSlider.dragging = false;
                lfoDepthSlider.dragging = false;
                attackSlider.dragging = false;
                decaySlider.dragging = false;
                sustainSlider.dragging = false;
                releaseSlider.dragging = false;
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                // Mouse motion için sadece dragging olan slider'lar için
                if (volumeSlider.handleEvent(event))
                {
                }
                else if (filterSlider.handleEvent(event))
                {
                }
                else if (lfoRateSlider.handleEvent(event))
                {
                }
                else if (lfoDepthSlider.handleEvent(event))
                {
                }
                else if (attackSlider.handleEvent(event))
                {
                }
                else if (decaySlider.handleEvent(event))
                {
                }
                else if (sustainSlider.handleEvent(event))
                {
                }
                else if (releaseSlider.handleEvent(event))
                {
                }
            }
        }

        // UI değerlerini synth'e aktar
        synth.amplitude = volumeSlider.value / 100.0f;
        synth.waveType = waveSelector.currentWave;

        // ADSR envelope parametrelerini güncelle
        synth.env.attack = attackSlider.value / 1000.0f; // ms to seconds
        synth.env.decay = decaySlider.value / 1000.0f;
        synth.env.sustain = sustainSlider.value / 100.0f; // 0-1 range
        synth.env.release = releaseSlider.value / 1000.0f;

        // Filter parametrelerini güncelle
        synth.baseCutoff = filterSlider.value;
        if (synth.lfo.target != LFOTarget::Filter)
        {
            synth.filter.setCutoff(filterSlider.value);
        }

        // LFO parametrelerini güncelle
        synth.lfo.rate = lfoRateSlider.value;
        synth.lfo.depth = lfoDepthSlider.value / 100.0f;
        synth.lfo.waveform = lfoWaveSelector.currentWave;
        synth.lfo.target = lfoTargetSelector.currentTarget;
        synth.lfo.enabled = (synth.lfo.target != LFOTarget::None);

        // Futuristic dark gradient background
        for (int y = 0; y < WINDOW_HEIGHT; y++)
        {
            float ratio = (float)y / WINDOW_HEIGHT;
            int r = 5 + (15 - 5) * ratio;   // 5 to 15
            int g = 10 + (25 - 10) * ratio; // 10 to 25
            int b = 20 + (40 - 20) * ratio; // 20 to 40

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawLine(renderer, 0, y, WINDOW_WIDTH, y);
        }

        // Ambient glow effects
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

        // Top ambient light
        for (int i = 0; i < 30; i++)
        {
            int alpha = 10 - i / 3;
            if (alpha > 0)
            {
                SDL_SetRenderDrawColor(renderer, 0, 20, 40, alpha);
                SDL_RenderDrawLine(renderer, 0, i, WINDOW_WIDTH, i);
            }
        }

        // Subtle grid pattern for sci-fi look
        SDL_SetRenderDrawColor(renderer, 0, 50, 100, 15);
        for (int x = 0; x < WINDOW_WIDTH; x += 50)
        {
            SDL_RenderDrawLine(renderer, x, 0, x, WINDOW_HEIGHT);
        }
        for (int y = 0; y < WINDOW_HEIGHT; y += 50)
        {
            SDL_RenderDrawLine(renderer, 0, y, WINDOW_WIDTH, y);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Waveform visualization area
        int waveAreaY = 180; // Label'lar için daha fazla yer bırakıyoruz
        int waveAreaHeight = pianoY - waveAreaY - 10;

        // Enhanced waveform display with modern styling
        SDL_Rect waveBackground = {margin, waveAreaY, WINDOW_WIDTH - (margin * 2), waveAreaHeight};

        // Waveform container with depth
        for (int i = 5; i >= 0; i--)
        {
            SDL_Rect depthRect = {waveBackground.x + i, waveBackground.y + i,
                                  waveBackground.w - i * 2, waveBackground.h - i * 2};
            int alpha = 40 - i * 5;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_RenderFillRect(renderer, &depthRect);
        }

        // Main waveform background with gradient
        SDL_SetRenderDrawColor(renderer, 15, 20, 30, 240);
        SDL_RenderFillRect(renderer, &waveBackground);

        // Inner glow
        SDL_Rect innerGlow = {waveBackground.x + 2, waveBackground.y + 2,
                              waveBackground.w - 4, waveBackground.h - 4};
        SDL_SetRenderDrawColor(renderer, 0, 30, 60, 30);
        SDL_RenderFillRect(renderer, &innerGlow);

        // Tech border
        SDL_SetRenderDrawColor(renderer, 0, 100, 150, 180);
        SDL_RenderDrawRect(renderer, &waveBackground);

        // Corner accents
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderDrawLine(renderer, waveBackground.x, waveBackground.y,
                           waveBackground.x + 10, waveBackground.y);
        SDL_RenderDrawLine(renderer, waveBackground.x, waveBackground.y,
                           waveBackground.x, waveBackground.y + 10);
        SDL_RenderDrawLine(renderer, waveBackground.x + waveBackground.w - 10, waveBackground.y,
                           waveBackground.x + waveBackground.w, waveBackground.y);
        SDL_RenderDrawLine(renderer, waveBackground.x + waveBackground.w, waveBackground.y,
                           waveBackground.x + waveBackground.w, waveBackground.y + 10);

        // Enhanced waveform visualization
        WaveForm::draw(renderer, synth.waveType, synth.baseFrequency, displayPhase,
                       waveBackground.x + 8, waveBackground.y + 8,
                       waveBackground.w - 16, waveBackground.h - 16);

        // Frekans barını kaldırıyoruz, gerekirse daha sonra ekleriz

        // Control labels çiz
        drawControlLabel(renderer, volumeSlider.x, volumeSlider.y - 25, "VOLUME");
        drawControlLabel(renderer, filterSlider.x, filterSlider.y - 25, "FILTER");
        drawControlLabel(renderer, waveSelector.x, waveSelector.y - 25, "WAVE");

        drawControlLabel(renderer, lfoRateSlider.x, lfoRateSlider.y - 25, "LFO RATE");
        drawControlLabel(renderer, lfoDepthSlider.x, lfoDepthSlider.y - 25, "LFO DEPTH");
        drawControlLabel(renderer, lfoWaveSelector.x, lfoWaveSelector.y - 25, "LFO WAVE");
        drawControlLabel(renderer, lfoTargetSelector.x, lfoTargetSelector.y - 25, "LFO TARGET");

        drawControlLabel(renderer, attackSlider.x, attackSlider.y - 25, "ATTACK");
        drawControlLabel(renderer, decaySlider.x, decaySlider.y - 25, "DECAY");
        drawControlLabel(renderer, sustainSlider.x, sustainSlider.y - 25, "SUSTAIN");
        drawControlLabel(renderer, releaseSlider.x, releaseSlider.y - 25, "RELEASE");

        // UI kontrollerini çiz
        volumeSlider.draw(renderer);
        filterSlider.draw(renderer);
        waveSelector.draw(renderer);

        // LFO kontrollerini çiz
        lfoRateSlider.draw(renderer);
        lfoDepthSlider.draw(renderer);
        lfoWaveSelector.draw(renderer);
        lfoTargetSelector.draw(renderer);

        // ADSR kontrollerini çiz
        attackSlider.draw(renderer);
        decaySlider.draw(renderer);
        sustainSlider.draw(renderer);
        releaseSlider.draw(renderer);

        // Piyano çiz (altta)
        piano.draw(renderer, pianoX, pianoY, pianoWidth, pianoHeight, activeKey);

        SDL_RenderPresent(renderer);

        // Dalga animasyonu için fazı güncelle
        displayPhase += TWO_PI * synth.baseFrequency / SAMPLE_RATE * 256;
        if (displayPhase >= TWO_PI)
            displayPhase -= TWO_PI;

        SDL_Delay(16); // Yaklaşık 60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}