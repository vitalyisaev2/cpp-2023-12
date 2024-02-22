#pragma once

#include "editor_model.hpp"
#include "shape.hpp"
#include <fstream>

namespace NEditor {

    class TEditorController {
    public:
        TEditorController() = delete;

        TEditorController(TEditorModel::TPtr model)
            : model(std::move(model)) {
        }

        void SaveToFile(const std::string& filename) {
            std::ofstream out(filename);
            model->ForEachShape([&](const IShape::TPtr& shape) {
                out << shape->Serialize() << std::endl;
            });
            out.close();
        }

        void LoadFromFile(const std::string& filename) {
            std::ifstream in(filename);
            // TODO:
            // Deserialize shapes from line and init model
            in.close();
        }


        template <class T, class... Args>
        IShape::TPtr AddShape(Args&&... args) {
            IShape::TPtr out = std::make_shared<T>(std::forward<Args>(args)...);

            model->AddShape(out);

            return out;
        }

        void DeleteShape(IShape::TPtr& shape) {
            model->DeleteShape(shape);
        }

    private:
        TEditorModel::TPtr model;
    };
}
