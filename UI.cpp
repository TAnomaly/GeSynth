#include "UI.hpp"
#include <math.h>

Slider::Slider(int x_, int y_, int w_, int h_, int min_, int max_, int value_, const std::string &label_)
    : x(x_), y(y_), w(w_), h(h_), min(min_), max(max_), value(value_), dragging(false), label(label_) {}

void Slider::draw(SDL_Renderer *renderer) const
{
    // Modern glassmorphism style - Outer glow/shadow
    for (int i = 6; i >= 0; i--)
    {
        SDL_Rect shadowRect = {x - 8 + i, y - 8 + i, w + 16 - i * 2, h + 16 - i * 2};
        int alpha = 25 - i * 3;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &shadowRect);
    }

    // Main background with subtle gradient
    SDL_Rect background = {x - 6, y - 6, w + 12, h + 12};
    drawGradientRect(renderer, background,
                     {30, 35, 45, 240}, {45, 50, 65, 240}, true);

    // Glass border effect
    SDL_SetRenderDrawColor(renderer, 100, 140, 180, 120);
    SDL_RenderDrawRect(renderer, &background);

    // Inner highlight for glass effect
    SDL_Rect innerHighlight = {background.x + 1, background.y + 1,
                               background.w - 2, background.h / 3};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 15);
    SDL_RenderFillRect(renderer, &innerHighlight);

    // Track groove (3D effect)
    SDL_Rect trackOuter = {x + 8, y + h / 2 - 5, w - 16, 10};
    SDL_SetRenderDrawColor(renderer, 15, 20, 25, 255);
    SDL_RenderFillRect(renderer, &trackOuter);

    SDL_Rect trackInner = {x + 10, y + h / 2 - 3, w - 20, 6};
    drawGradientRect(renderer, trackInner,
                     {25, 30, 35, 255}, {35, 40, 50, 255}, true);

    // Progress fill with neon glow
    int fillWidth = (value - min) * (w - 20) / (max - min);
    if (fillWidth > 2)
    {
        SDL_Rect progressTrack = {x + 10, y + h / 2 - 3, fillWidth, 6};

        // Multi-layer glow effect
        for (int i = 4; i >= 0; i--)
        {
            SDL_Rect glowRect = {progressTrack.x - i, progressTrack.y - i,
                                 progressTrack.w + i * 2, progressTrack.h + i * 2};
            int alpha = (4 - i) * 20;
            SDL_SetRenderDrawColor(renderer, 0, 180, 255, alpha);
            SDL_RenderFillRect(renderer, &glowRect);
        }

        // Main progress with gradient
        drawGradientRect(renderer, progressTrack,
                         {0, 220, 255, 255}, {0, 140, 255, 255}, false);

        // Progress shine
        SDL_Rect shine = {progressTrack.x, progressTrack.y, progressTrack.w, 2};
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 150);
        SDL_RenderFillRect(renderer, &shine);
    }

    // Futuristic knob
    int knobX = x + 10 + fillWidth;
    SDL_Rect knob = {knobX - 10, y + h / 2 - 12, 20, 24};

    // Knob glow (stronger when dragging)
    int glowSize = dragging ? 8 : 4;
    int glowIntensity = dragging ? 120 : 60;

    for (int i = glowSize; i >= 0; i--)
    {
        SDL_Rect glowRect = {knob.x - i * 2, knob.y - i * 2,
                             knob.w + i * 4, knob.h + i * 4};
        int alpha = glowIntensity - i * 12;
        if (alpha > 0)
        {
            SDL_SetRenderDrawColor(renderer, 120, 200, 255, alpha);
            SDL_RenderFillRect(renderer, &glowRect);
        }
    }

    // Knob body - 3D cylindrical effect
    drawGradientRect(renderer, knob,
                     {200, 220, 240, 255}, {160, 180, 200, 255}, true);

    // Knob center groove
    SDL_Rect centerGroove = {knob.x + 7, knob.y + 4, 6, knob.h - 8};
    SDL_SetRenderDrawColor(renderer, 100, 120, 140, 255);
    SDL_RenderFillRect(renderer, &centerGroove);

    // Knob indicator line
    SDL_Rect indicator = {knob.x + 8, knob.y + 2, 4, 6};
    SDL_SetRenderDrawColor(renderer, 0, 180, 255, 255);
    SDL_RenderFillRect(renderer, &indicator);

    // Knob border with metallic effect
    SDL_SetRenderDrawColor(renderer, 140, 160, 180, 255);
    SDL_RenderDrawRect(renderer, &knob);

    // Knob top highlight
    SDL_Rect topHighlight = {knob.x + 2, knob.y + 2, knob.w - 4, 4};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);
    SDL_RenderFillRect(renderer, &topHighlight);

    // Modern value display
    drawValueDisplay(renderer, x + w - 70, y - 35, value, min, max);
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
        return false;
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (dragging)
        {
            dragging = false;
            return true;
        }
        return false;
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        if (dragging)
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
    // Futuristic button with glow
    SDL_Rect buttonRect = {x, y, w, h};

    // Shadow/depth effect
    for (int i = 3; i >= 0; i--)
    {
        SDL_Rect shadowRect = {x + i, y + i, w - i * 2, h - i * 2};
        int alpha = 40 - i * 8;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &shadowRect);
    }

    // Main button background
    SDL_SetRenderDrawColor(renderer, 20, 25, 35, 240);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Waveform-specific neon glow
    SDL_Color glowColor;
    switch (currentWave)
    {
    case WaveForm::Sine:
        glowColor = {255, 100, 100, 255};
        break;
    case WaveForm::Square:
        glowColor = {100, 255, 100, 255};
        break;
    case WaveForm::Triangle:
        glowColor = {100, 100, 255, 255};
        break;
    case WaveForm::Saw:
        glowColor = {255, 255, 100, 255};
        break;
    }

    // Animated glow border
    for (int i = 3; i >= 0; i--)
    {
        SDL_Rect glowRect = {x - i, y - i, w + i * 2, h + i * 2};
        int alpha = 60 - i * 12;
        SDL_SetRenderDrawColor(renderer, glowColor.r, glowColor.g, glowColor.b, alpha);
        SDL_RenderDrawRect(renderer, &glowRect);
    }

    // Inner content area
    SDL_Rect contentRect = {x + 5, y + 5, w - 10, h - 10};
    SDL_SetRenderDrawColor(renderer, glowColor.r / 4, glowColor.g / 4, glowColor.b / 4, 200);
    SDL_RenderFillRect(renderer, &contentRect);

    // Enhanced waveform visualization
    SDL_SetRenderDrawColor(renderer, glowColor.r, glowColor.g, glowColor.b, 255);
    int centerY = y + h / 2;
    int amplitude = 12;
    int points = 20;

    for (int i = 0; i < points - 1; i++)
    {
        int x1 = x + 10 + i * (w - 20) / (points - 1);
        int x2 = x + 10 + (i + 1) * (w - 20) / (points - 1);

        float t1 = (float)i / (points - 1) * 4.0f * 3.14159f; // 2 cycles
        float t2 = (float)(i + 1) / (points - 1) * 4.0f * 3.14159f;

        int y1, y2;

        switch (currentWave)
        {
        case WaveForm::Sine:
            y1 = centerY - (int)(sin(t1) * amplitude);
            y2 = centerY - (int)(sin(t2) * amplitude);
            break;
        case WaveForm::Square:
            y1 = centerY - (sin(t1) > 0 ? amplitude : -amplitude);
            y2 = centerY - (sin(t2) > 0 ? amplitude : -amplitude);
            break;
        case WaveForm::Triangle:
            y1 = centerY - (int)((2.0f * abs(2.0f * (t1 / (2 * 3.14159f) - floor(t1 / (2 * 3.14159f) + 0.5f))) - 1.0f) * amplitude);
            y2 = centerY - (int)((2.0f * abs(2.0f * (t2 / (2 * 3.14159f) - floor(t2 / (2 * 3.14159f) + 0.5f))) - 1.0f) * amplitude);
            break;
        case WaveForm::Saw:
            y1 = centerY - (int)((2.0f * (t1 / (2 * 3.14159f) - floor(t1 / (2 * 3.14159f) + 0.5f))) * amplitude);
            y2 = centerY - (int)((2.0f * (t2 / (2 * 3.14159f) - floor(t2 / (2 * 3.14159f) + 0.5f))) * amplitude);
            break;
        }

        // Draw thicker line for better visibility
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer, x1, y1 + 1, x2, y2 + 1);
    }

    // Glass effect border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 60);
    SDL_RenderDrawRect(renderer, &buttonRect);
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

