#include "UI.hpp"

Slider::Slider(int x_, int y_, int w_, int h_, int min_, int max_, int value_, const std::string &label_)
    : x(x_), y(y_), w(w_), h(h_), min(min_), max(max_), value(value_), dragging(false), label(label_) {}

void Slider::draw(SDL_Renderer *renderer) const
{
    // Gradient effect için arka plan
    SDL_Rect background = {x - 5, y - 5, w + 10, h + 10};
    SDL_SetRenderDrawColor(renderer, 45, 45, 55, 255);
    SDL_RenderFillRect(renderer, &background);

    // Soft border
    SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
    SDL_RenderDrawRect(renderer, &background);

    // Progress bar (filled portion)
    int fillWidth = (value - min) * w / (max - min);
    SDL_Rect progressBar = {x, y + h / 2 - 3, fillWidth, 6};
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255); // Cyan blue
    SDL_RenderFillRect(renderer, &progressBar);

    // Track (unfilled portion)
    SDL_Rect track = {x + fillWidth, y + h / 2 - 3, w - fillWidth, 6};
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &track);

    // Knob
    int knobX = x + (value - min) * w / (max - min);
    SDL_Rect knob = {knobX - 6, y + h / 2 - 8, 12, 16};

    // Knob glow effect
    if (dragging)
    {
        SDL_Rect glow = {knobX - 8, y + h / 2 - 10, 16, 20};
        SDL_SetRenderDrawColor(renderer, 150, 220, 255, 100);
        SDL_RenderFillRect(renderer, &glow);
    }

    // Knob body
    SDL_SetRenderDrawColor(renderer, 200, 200, 220, 255);
    SDL_RenderFillRect(renderer, &knob);

    // Knob border
    SDL_SetRenderDrawColor(renderer, 150, 150, 170, 255);
    SDL_RenderDrawRect(renderer, &knob);
}

bool Slider::handleEvent(const SDL_Event &event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int mx = event.button.x, my = event.button.y;
        if (mx >= x && mx < x + w && my >= y && my < y + h)
        {
            dragging = true;
            // Mouse pozisyonuna göre hemen değeri güncelle
            int newValue = min + (mx - x) * (max - min) / w;
            if (newValue < min)
                newValue = min;
            if (newValue > max)
                newValue = max;
            value = newValue;
            return true;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        // Sadece bu slider dragging durumundaysa kapat
        if (dragging)
        {
            dragging = false;
            return true;
        }
    }
    else if (event.type == SDL_MOUSEMOTION && dragging)
    {
        int mx = event.motion.x;
        int newValue = min + (mx - x) * (max - min) / w;
        if (newValue < min)
            newValue = min;
        if (newValue > max)
            newValue = max;
        value = newValue;
        return true;
    }
    return false;
}

// WaveSelector implementation
WaveSelector::WaveSelector(int x_, int y_, int w_, int h_)
    : x(x_), y(y_), w(w_), h(h_), currentWave(WaveForm::Sine)
{
    waveNames[0] = "SINE";
    waveNames[1] = "SQUARE";
    waveNames[2] = "TRIANGLE";
    waveNames[3] = "SAW";
}

void WaveSelector::draw(SDL_Renderer *renderer) const
{
    // Modern button background
    SDL_Rect outerRect = {x - 2, y - 2, w + 4, h + 4};
    SDL_SetRenderDrawColor(renderer, 35, 35, 45, 255);
    SDL_RenderFillRect(renderer, &outerRect);

    SDL_Rect buttonRect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Seçili dalga türü için gradient effect
    SDL_Rect innerRect = {x + 3, y + 3, w - 6, h - 6};

    // Waveform specific colors with better gradients
    switch (currentWave)
    {
    case WaveForm::Sine:
        SDL_SetRenderDrawColor(renderer, 255, 120, 120, 255);
        SDL_RenderFillRect(renderer, &innerRect);
        SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);
        break;
    case WaveForm::Square:
        SDL_SetRenderDrawColor(renderer, 120, 255, 120, 255);
        SDL_RenderFillRect(renderer, &innerRect);
        SDL_SetRenderDrawColor(renderer, 80, 200, 80, 255);
        break;
    case WaveForm::Triangle:
        SDL_SetRenderDrawColor(renderer, 120, 120, 255, 255);
        SDL_RenderFillRect(renderer, &innerRect);
        SDL_SetRenderDrawColor(renderer, 80, 80, 200, 255);
        break;
    case WaveForm::Saw:
        SDL_SetRenderDrawColor(renderer, 255, 255, 120, 255);
        SDL_RenderFillRect(renderer, &innerRect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 80, 255);
        break;
    }

    // Border highlight
    SDL_SetRenderDrawColor(renderer, 150, 150, 180, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);

    // Wave pattern visualization (simple)
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    int centerY = y + h / 2;
    int points = 8;

    for (int i = 0; i < points - 1; i++)
    {
        int x1 = x + 8 + i * (w - 16) / (points - 1);
        int x2 = x + 8 + (i + 1) * (w - 16) / (points - 1);

        float t1 = (float)i / (points - 1) * 2.0f * 3.14159f;
        float t2 = (float)(i + 1) / (points - 1) * 2.0f * 3.14159f;

        int y1, y2;

        switch (currentWave)
        {
        case WaveForm::Sine:
            y1 = centerY - (int)(sin(t1) * 8);
            y2 = centerY - (int)(sin(t2) * 8);
            break;
        case WaveForm::Square:
            y1 = centerY - (sin(t1) > 0 ? 8 : -8);
            y2 = centerY - (sin(t2) > 0 ? 8 : -8);
            break;
        case WaveForm::Triangle:
            y1 = centerY - (int)((2.0f * abs(2.0f * (t1 / (2 * 3.14159f) - floor(t1 / (2 * 3.14159f) + 0.5f))) - 1.0f) * 8);
            y2 = centerY - (int)((2.0f * abs(2.0f * (t2 / (2 * 3.14159f) - floor(t2 / (2 * 3.14159f) + 0.5f))) - 1.0f) * 8);
            break;
        case WaveForm::Saw:
            y1 = centerY - (int)((2.0f * (t1 / (2 * 3.14159f) - floor(t1 / (2 * 3.14159f) + 0.5f))) * 8);
            y2 = centerY - (int)((2.0f * (t2 / (2 * 3.14159f) - floor(t2 / (2 * 3.14159f) + 0.5f))) * 8);
            break;
        }

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

bool WaveSelector::handleEvent(const SDL_Event &event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int mx = event.button.x, my = event.button.y;
        if (mx >= x && mx < x + w && my >= y && my < y + h)
        {
            nextWave();
            return true;
        }
    }
    return false;
}

void WaveSelector::nextWave()
{
    currentWave = static_cast<WaveForm::Type>((static_cast<int>(currentWave) + 1) % 4);
}
