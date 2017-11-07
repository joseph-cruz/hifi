#include "HighlightStage.h"

using namespace render;

std::string HighlightStage::_name("Highlight");

HighlightStage::Index HighlightStage::addHighlight(const std::string& selectionName, const HighlightStyle& style) {
    Highlight outline{ selectionName, style };
    Index id;

    id = _highlights.newElement(outline);
    _activeHighlightIds.push_back(id);

    return id;
}

void HighlightStage::removeHighlight(Index index) {
    HighlightIdList::iterator  idIterator = std::find(_activeHighlightIds.begin(), _activeHighlightIds.end(), index);
    if (idIterator != _activeHighlightIds.end()) {
        _activeHighlightIds.erase(idIterator);
    }
    if (!_highlights.isElementFreed(index)) {
        _highlights.freeElement(index);
    }
}

Index HighlightStage::getHighlightIdBySelection(const std::string& selectionName) const {
    for (auto outlineId : _activeHighlightIds) {
        const auto& outline = _highlights.get(outlineId);
        if (outline._selectionName == selectionName) {
            return outlineId;
        }
    }
    return INVALID_INDEX;
}

const HighlightStyle& HighlightStageConfig::getStyle() const {
    auto styleIterator = _styles.find(_selectionName);
    if (styleIterator != _styles.end()) {
        return styleIterator->second;
    } else {
        auto insertion = _styles.insert(SelectionStyles::value_type{ _selectionName, HighlightStyle{} });
        return insertion.first->second;
    }
}

HighlightStyle& HighlightStageConfig::editStyle() {
    auto styleIterator = _styles.find(_selectionName);
    if (styleIterator != _styles.end()) {
        return styleIterator->second;
    } else {
        auto insertion = _styles.insert(SelectionStyles::value_type{ _selectionName, HighlightStyle{} });
        return insertion.first->second;
    }
}

void HighlightStageConfig::setSelectionName(const QString& name) {
    _selectionName = name.toStdString();
    emit dirty();
}

void HighlightStageConfig::setOutlineSmooth(bool isSmooth) {
    editStyle().isOutlineSmooth = isSmooth;
    emit dirty();
}

void HighlightStageConfig::setColorRed(float value) {
    editStyle().color.r = value;
    emit dirty();
}

void HighlightStageConfig::setColorGreen(float value) {
    editStyle().color.g = value;
    emit dirty();
}

void HighlightStageConfig::setColorBlue(float value) {
    editStyle().color.b = value;
    emit dirty();
}

void HighlightStageConfig::setOutlineWidth(float value) {
    editStyle().outlineWidth = value;
    emit dirty();
}

void HighlightStageConfig::setOutlineIntensity(float value) {
    editStyle().outlineIntensity = value;
    emit dirty();
}

void HighlightStageConfig::setUnoccludedFillOpacity(float value) {
    editStyle().unoccludedFillOpacity = value;
    emit dirty();
}

void HighlightStageConfig::setOccludedFillOpacity(float value) {
    editStyle().occludedFillOpacity = value;
    emit dirty();
}

HighlightStageSetup::HighlightStageSetup() {
}

void HighlightStageSetup::configure(const Config& config) {
    // Copy the styles here but update the stage with the new styles in run to be sure everything is
    // thread safe...
    _styles = config._styles;
}

void HighlightStageSetup::run(const render::RenderContextPointer& renderContext) {
    auto stage = renderContext->_scene->getStage<HighlightStage>(HighlightStage::getName());
    if (!stage) {
        stage = std::make_shared<HighlightStage>();
        renderContext->_scene->resetStage(HighlightStage::getName(), stage);
    }

    if (!_styles.empty()) {
        render::Transaction transaction;
        for (const auto& selection : _styles) {
            auto& selectionName = selection.first;
            auto& selectionStyle = selection.second;
            transaction.resetSelectionHighlight(selectionName, selectionStyle);
        }
        renderContext->_scene->enqueueTransaction(transaction);
        _styles.clear();
    }
}

