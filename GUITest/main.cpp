#include <vector>
#include <string>
#include <filesystem>

#include "DXMain.h"
#include "DXContext.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Logging.h"
#include "AsyncCallbacks.h"
#include "user_gui/ScriptWindow.h"

typedef void (*UserInit)();
typedef void (*UserClean)();
typedef void* (*UserGetObject)();

inline std::wstring OpenFile(const wchar_t* filter_name, const wchar_t* filter_ext)
{
	wchar_t filter[1024];
	wsprintf(filter, L"%s", filter_name);

	int pos = lstrlenW(filter);
	wsprintf(filter + pos + 1, L"%s", filter_ext);

	wchar_t buffer[1024];
	OPENFILENAMEW ofn{ 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = buffer;
	ofn.lpstrFile[0] = 0;
	ofn.nMaxFile = 1024;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	BOOL res = ::GetOpenFileName(&ofn);
	if (res)
	{
		return buffer;
	}
	else
	{
		return L"";
	}
}

class Test : public DXMain
{
public:
	Test(const wchar_t* title, int width, int height)
		: DXMain(title, width, height)
	{	
		Logging::SetPrintCallbacks(this, print, print);
		this->SetFramerate(60.0f);
	}

	~Test()
	{
		UnloadScript();

	}

	void LoadScript(const wchar_t* dir, const wchar_t* filename)
	{
		UnloadScript();
		std::filesystem::current_path(dir);
		m_module = ::LoadLibraryW(filename);
		UserInit init = (UserInit)::GetProcAddress((HMODULE)m_module, "Init");
		init();
		UserGetObject get_script_window = (UserGetObject)::GetProcAddress((HMODULE)m_module, "GetScriptWindow");
		if (get_script_window != nullptr)
		{
			m_script_window = (ScriptWindow*)get_script_window();
		}

	}

	void UnloadScript()
	{
		AsyncCallbacks::CheckPendings();
		if (m_module != nullptr)
		{
			UserClean clean = (UserClean)::GetProcAddress((HMODULE)m_module, "Clean");
			clean();
			::FreeLibrary(m_module);
			m_module = nullptr;
			m_script_window = nullptr;
		}
	}

protected:
	void idle() override
	{
		AsyncCallbacks::CheckPendings();
	}

	void paint(int width, int height) override
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		{
			ImGui::Begin("Launcher");

			if (ImGui::Button("Load Script"))
			{
				PostAction(s_load_script, this);
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear Log"))
			{
				m_console_text = "";
			}

			ImGui::Text("Console log:");
			ImGui::BeginChild("ConsoleLog", { 0, 0 }, true, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::Text(m_console_text.c_str());
			ImGui::EndChild();

			ImGui::End();
		}

		if (m_script_window != nullptr)
		{
			if (m_script_window->show)
			{
				m_script_window->Draw(this);
			}
		}

		ImGui::Render();

		DXContext* dxctx = get_context();
		const float clear_color_with_alpha[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
		dxctx->m_pContext->OMSetRenderTargets(1, &dxctx->m_pRenderTargetView, NULL);
		dxctx->m_pContext->ClearRenderTargetView(dxctx->m_pRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}

private:
	HMODULE m_module = nullptr;
	ScriptWindow* m_script_window = nullptr;

	std::string m_console_text;

	static void print(void* ptr, const char* str)
	{
		Test* self = (Test*)ptr;
		self->m_console_text += str;
		self->m_console_text += "\n";
	}

	static void s_load_script(void* ptr)
	{
		Test* self = (Test*)ptr;
		std::wstring file_path = OpenFile(L"Cpp Dll", L"*.dll");
		if (file_path != L"")
		{
			std::filesystem::path path(file_path);
			std::wstring filename = path.filename().wstring();
			std::wstring directory = path.parent_path().wstring();
			self->LoadScript(directory.c_str(), filename.c_str());
		}
	}
};

int main()
{
	Test test(L"GUITest", 1280, 720);
	test.MainLoop();

	return 0;
}

