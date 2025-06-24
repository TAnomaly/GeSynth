#include "Piano.hpp"

Piano::Piano() : noteNames{
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C5", ""
} {}

void Piano::draw(SDL_Renderer* renderer, int x, int y, int width, int height, int activeKey) const {
    int whiteKeyCount = 8;
    int whiteKeyWidth = width / whiteKeyCount;
    int blackKeyWidth = whiteKeyWidth / 2;
    int blackKeyHeight = height * 2 / 3;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color blue  = {0, 128, 255, 255};
    // Beyaz tuşlar
    for (int i = 0; i < whiteKeyCount; ++i) {
        SDL_Rect keyRect = {x + i * whiteKeyWidth, y, whiteKeyWidth, height};
        if (activeKey == i * 2) {
            SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
        } else {
            SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
        }
        SDL_RenderFillRect(renderer, &keyRect);
        SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
        SDL_RenderDrawRect(renderer, &keyRect);
    }
    // Siyah tuşlar
    for (int i = 0; i < whiteKeyCount - 1; ++i) {
        int blackKeyIndex = i * 2 + 1;
        if (blackKeyIndex >= NUM_KEYS - 1) break;
        if (blackKeyIndex == 5 || blackKeyIndex == 12) continue; // E-F ve B-C arası siyah yok
        SDL_Rect keyRect = {x + (i + 1) * whiteKeyWidth - blackKeyWidth / 2, y, blackKeyWidth, blackKeyHeight};
        if (activeKey == blackKeyIndex) {
            SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
        } else {
            SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
        }
        SDL_RenderFillRect(renderer, &keyRect);
    }
}

int Piano::getKeyAtPosition(int px, int py, int pianoX, int pianoY, int pianoWidth, int pianoHeight) const {
    int whiteKeyCount = 8;
    int whiteKeyWidth = pianoWidth / whiteKeyCount;
    int blackKeyWidth = whiteKeyWidth / 2;
    int blackKeyHeight = pianoHeight * 2 / 3;
    // Önce siyah tuşlara bak
    for (int i = 0; i < whiteKeyCount - 1; ++i) {
        int blackKeyIndex = i * 2 + 1;
        if (blackKeyIndex >= NUM_KEYS - 1) break;
        if (blackKeyIndex == 5 || blackKeyIndex == 12) continue;
        int bx = pianoX + (i + 1) * whiteKeyWidth - blackKeyWidth / 2;
        int by = pianoY;
        if (px >= bx && px < bx + blackKeyWidth && py >= by && py < by + blackKeyHeight) {
            return blackKeyIndex;
        }
    }
    // Sonra beyaz tuşlara bak
    for (int i = 0; i < whiteKeyCount; ++i) {
        int wx = pianoX + i * whiteKeyWidth;
        int wy = pianoY;
        if (px >= wx && px < wx + whiteKeyWidth && py >= wy && py < wy + pianoHeight) {
            return i * 2;
        }
    }
    return -1;
}

const std::string& Piano::getNoteName(int key) const {
    if (key >= 0 && key < (int)noteNames.size())
        return noteNames[key];
    static std::string empty = "";
    return empty;
}
