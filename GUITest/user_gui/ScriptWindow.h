#pragma once

#include <string>
#include <vector>

class Element;
class DXMain;
class ScriptWindow
{
public:
	bool show = false;
	std::string title = "Script";

	std::vector<Element*> m_elements;

	void add(Element* object);
	void remove(Element* object);
	void clear();

	void Draw(DXMain* main_wnd);
};
