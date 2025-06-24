
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

void drawValueIndicator(SDL_Renderer *renderer, int x, int y, int w, int h,
                        const std::string &label, float value, const std::string &unit)
{
    // Değer göstergesi için arka plan
    SDL_Rect bgRect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &bgRect);
    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderDrawRect(renderer, &bgRect);

    // Simple text simulation with rectangles (pixel art style)
    // Frequency value as small bars
    int barCount = 10;
    int normalizedValue = (int)(value / 100.0f * barCount); // Normalize to bar count
    for (int i = 0; i < barCount; i++)
    {
        SDL_Rect bar = {x + 5 + i * 3, y + h - 8, 2, 6};
        if (i < normalizedValue)
        {
            SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        }
        SDL_RenderFillRect(renderer, &bar);
    }
}

int main(int argc, char *argv[])
{
    Envelope env;
    Sequencer seq;
    // Daha organize layout
    int margin = 20;
    int sliderWidth = 180;
    int sliderHeight = 35;

    // Sol sütun - Ana kontroller
    Slider volumeSlider(margin, margin, sliderWidth, sliderHeight, 0, 100, 50, "Volume");
    Slider filterSlider(margin, margin + 50, sliderWidth, sliderHeight, 100, 5000, 1000, "Filter");
    WaveSelector waveSelector(margin + sliderWidth + 20, margin, 100, sliderHeight * 2 + 15);

    // Orta sütun - LFO kontrolleri
    int midCol = WINDOW_WIDTH / 2 - 80;
    Slider lfoRateSlider(midCol, margin, 160, sliderHeight, 1, 20, 4, "LFO Rate");
    Slider lfoDepthSlider(midCol, margin + 50, 160, sliderHeight, 0, 100, 30, "LFO Depth");
    WaveSelector lfoWaveSelector(midCol, margin + 100, 80, sliderHeight);
    LFOTargetSelector lfoTargetSelector(midCol + 90, margin + 100, 70, sliderHeight);

    // Sağ sütun - ADSR envelope kontrolleri
    int rightCol = WINDOW_WIDTH - 200;
    Slider attackSlider(rightCol, margin, 160, sliderHeight, 1, 500, 10, "Attack");
    Slider decaySlider(rightCol, margin + 50, 160, sliderHeight, 1, 500, 100, "Decay");
    Slider sustainSlider(rightCol, margin + 100, 160, sliderHeight, 0, 100, 80, "Sustain");
    Slider releaseSlider(rightCol, margin + 150, 160, sliderHeight, 1, 500, 200, "Release");

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
    int pianoY = WINDOW_HEIGHT - 80;
    int pianoWidth = WINDOW_WIDTH - (margin * 2);
    int pianoHeight = 60;
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
                // UI kontrolleri
                volumeSlider.handleEvent(event);
                filterSlider.handleEvent(event);
                waveSelector.handleEvent(event);

                // LFO kontrolleri
                lfoRateSlider.handleEvent(event);
                lfoDepthSlider.handleEvent(event);
                lfoWaveSelector.handleEvent(event);
                lfoTargetSelector.handleEvent(event);

                // ADSR kontrolleri
                attackSlider.handleEvent(event);
                decaySlider.handleEvent(event);
                sustainSlider.handleEvent(event);
                releaseSlider.handleEvent(event);
            }
            if (event.type == SDL_MOUSEBUTTONUP)
            {
                activeKey = -1;
                synth.env.noteOff();
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                volumeSlider.handleEvent(event);
                filterSlider.handleEvent(event);
                lfoRateSlider.handleEvent(event);
                lfoDepthSlider.handleEvent(event);
                attackSlider.handleEvent(event);
                decaySlider.handleEvent(event);
                sustainSlider.handleEvent(event);
                releaseSlider.handleEvent(event);
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

        // Modern gradient background
        SDL_SetRenderDrawColor(renderer, 25, 25, 35, 255);
        SDL_RenderClear(renderer);

        // Waveform visualization area
        int waveAreaY = 120;
        int waveAreaHeight = pianoY - waveAreaY - 10;

        // Waveform background
        SDL_Rect waveBackground = {margin, waveAreaY, WINDOW_WIDTH - (margin * 2), waveAreaHeight};
        SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
        SDL_RenderFillRect(renderer, &waveBackground);
        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_RenderDrawRect(renderer, &waveBackground);

        // Dalga şekli çiz (ortada)
        WaveForm::draw(renderer, synth.waveType, synth.baseFrequency, displayPhase,
                       waveBackground.x + 5, waveBackground.y + 5,
                       waveBackground.w - 10, waveBackground.h - 10);

        // Frekans barını kaldırıyoruz, gerekirse daha sonra ekleriz

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