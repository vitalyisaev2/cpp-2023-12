#include "editor_controller.hpp"
#include "editor_model.hpp"
#include "editor_view.hpp"
#include "shape.hpp"

#include <memory>

int main() {
    // Create objects comprising the MVC pattern
    auto editorView = std::make_unique<NEditor::TEditorView>();
    auto editorModel = std::make_unique<NEditor::TEditorModel>(std::move(editorView));
    auto editorController = NEditor::TEditorController(std::move(editorModel));

    // draw line $y = 2x - 5$
    auto line1 = editorController.AddShape<NEditor::TLine>(2, -5);
    // draw line $y = x + 5$
    auto line2 = editorController.AddShape<NEditor::TLine>(1, 5);
    // draw square
    auto square1 = editorController.AddShape<NEditor::TSquare>(20, 5, 4);

    // drop first line
    editorController.DeleteShape(line1);

    // editor dump
    editorController.SaveToFile("/tmp/editor.dump");
}