// LFOTargetSelector implementation
LFOTargetSelector::LFOTargetSelector(int x_, int y_, int w_, int h_)
    : x(x_), y(y_), w(w_), h(h_), currentTarget(LFOTarget::None)
{
    targetNames[0] = "OFF";
    targetNames[1] = "PITCH";
    targetNames[2] = "AMP";
    targetNames[3] = "FILTER";
}

void LFOTargetSelector::draw(SDL_Renderer *renderer) const
{
    // Ultra-modern HUD-style selector
    SDL_Rect mainRect = {x, y, w, h};

    // Deep shadow for depth
    for (int i = 4; i >= 0; i--)
    {
        SDL_Rect shadowRect = {x + i, y + i, w - i * 2, h - i * 2};
        int alpha = 30 - i * 5;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &shadowRect);
    }

    // Main background - dark tech look
    SDL_SetRenderDrawColor(renderer, 15, 20, 30, 245);
    SDL_RenderFillRect(renderer, &mainRect);

    // Color-coded neon targets
    SDL_Color targetColors[4] = {
        {60, 60, 70, 255},   // None - Dim Gray
        {255, 80, 120, 255}, // Pitch - Hot Pink
        {80, 255, 120, 255}, // Amplitude - Neon Green
        {80, 120, 255, 255}  // Filter - Electric Blue
    };

    SDL_Color currentColor = targetColors[static_cast<int>(currentTarget)];

    // Holographic display grid
    SDL_SetRenderDrawColor(renderer, 0, 100, 150, 60);
    for (int i = 0; i < 4; i++)
    {
        int lineY = y + 10 + i * 8;
        SDL_RenderDrawLine(renderer, x + 8, lineY, x + w - 8, lineY);
    }

    // Target visualization zones
    int zoneWidth = (w - 20) / 4;
    for (int i = 0; i < 4; i++)
    {
        SDL_Rect zone = {x + 10 + i * zoneWidth, y + 8, zoneWidth - 4, h - 16};

        if (i == static_cast<int>(currentTarget))
        {
            // Active zone with intense glow
            for (int g = 5; g >= 0; g--)
            {
                SDL_Rect glowZone = {zone.x - g, zone.y - g, zone.w + g * 2, zone.h + g * 2};
                int alpha = 80 - g * 10;
                SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, alpha);
                SDL_RenderFillRect(renderer, &glowZone);
            }

            // Core active area
            SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, 200);
            SDL_RenderFillRect(renderer, &zone);

            // Pulsing effect
            SDL_Rect pulseRect = {zone.x + 2, zone.y + 2, zone.w - 4, zone.h - 4};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 120);
            SDL_RenderFillRect(renderer, &pulseRect);
        }
        else
        {
            // Inactive zone - subtle presence
            SDL_SetRenderDrawColor(renderer, targetColors[i].r / 3, targetColors[i].g / 3, targetColors[i].b / 3, 150);
            SDL_RenderFillRect(renderer, &zone);
        }

        // Zone border
        SDL_SetRenderDrawColor(renderer, targetColors[i].r / 2, targetColors[i].g / 2, targetColors[i].b / 2, 255);
        SDL_RenderDrawRect(renderer, &zone);
    }

    // Futuristic indicator symbols
    drawTargetSymbol(renderer, x + 15, y + h / 2 - 5, 0, currentTarget == LFOTarget::None);      // None
    drawTargetSymbol(renderer, x + 35, y + h / 2 - 5, 1, currentTarget == LFOTarget::Pitch);     // Pitch
    drawTargetSymbol(renderer, x + 55, y + h / 2 - 5, 2, currentTarget == LFOTarget::Amplitude); // Amplitude
    drawTargetSymbol(renderer, x + 75, y + h / 2 - 5, 3, currentTarget == LFOTarget::Filter);    // Filter

    // High-tech border
    SDL_SetRenderDrawColor(renderer, 100, 150, 200, 180);
    SDL_RenderDrawRect(renderer, &mainRect);

    // Corner accents
    for (int i = 0; i < 4; i++)
    {
        SDL_Rect corner = {x + 2, y + 2, 4, 4};
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, 255);
        SDL_RenderFillRect(renderer, &corner);
    }
}

