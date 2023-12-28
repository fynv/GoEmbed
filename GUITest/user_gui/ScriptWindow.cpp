#include "ScriptWindow.h"
#include "Elements.h"
#include "imgui.h"

void ScriptWindow::add(Element* object)
{
	m_elements.push_back(object);
}

void ScriptWindow::remove(Element* object)
{
	auto iter = std::find(m_elements.begin(), m_elements.end(), object);
	if (iter != m_elements.end())
	{
		m_elements.erase(iter);
	}
}

void ScriptWindow::clear()
{
	m_elements.clear();
}

void ScriptWindow::Draw(DXMain* main_wnd)
{
	ImGui::Begin(title.c_str());	
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		m_elements[i]->Draw(main_wnd);
	}
	ImGui::End();
}
