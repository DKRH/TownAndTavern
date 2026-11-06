#include "TopDownLevel.h"

#include <algorithm>
#include <fstream>
#include <utility>

namespace
{
    constexpr int paletteCount = 6;
    constexpr int paletteCardWidth = 130;
    constexpr int paletteStartX = 8;
    constexpr char levelFileName[] = "town_level.txt";

    const char* TileName(TopDownLevel::Tile tile)
    {
        switch (tile)
        {
        case TopDownLevel::Tile::Grass: return "Grass";
        case TopDownLevel::Tile::Path: return "Path";
        case TopDownLevel::Tile::Water: return "Water";
        case TopDownLevel::Tile::Tree: return "Tree";
        case TopDownLevel::Tile::Building: return "Building";
        case TopDownLevel::Tile::TavernDoor: return "Door";
        }
        return "Unknown";
    }

    Color TileColor(TopDownLevel::Tile tile)
    {
        switch (tile)
        {
        case TopDownLevel::Tile::Grass: return { 94, 166, 83, 255 };
        case TopDownLevel::Tile::Path: return { 202, 174, 117, 255 };
        case TopDownLevel::Tile::Water: return { 58, 126, 181, 255 };
        case TopDownLevel::Tile::Tree: return DARKGREEN;
        case TopDownLevel::Tile::Building: return { 143, 78, 55, 255 };
        case TopDownLevel::Tile::TavernDoor: return { 191, 128, 68, 255 };
        }
        return MAGENTA;
    }
}

TopDownLevel::TopDownLevel()
    : tiles_(columns * rows, Tile::Grass)
{
    // A small default town layout. The door tile is also the entrance trigger.
    for (int column = 0; column < columns; ++column)
        tiles_[Index(column, 7)] = Tile::Path;
    for (int row = 0; row < rows; ++row)
        tiles_[Index(12, row)] = Tile::Path;

    for (int row = 1; row <= 4; ++row)
    {
        for (int column = 7; column <= 17; ++column)
        {
            if (row == 4 && column == 12) continue;
            tiles_[Index(column, row)] = Tile::Building;
        }
    }
    tiles_[Index(12, 4)] = Tile::TavernDoor;
    tiles_[Index(3, 3)] = Tile::Tree;
    tiles_[Index(21, 7)] = Tile::Tree;
    tiles_[Index(5, 9)] = Tile::Water;
    tiles_[Index(6, 9)] = Tile::Water;
    tiles_[Index(7, 9)] = Tile::Water;

    // Reuse the last saved map on the next launch when one is available.
    (void)Load();
}

int TopDownLevel::Index(int column, int row) const
{
    return row * columns + column;
}

TopDownLevel::Tile TopDownLevel::GetTile(int column, int row) const
{
    if (column < 0 || column >= columns || row < 0 || row >= rows) return Tile::Grass;
    return tiles_[Index(column, row)];
}

void TopDownLevel::DrawTile(int column, int row, Tile tile) const
{
    const int x = column * tileSize;
    const int y = row * tileSize;
    const Rectangle cell = { static_cast<float>(x), static_cast<float>(y),
                             static_cast<float>(tileSize), static_cast<float>(tileSize) };
    DrawRectangleRec(cell, TileColor(tile));

    switch (tile)
    {
    case Tile::Water:
        DrawLine(x + 5, y + 11, x + 25, y + 11, SKYBLUE);
        DrawLine(x + 8, y + 23, x + 27, y + 23, SKYBLUE);
        break;
    case Tile::Tree:
        DrawRectangle(x + 13, y + 18, 7, 12, BROWN);
        DrawCircle(x + 16, y + 13, 13, DARKGREEN);
        DrawCircle(x + 11, y + 14, 8, GREEN);
        break;
    case Tile::Building:
        DrawRectangle(x + 3, y + 3, 26, 26, { 166, 94, 62, 255 });
        DrawRectangle(x + 3, y + 3, 26, 5, { 95, 48, 42, 255 });
        DrawLine(x + 4, y + 29, x + 28, y + 29, { 95, 48, 42, 255 });
        break;
    case Tile::TavernDoor:
        DrawRectangle(x + 7, y + 2, 18, 30, { 68, 42, 37, 255 });
        DrawRectangle(x + 10, y + 8, 12, 24, { 191, 128, 68, 255 });
        DrawCircle(x + 19, y + 21, 1.5f, GOLD);
        break;
    case Tile::Grass:
    case Tile::Path:
        break;
    }
}

void TopDownLevel::DrawTown() const
{
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column)
            DrawTile(column, row, GetTile(column, row));
}

std::vector<Rectangle> TopDownLevel::GetTownObstacles() const
{
    std::vector<Rectangle> obstacles;
    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            const Tile tile = GetTile(column, row);
            if (tile == Tile::Tree || tile == Tile::Building || tile == Tile::Water)
                obstacles.push_back({ static_cast<float>(column * tileSize),
                                      static_cast<float>(row * tileSize),
                                      static_cast<float>(tileSize),
                                      static_cast<float>(tileSize) });
        }
    }
    return obstacles;
}

