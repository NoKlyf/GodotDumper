#define IMGUI_DEFINE_MATH_OPERATORS
#include "render.h"
#include "godot.h"

#include <dwmapi.h>
#include <cstdio>
#include <chrono>
#include <thread>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
        {
            return 0;
        }
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_F4) {
            DestroyWindow(hwnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

render_t::render_t()
{
    detail = std::make_unique<detail_t>();
}

render_t::~render_t()
{
    destroy_imgui();
    destroy_window();
    destroy_device();
}

bool render_t::create_window()
{
    detail->window_class.cbSize = sizeof(detail->window_class);
    detail->window_class.style = CS_CLASSDC;
    detail->window_class.lpszClassName = "T4";
    detail->window_class.hInstance = GetModuleHandleA(0);
    detail->window_class.lpfnWndProc = wnd_proc;

    RegisterClassExA(&detail->window_class);

    detail->window = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        detail->window_class.lpszClassName,
        "T4",
        WS_POPUP,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        0,
        0,
        detail->window_class.hInstance,
        0
    );

    if (!detail->window)
    {
        return false;
    }

    SetLayeredWindowAttributes(detail->window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    RECT client_area{};
    RECT window_area{};

    GetClientRect(detail->window, &client_area);
    GetWindowRect(detail->window, &window_area);

    POINT diff{};
    ClientToScreen(detail->window, &diff);

    MARGINS margins
    {
        window_area.left + (diff.x - window_area.left),
        window_area.top + (diff.y - window_area.top),
        window_area.right,
        window_area.bottom,
    };

    DwmExtendFrameIntoClientArea(detail->window, &margins);

    ShowWindow(detail->window, SW_SHOW);
    UpdateWindow(detail->window);

    return true;
}

bool render_t::create_device()
{
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};

    swap_chain_desc.BufferCount = 1;

    swap_chain_desc.BufferDesc.Width = 0;
    swap_chain_desc.BufferDesc.Height = 0;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    swap_chain_desc.OutputWindow = detail->window;

    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.Windowed = 1;

    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    swap_chain_desc.SampleDesc.Count = 2;
    swap_chain_desc.SampleDesc.Quality = 0;

    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    D3D_FEATURE_LEVEL feature_level;
    D3D_FEATURE_LEVEL feature_level_list[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        feature_level_list,
        2,
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        &detail->swap_chain,
        &detail->device,
        &feature_level,
        &detail->device_context
    );

    if (result == DXGI_ERROR_UNSUPPORTED)
    {
        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            feature_level_list,
            2,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &detail->swap_chain,
            &detail->device,
            &feature_level,
            &detail->device_context
        );
    }

    if (result != S_OK)
    {
        MessageBoxA(nullptr, "This software can not run on your computer.", "Critical Problem", MB_ICONERROR | MB_OK);
    }

    ID3D11Texture2D* back_buffer{ nullptr };
    detail->swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

    if (back_buffer)
    {
        detail->device->CreateRenderTargetView(back_buffer, nullptr, &detail->render_target_view);
        back_buffer->Release();

        return true;
    }

    return false;
}

bool render_t::create_imgui()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    io.Fonts->AddFontDefault();
    ImGui::MergeIconsWithLatestFont(10.f);

    if (!ImGui_ImplWin32_Init(detail->window))
    {
        return false;
    }

    if (!detail->device || !detail->device_context)
    {
        return false;
    }

    if (!ImGui_ImplDX11_Init(detail->device, detail->device_context))
    {
        return false;
    }

    return true;
}

void render_t::destroy_device()
{
    if (detail->render_target_view) detail->render_target_view->Release();
    if (detail->swap_chain) detail->swap_chain->Release();
    if (detail->device_context) detail->device_context->Release();
    if (detail->device) detail->device->Release();
}

void render_t::destroy_window()
{
    DestroyWindow(detail->window);
    UnregisterClassA(detail->window_class.lpszClassName, detail->window_class.hInstance);
}

void render_t::destroy_imgui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void render_t::start_render()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        running = !running;

        if (running)
        {
            SetWindowLong(detail->window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
        }
        else
        {
            SetWindowLong(detail->window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED);
        }
    }
}

void render_t::end_render()
{
    ImGui::RenderNotifications();
    ImGui::Render();

    float clear_color[4]{ 0, 0, 0, 0 };
    detail->device_context->OMSetRenderTargets(1, &detail->render_target_view, nullptr);
    detail->device_context->ClearRenderTargetView(detail->render_target_view, clear_color);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    detail->swap_chain->Present(0, 0);
}

static gd::Node* current_node = nullptr;
static gd::Node* last_scene = nullptr;

static void recursive_draw(gd::Node* node)
{
    if (!node)
        return;

    std::string name = std::format("{} ({})##{}", node->get_name(), node->get_class_name(), (std::uintptr_t)node);

    if (ImGui::TreeNode(name.c_str()))
    {
        current_node = node;

        LocalVector<gd::Node*> children = node->get_children();
        for (gd::Node* child : children)
            recursive_draw(child);

        ImGui::TreePop();
    }
}

void render_t::render_menu()
{
    ImGui::SetNextWindowSize({ 400, 400 }, ImGuiCond_Always);

    ImGui::Begin("Godot Explorer");
    recursive_draw(gd::SceneTree::get_singleton()->get_current_scene());
    ImGui::End();

    if (gd::SceneTree::get_singleton()->get_current_scene() != last_scene)
        current_node = nullptr;

    if (current_node != nullptr)
    {
        std::string class_name = current_node->get_class_name();

        ImGui::Begin("Node properties");
        ImGui::Text("Name: %s", current_node->get_name().c_str());
        ImGui::Text("Class: %s", class_name.c_str());
        ImGui::Separator();

        // TODO: Fix gd::Object::inherits_from
        if (class_name.contains("3D"))
        {
            ImGui::InputFloat("Position X", &current_node->as<gd::Node3D>()->local_transform.origin.x);
            ImGui::InputFloat("Position Y", &current_node->as<gd::Node3D>()->local_transform.origin.y);
            ImGui::InputFloat("Position Z", &current_node->as<gd::Node3D>()->local_transform.origin.z);
        }
        else if (class_name.contains("2D"))
        {
            ImGui::InputFloat("Position X", &current_node->as<gd::Node2D>()->position.x);
            ImGui::InputFloat("Position Y", &current_node->as<gd::Node2D>()->position.y);
        }

        if (class_name == "Camera3D")
            ImGui::InputFloat("FOV ", &current_node->as<gd::Camera3D>()->fov);

        if (current_node == last_scene)
            ImGui::Text("Scene path: %s", current_node->get_scene_file_path().c_str());

        ImGui::Separator();
        if (current_node->get_parent() != nullptr)
            ImGui::Text("Parent: %s", current_node->get_parent()->get_name().c_str());

        if (current_node->get_owner() != nullptr)
            ImGui::Text("Owner: %s", current_node->get_owner()->get_name().c_str());

        ImGui::Separator();
        ImGui::Text("Address: %p", current_node);
        ImGui::End();
    }
}

void render_t::render_visuals()
{
    if (gd::SceneTree::get_singleton()->get_current_scene() != nullptr)
    {
        if (gd::SceneTree::get_singleton()->get_current_scene() != last_scene)
        {
            std::string notification = std::format("Loaded new scene: {}", gd::SceneTree::get_singleton()->get_current_scene()->get_scene_file_path());
            ImGui::InsertNotification({ ImGuiToastType_Info, 3000, notification.c_str() });
        }
    }

    last_scene = gd::SceneTree::get_singleton()->get_current_scene();
}
