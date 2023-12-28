#include "UserScriptWindow.h"
#include "ScriptWindow.h"

void* ScriptWindow_New()
{
	return new ScriptWindow;
}

void ScriptWindow_Delete(void* ptr)
{
	delete (ScriptWindow*)ptr;
}

int ScriptWindow_GetShow(void* ptr)
{
	ScriptWindow* self = (ScriptWindow*)ptr;
	return self->show?1:0;
}

void ScriptWindow_SetShow(void* ptr, int show)
{
	ScriptWindow* self = (ScriptWindow*)ptr;
	self->show = show!=0;
}

const char* ScriptWindow_GetTitle(void* ptr)
{
	ScriptWindow* self = (ScriptWindow*)ptr;
	return self->title.c_str();
}

void ScriptWindow_SetTitle(void* ptr, const char* title)
{
	ScriptWindow* self = (ScriptWindow*)ptr;
	self->title = title;
}

void ScriptWindow_Add(void* ptr, void* ptr_elem)
{
	ScriptWindow* self = (ScriptWindow*)ptr;
	Element* elem = (Element*)ptr_elem;
	self->add(elem);
}


void ScriptWindow_Remove(void* ptr, void* ptr_elem)
{
	ScriptWindow* self = (ScriptWindow*)ptr;
	Element* elem = (Element*)ptr_elem;
	self->remove(elem);
}

void ScriptWindow_Clear(void* ptr)
{
	ScriptWindow* self = (ScriptWindow*)ptr;
	self->clear();
}
