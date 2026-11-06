#pragma once

#include "raylib.h"
#include <string>
#include <vector>

class TopDownLevel
{
public:
    enum class Tile : int { Grass, Path, Water, Tree, Building, TavernDoor };

    TopDownLevel();
    void UpdateEditor();
    void DrawEditor() const;
    void DrawTown() const;
    std::vector<Rectangle> GetTownObstacles() const;
    bool GetTavernDoor(Rectangle& door) const;

private:
    static constexpr int columns = 25;
    static constexpr int rows = 12;
    static constexpr int tileSize = 32;
    static constexpr int mapHeight = rows * tileSize;
    static constexpr int paletteY = mapHeight;

    int Index(int column, int row) const;
    Tile GetTile(int column, int row) const;
    void SetTileAtMouse(Tile tile);
    bool IsMouseOnMap() const;
    bool Save();
    bool Load();
    void DrawTile(int column, int row, Tile tile) const;

    std::vector<Tile> tiles_;
    Tile selectedTile_ = Tile::Path;
    bool draggingPaletteTile_ = false;
    Tile draggedTile_ = Tile::Path;
    std::string statusMessage_;
    float statusTimer_ = 0.0f;
};