bool LFOTargetSelector::handleEvent(const SDL_Event &event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int mx = event.button.x, my = event.button.y;
        if (mx >= x && mx < x + w && my >= y && my < y + h)
        {
            nextTarget();
            return true;
        }
    }
    return false;
}

void LFOTargetSelector::nextTarget()
{
    currentTarget = static_cast<LFOTarget>((static_cast<int>(currentTarget) + 1) % 4);
}

// Slider helper functions for modern GUI
void Slider::drawGradientRect(SDL_Renderer *renderer, SDL_Rect rect,
                              SDL_Color start, SDL_Color end, bool vertical) const
{
    for (int i = 0; i < (vertical ? rect.h : rect.w); i++)
    {
        float ratio = (float)i / (vertical ? rect.h : rect.w);
        Uint8 r = start.r + (end.r - start.r) * ratio;
        Uint8 g = start.g + (end.g - start.g) * ratio;
        Uint8 b = start.b + (end.b - start.b) * ratio;
        Uint8 a = start.a + (end.a - start.a) * ratio;

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        if (vertical)
        {
            SDL_RenderDrawLine(renderer, rect.x, rect.y + i, rect.x + rect.w - 1, rect.y + i);
        }
        else
        {
            SDL_RenderDrawLine(renderer, rect.x + i, rect.y, rect.x + i, rect.y + rect.h - 1);
        }
    }
}

