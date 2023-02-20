#include "postProcess.hpp"

void engine::PostProcess::resolve(RenderTarget& MSAAInput) const
{
	m_postRenderProgram->bind();
	MSAAInput.bindPS();
	s_devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	s_devcon->Draw(3, 0);
}

void engine::PostProcess::clean()
{
	m_postRenderProgram = nullptr;
}
