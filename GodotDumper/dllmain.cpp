// Godot Dumper made by NoKlyf_
// It supports versions 4.3 and 4.4

/*
 * To set the version you want go to the preprocessor definitions
 * int project properties and edit or add a definition like this:
 * GODOT_VERSION_x_y
 * where x is the engine's major version and y is the engine's minor version
*/

#include <Windows.h>
#include <iostream>
#include "godot.h"
#include "render.h"

void WINAPI MainThread(HMODULE hModule)
{
    AllocConsole();
    FILE *out;
    freopen_s(&out, "CONOUT$", "w", stdout);

    SetConsoleTitleA(gd::SceneTree::get_singleton()->get_root()->get_title().append(" (Explorer by NoKlyf_)").c_str());

    std::cout << "[+] Base: " << std::hex << mem->get_base_address() << std::endl;
    std::cout << "[+] SceneTree: " << std::hex << gd::SceneTree::get_singleton() << std::endl;

    if (!render->create_window())
    {
        std::cout << "[-] Failed to create the overlay's window" << std::endl;

        fclose(out);
        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
    }

    if (!render->create_device())
    {
        std::cout << "[-] Failed to create the D3D11 Device" << std::endl;

        fclose(out);
        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
    }

    if (!render->create_imgui())
    {
        std::cout << "[-] Failed to initialize ImGui" << std::endl;

        fclose(out);
        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
    }

#ifdef GODOT_VERSION_4_4
    ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Dumper initialized! (Godot version: 4.4)" });
#elif defined GODOT_VERSION_4_3
    ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Explorer initialized! (Godot version: 4.3)" });
#endif

    while (true)
    {
        render->start_render();

        if (render->running)
            render->render_menu();

        render->render_visuals();
        render->end_render();
    }

    fclose(out);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL));
    }

    return TRUE;
}