void Slider::drawValueDisplay(SDL_Renderer *renderer, int x, int y, int val, int minVal, int maxVal) const
{
    // Futuristic HUD-style value display
    SDL_Rect displayBg = {x, y, 60, 25};

    // Background with depth
    SDL_SetRenderDrawColor(renderer, 5, 10, 15, 220);
    SDL_RenderFillRect(renderer, &displayBg);

    // Neon border
    SDL_SetRenderDrawColor(renderer, 0, 200, 255, 200);
    SDL_RenderDrawRect(renderer, &displayBg);

    // Inner glow
    SDL_Rect innerGlow = {x + 1, y + 1, 58, 23};
    SDL_SetRenderDrawColor(renderer, 0, 100, 150, 30);
    SDL_RenderFillRect(renderer, &innerGlow);

    // Value bar visualization
    int normalizedVal = (val - minVal) * 50 / (maxVal - minVal);
    for (int i = 0; i < 12; i++)
    {
        SDL_Rect segment = {x + 5 + i * 4, y + 18, 2, 4};
        if (i * 4 < normalizedVal)
        {
            // Active segments with intensity based on position
            int intensity = 100 + (i * 15);
            SDL_SetRenderDrawColor(renderer, 0, intensity, 255, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 40, 80, 255);
        }
        SDL_RenderFillRect(renderer, &segment);
    }

    // Digital number display
    drawDigit(renderer, x + 8, y + 3, (val / 100) % 10);
    drawDigit(renderer, x + 16, y + 3, (val / 10) % 10);
    drawDigit(renderer, x + 24, y + 3, val % 10);
}

