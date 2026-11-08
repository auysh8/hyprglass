#include "GlassLayerCompositeElement.hpp"
#include "GlassLayerSurface.hpp"
#include "GlassRenderer.hpp"
#include "Globals.hpp"
#include "LayerGeometry.hpp"

CGlassLayerCompositeElement::CGlassLayerCompositeElement(const SGlassLayerCompositeData& data)
    : m_data(data) {}

std::vector<UP<IPassElement>> CGlassLayerCompositeElement::draw(Render::CRenderContext& ctx) {
    if (m_data.layerState)
        m_data.layerState->compositeAndRestore(ctx, ctx.m_data.pMonitor.lock(), m_data.alpha, m_data.maskSource);

    return {};
}

std::optional<CBox> CGlassLayerCompositeElement::boundingBox(Render::CRenderContext& ctx) {
    if (!m_data.layerState)
        return std::nullopt;

    const auto monitor = ctx.m_data.pMonitor.lock();
    auto layerSurface = m_data.layerState->getLayerSurface();
    if (layerSurface) {
        auto box = LayerGeometry::computePaddedLogicalLayerBox(layerSurface, monitor, GlassRenderer::SAMPLE_PADDING_PX);
        if (box)
            return box;
    }

    if (m_data.layerState->getLastRawBox().w > 0.0 && m_data.layerState->getLastRawBox().h > 0.0)
        return LayerGeometry::computePaddedLogicalLayerBox(m_data.layerState->getLastRawBox(), monitor, GlassRenderer::SAMPLE_PADDING_PX);

    return std::nullopt;
}

bool CGlassLayerCompositeElement::needsLiveBlur(Render::CRenderContext&) {
    // Always false: Hyprland's CRenderPass::render() asserts a bounding box
    // for any element reporting live blur ("No bounding box for an element
    // with live blur is illegal", Pass.cpp), and this element's boundingBox()
    // can be nullopt (e.g. layer torn down mid-frame).
    return false;
}

bool CGlassLayerCompositeElement::needsPrecomputeBlur(Render::CRenderContext&) {
    return false;
}