bool TopDownLevel::GetTavernDoor(Rectangle& door) const
{
    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            if (GetTile(column, row) == Tile::TavernDoor)
            {
                door = { static_cast<float>(column * tileSize), static_cast<float>(row * tileSize),
                         static_cast<float>(tileSize), static_cast<float>(tileSize) };
                return true;
            }
        }
    }
    return false;
}

bool TopDownLevel::IsMouseOnMap() const
{
    const Vector2 mouse = GetMousePosition();
    return mouse.x >= 0.0f && mouse.x < columns * tileSize &&
           mouse.y >= 0.0f && mouse.y < mapHeight;
}

void TopDownLevel::SetTileAtMouse(Tile tile)
{
    const Vector2 mouse = GetMousePosition();
    const int column = static_cast<int>(mouse.x) / tileSize;
    const int row = static_cast<int>(mouse.y) / tileSize;
    if (column >= 0 && column < columns && row >= 0 && row < rows)
        tiles_[Index(column, row)] = tile;
}

bool TopDownLevel::Save()
{
    std::ofstream file(levelFileName);
    if (!file) return false;
    file << columns << ' ' << rows << '\n';
    for (const Tile tile : tiles_) file << static_cast<int>(tile) << ' ';
    return static_cast<bool>(file);
}

bool TopDownLevel::Load()
{
    std::ifstream file(levelFileName);
    int fileColumns = 0;
    int fileRows = 0;
    if (!(file >> fileColumns >> fileRows) || fileColumns != columns || fileRows != rows)
        return false;

    std::vector<Tile> loaded;
    loaded.reserve(columns * rows);
    for (int i = 0; i < columns * rows; ++i)
    {
        int value = 0;
        if (!(file >> value) || value < 0 || value > static_cast<int>(Tile::TavernDoor))
            return false;
        loaded.push_back(static_cast<Tile>(value));
    }
    tiles_ = std::move(loaded);
    return true;
}

void TopDownLevel::UpdateEditor()
{
    statusTimer_ = std::max(0.0f, statusTimer_ - GetFrameTime());
    if (IsKeyPressed(KEY_F5))
    {
        statusMessage_ = Save() ? "Saved town_level.txt" : "Could not save town_level.txt";
        statusTimer_ = 2.0f;
    }
    if (IsKeyPressed(KEY_F9))
    {
        statusMessage_ = Load() ? "Loaded town_level.txt" : "Could not load town_level.txt";
        statusTimer_ = 2.0f;
    }

    const Vector2 mouse = GetMousePosition();
    bool overPalette = false;
    if (mouse.y >= paletteY)
    {
        const int card = static_cast<int>(mouse.x) / paletteCardWidth;
        overPalette = card >= 0 && card < paletteCount &&
                      mouse.x >= paletteStartX && mouse.x < paletteStartX + paletteCount * paletteCardWidth;
        if (overPalette && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            selectedTile_ = static_cast<Tile>(card);
            draggedTile_ = selectedTile_;
            draggingPaletteTile_ = true;
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsMouseOnMap())
        SetTileAtMouse(draggingPaletteTile_ ? draggedTile_ : selectedTile_);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        if (draggingPaletteTile_ && IsMouseOnMap()) SetTileAtMouse(draggedTile_);
        draggingPaletteTile_ = false;
    }
}

void TopDownLevel::DrawEditor() const
{
    DrawTown();
    for (int column = 0; column <= columns; ++column)
        DrawLine(column * tileSize, 0, column * tileSize, mapHeight, Fade(RAYWHITE, 0.28f));
    for (int row = 0; row <= rows; ++row)
        DrawLine(0, row * tileSize, columns * tileSize, row * tileSize, Fade(RAYWHITE, 0.28f));

    const Vector2 mouse = GetMousePosition();
    if (IsMouseOnMap())
    {
        const int column = static_cast<int>(mouse.x) / tileSize;
        const int row = static_cast<int>(mouse.y) / tileSize;
        DrawRectangleLines(column * tileSize, row * tileSize, tileSize, tileSize, YELLOW);
    }

    DrawRectangle(0, paletteY, 800, 66, { 35, 38, 45, 255 });
    for (int i = 0; i < paletteCount; ++i)
    {
        const Tile tile = static_cast<Tile>(i);
        const int x = paletteStartX + i * paletteCardWidth;
        DrawRectangle(x, paletteY + 7, 124, 49,
                      tile == selectedTile_ ? Color{ 73, 84, 104, 255 } : Color{ 51, 56, 66, 255 });
        DrawRectangle(x + 6, paletteY + 17, 28, 28, TileColor(tile));
        if (tile == Tile::Tree || tile == Tile::Building || tile == Tile::TavernDoor)
            DrawRectangleLines(x + 6, paletteY + 17, 28, 28, RAYWHITE);
        DrawText(TileName(tile), x + 41, paletteY + 23, 16, RAYWHITE);
    }

    DrawRectangle(0, 0, 258, 48, Fade(BLACK, 0.62f));
    DrawText("TOP-DOWN LEVEL EDITOR", 10, 7, 19, RAYWHITE);
    DrawText("Drag tiles, or select then paint | F5 save | F9 load | Esc exit", 10, 28, 13, RAYWHITE);
    if (statusTimer_ > 0.0f)
        DrawText(statusMessage_.c_str(), 560, 12, 18, YELLOW);
}
