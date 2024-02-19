#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <iostream>
#include <unordered_set>

#include "shape.hpp"

namespace NEditor {

    class TEditorView {
    public:
        using TPtr = std::unique_ptr<TEditorView>;

        void Draw(const std::unordered_set<IShape::TPtr>& shapes) {
            std::cout << ">>>>>>>>>>> FRAME #" << DrawCounter++ << " <<<<<<<<<<" << std::endl;

            Clear();
            AddCanvas();
            for (const auto& shape : shapes) {
                AddShape(shape);
            }
            Print();

            std::cout << std::endl;
        }

    private:
        std::size_t DrawCounter = 0;
        static constexpr std::size_t SideSize = 32;
        static constexpr char EmptySymbol = ' ';
        static constexpr char CanvasSymbol = 'X';
        static constexpr char PointSymbol = 'O';

        void Clear() {
            for (auto& line : screen) {
                for (auto& symbol : line) {
                    symbol = ' ';
                }
            }
        }

        void AddCanvas() {
            std::for_each(screen[0].begin(), screen[0].end(), [](char& c) { c = CanvasSymbol; });
            std::for_each(screen[SideSize - 1].begin(), screen[SideSize - 1].end(), [](char& c) { c = CanvasSymbol; });
            std::for_each(screen.begin(), screen.end(), [](std::array<char, SideSize>& line) {
                line.front() = CanvasSymbol;
                line.back() = CanvasSymbol;
            });
        }

        void AddShape(const IShape::TPtr& shape) {
            std::vector<int> coordsY;
            for (std::size_t coordX = 0; coordX < SideSize; coordX++) {
                shape->ComputePoints(coordX, coordsY);
                for (const auto coordY : coordsY) {
                    if (0 < coordX && coordX < SideSize && 0 < coordY && coordY < int(SideSize)) {
                        screen[coordX][coordY] = PointSymbol;
                    }
                }
                coordsY.clear();
            }
        }

        void Print() {
            // top to bottom
            for (int y = SideSize-1 ; y >= 0; y-- ) {
                for (int x = 0; x < int(SideSize); x++) {
                    std::cout << screen[x][y];
                }
                std::cout << std::endl;
            }
        }

        std::array<std::array<char, SideSize>, SideSize> screen;
    };
} //namespace NEditor