void Slider::drawDigit(SDL_Renderer *renderer, int x, int y, int digit) const
{
    // 7-segment display style
    SDL_SetRenderDrawColor(renderer, 0, 255, 200, 255);

    // Segment patterns for digits 0-9
    bool segments[10][7] = {
        {1, 1, 1, 1, 1, 1, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1}, // 2
        {1, 1, 1, 1, 0, 0, 1}, // 3
        {0, 1, 1, 0, 0, 1, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1}  // 9
    };

    if (digit < 0 || digit > 9)
        return;

    // Draw segments
    if (segments[digit][0])
        SDL_RenderDrawLine(renderer, x + 1, y, x + 5, y); // top
    if (segments[digit][1])
        SDL_RenderDrawLine(renderer, x + 6, y + 1, x + 6, y + 5); // top right
    if (segments[digit][2])
        SDL_RenderDrawLine(renderer, x + 6, y + 7, x + 6, y + 11); // bottom right
    if (segments[digit][3])
        SDL_RenderDrawLine(renderer, x + 1, y + 12, x + 5, y + 12); // bottom
    if (segments[digit][4])
        SDL_RenderDrawLine(renderer, x, y + 7, x, y + 11); // bottom left
    if (segments[digit][5])
        SDL_RenderDrawLine(renderer, x, y + 1, x, y + 5); // top left
    if (segments[digit][6])
        SDL_RenderDrawLine(renderer, x + 1, y + 6, x + 5, y + 6); // middle
}

void Slider::drawRoundedRect(SDL_Renderer *renderer, SDL_Rect rect, int radius) const
{
    // Simple rounded corners
    SDL_Rect center = {rect.x + radius, rect.y, rect.w - 2 * radius, rect.h};
    SDL_RenderFillRect(renderer, &center);

    SDL_Rect left = {rect.x, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &left);

    SDL_Rect right = {rect.x + rect.w - radius, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &right);
}

void Slider::drawRoundedRectBorder(SDL_Renderer *renderer, SDL_Rect rect, int radius) const
{
    // Simple border drawing
    SDL_RenderDrawLine(renderer, rect.x + radius, rect.y, rect.x + rect.w - radius, rect.y);
    SDL_RenderDrawLine(renderer, rect.x + radius, rect.y + rect.h - 1, rect.x + rect.w - radius, rect.y + rect.h - 1);
    SDL_RenderDrawLine(renderer, rect.x, rect.y + radius, rect.x, rect.y + rect.h - radius);
    SDL_RenderDrawLine(renderer, rect.x + rect.w - 1, rect.y + radius, rect.x + rect.w - 1, rect.y + rect.h - radius);
}

void LFOTargetSelector::drawTargetSymbol(SDL_Renderer *renderer, int x, int y, int type, bool active) const
{
    SDL_Color color = active ? SDL_Color{255, 255, 255, 255} : SDL_Color{100, 100, 120, 255};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    switch (type)
    {
    case 0: // None - Empty circle
        for (int i = 0; i < 8; i++)
        {
            int px = x + 4 + cos(i * 3.14159f / 4) * 3;
            int py = y + 4 + sin(i * 3.14159f / 4) * 3;
            SDL_RenderDrawPoint(renderer, px, py);
        }
        break;
    case 1: // Pitch - Wave pattern
        for (int i = 0; i < 8; i++)
        {
            int py = y + 4 + sin(i * 3.14159f / 2) * 2;
            SDL_RenderDrawPoint(renderer, x + i, py);
        }
        break;
    case 2: // Amplitude - Vertical bars
        SDL_RenderDrawLine(renderer, x + 2, y + 2, x + 2, y + 6);
        SDL_RenderDrawLine(renderer, x + 4, y + 1, x + 4, y + 7);
        SDL_RenderDrawLine(renderer, x + 6, y + 3, x + 6, y + 5);
        break;
    case 3: // Filter - Diamond
        SDL_RenderDrawLine(renderer, x + 4, y + 1, x + 6, y + 4);
        SDL_RenderDrawLine(renderer, x + 6, y + 4, x + 4, y + 7);
        SDL_RenderDrawLine(renderer, x + 4, y + 7, x + 2, y + 4);
        SDL_RenderDrawLine(renderer, x + 2, y + 4, x + 4, y + 1);
        break;
    }
}
