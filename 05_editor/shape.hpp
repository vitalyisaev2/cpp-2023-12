#pragma once

#include <cstddef>
#include <memory>
#include <sstream>
#include <vector>

namespace NEditor {
    class IShape {
    public:
        using TPtr = std::shared_ptr<IShape>;

        // Ask shape for Y coordinates corresponding the given X coordinate
        virtual void ComputePoints(int x, std::vector<int>& ys) const = 0;

        // Serialize to string
        virtual std::string Serialize() const = 0;
    };

    class TLine: public IShape {
    public:
        TLine() = delete;

        TLine(int k, int b)
            : k(k)
            , b(b) {
        }

        virtual void ComputePoints(int x, std::vector<int>& ys) const override {
            ys.push_back(k * x + b);
        };

        virtual std::string Serialize() const override {
            std::stringstream ss;
            ss << "line " << k << " " << b;
            return ss.str();
        }

    private:
        int k;
        int b;
    };

    class TSquare: public IShape {
    public:
        TSquare() = delete;

        TSquare(int centerX, int centerY, std::size_t sideLength)
            : centerX(centerX)
            , centerY(centerY)
            , sideLength(sideLength) {
            if (sideLength % 2 != 0) {
                throw "cannot work with odd side length now";
            }
        }

        virtual void ComputePoints(int x, std::vector<int>& ys) const override {
            if (x == centerX - int(sideLength) || x == centerX + int(sideLength)) {
                for (auto y = centerY - sideLength; y <= centerY + sideLength; y++) {
                    ys.push_back(y);
                }
                return;
            }

            if (x > centerX - int(sideLength) && x < centerX + int(sideLength)) {
                ys.push_back(centerY - sideLength);
                ys.push_back(centerY + sideLength);
                return;
            }

            return;
        };

        virtual std::string Serialize() const override {
            std::stringstream ss;
            ss << "square " << centerX << " " << centerY << " " << sideLength;
            return ss.str();
        }

    private:
        int centerX;
        int centerY;
        std::size_t sideLength;
    };

} //namespace NEditor
