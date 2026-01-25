#include "CurveEditor.h"

#include "imgui.h"
#include "implot.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
// Prevent windows.h from colliding/interfering with std::numeric_limits::max()
#define NOMINMAX
#include <fstream>
#include <windows.h>
#include <GL/gl.h>

#include "CurveLibAll.h"

// Data stored per platform window
struct WGL_WindowData { HDC hDC; };
static HGLRC            ghRC;
static WGL_WindowData   gMainWindow;
static int              gWidth;
static int              gHeight;

// Forward declarations of helper functions
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data);
void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool isIntegrationEnabled = true;
bool isLookupTableDrawn = true;
CurveLib::AreaAccumulator integrationAccumulator;
// Signed to match the IMGUI input function
int32_t numSamplesPerSegment = 32;

namespace CurveLib
{
    CurveEditor::CurveEditor()
    {
        // Test data
        CurveLib::BezierCurveSegment<CurveLib::Double2> defaultSegment1(std::vector<CurveLib::Double2> { {0, 0}, { 0.33, 1 }, { 0.67, 1 }, { 1, 0 } });
        CurveLib::BezierCurveSegment<CurveLib::Double2> defaultSegment2(std::vector<CurveLib::Double2> { { 1, 0 }, { 1.5, -1 }, { 1.75, -1 }, { 2, 0 } });
        defaultCurve = CurveLib::BezierCurve<CurveLib::Double2>({defaultSegment1, defaultSegment2});
    }

    CurveEditor::~CurveEditor()
    {
    }

    bool CurveEditor::Tick()
    {
        DrawGUI();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, gWidth, gHeight);
        static const ImVec4 sClearColor = ImVec4(0.1f, 0.f, 0.1f, 1.00f);
        glClearColor(sClearColor.x, sClearColor.y, sClearColor.z, sClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ::SwapBuffers(gMainWindow.hDC);

        return true;
    }

    void CurveEditor::DrawGUI()
    {
        using namespace CurveLib;

        static const ImVec4 pointColor{ 1, 0, 0, 1 };
        static const ImVec2 sMinWindowSize{ 800, 600 };
        static const ImVec2 sMaxWindowSize{ CurveLib::sFloatMax, CurveLib::sFloatMax };

        ImGui::SetNextWindowSizeConstraints(sMinWindowSize, sMaxWindowSize);

        if (!ImGui::Begin("Curve Editor"))
        {
            return;
        }

        if (ImGui::Button("Save"))
        {
            DrawSaveDialog();
        }

        ImGui::SameLine();
        if (ImGui::Button("Open"))
        {
            DrawOpenDialog();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Integrate", &isIntegrationEnabled);

        ImGui::SameLine();
        ImGui::Checkbox("Lookup", &isLookupTableDrawn);

        if (isIntegrationEnabled)
        {
            ImGui::SameLine();
            ImGui::PushItemWidth(100.f);
            ImGui::InputInt("Samples per segment", &numSamplesPerSegment);
            ImGui::PopItemWidth();
            numSamplesPerSegment = std::abs(numSamplesPerSegment);
        }

        // Default to the useful range for velocity curves, but let the user move and zoom the plot
        ImPlot::SetNextAxesLimits(0, 1, -1, 1, ImPlotCond_Once);

        if (ImPlot::BeginPlot("Curve", ImVec2(-1, -1), ImPlotFlags_NoBoxSelect))
        {
            // Draw the integration bars under everything else
            if (isIntegrationEnabled)
            {
                DrawIntegration();
            }

            // ImPlot expects doubles
            std::vector<double> sampleX{};
            std::vector<double> sampleY{};

            // Sample the curve to generate some lines for ImPlot to draw
            constexpr float SAMPLE_T_STEP = 0.01f;
            for (double t = 0; t < 2; t += SAMPLE_T_STEP)
            {
                const Double2 position = defaultCurve.CalculatePositionAtT(t);
                sampleX.push_back(position.X);
                sampleY.push_back(position.Y);
            }

            ImPlot::PlotLine("CurveLines", sampleX.data(), sampleY.data(), (int)sampleX.size());

            // Render from lookup table
            if (isLookupTableDrawn)
            {
                sampleX.clear();
                sampleY.clear();

                const auto& segments = defaultCurve.GetSegments();
                for (size_t segmentNum = 0; segmentNum < segments.size(); ++segmentNum)
                {
                    const BezierCurveSegment<Double2>& segment = segments[segmentNum];
                    const size_t lookupTableSize = segment.GetLookupSampleRate();
                    if (lookupTableSize == 0)
                    {
                        continue;
                    }

                    for (size_t i = 0; i < lookupTableSize; ++i)
                    {
                        const double xCoord = (double)i / lookupTableSize;

                        sampleX.push_back(xCoord + segmentNum);
                        sampleY.push_back((double)segment.CalculatePositionAtXCoordinate(xCoord).Y);
                    }
                    ImPlot::PlotLine("LookupTable", sampleX.data(), sampleY.data(), (int)sampleX.size());
                }
            }

            int plotPointID = 0;
            auto& segments = defaultCurve.AccessSegments();
            std::vector<double> tangentXs{};
            std::vector<double> tangentYs{};

            for (auto& segment : segments)
            {
                auto& points = segment.AccessPoints();
                for (auto& point : points)
                {
                    ImPlot::DragPoint(plotPointID++, &point.X, &point.Y, pointColor);
                }

                // Tangent lines
                tangentXs = { points[0].X, points[1].X };
                tangentYs = { points[0].Y, points[1].Y };
                ImPlot::PlotLine("Tangents1", tangentXs.data(), tangentYs.data(), 2);

                const Double2& secondToLastPoint = points.at(points.size() - 2);
                const Double2& lastPoint = points.at(points.size() - 1);
                tangentXs = { secondToLastPoint.X, lastPoint.X };
                tangentYs = { secondToLastPoint.Y, lastPoint.Y };
                ImPlot::PlotLine("Tangents2", tangentXs.data(), tangentYs.data(), 2);
            }

            ImPlot::EndPlot();
        }

        ImGui::End();
    }

    void CurveEditor::DrawIntegration()
    {
        std::vector<double> xCoords{};
        std::vector<double> yCoords{};
        std::vector<double> sampleAreas{};

        // TODO: Only recalculate when curve is dirty
        integrationAccumulator.Reset();

        const auto& segments = defaultCurve.GetSegments();
        const size_t numTotalSamples = numSamplesPerSegment * segments.size();

        for (int i = 0; i < numTotalSamples; ++i)
        {
            const double x = (double)i / numTotalSamples;
            const double curveY = defaultCurve.CalculatePositionAtXCoordinate(x).Y;

            integrationAccumulator.AccumulateArea((float)x, (float)curveY);

            xCoords.push_back(x);
            yCoords.push_back(curveY);
            sampleAreas.push_back(integrationAccumulator.GetTotalArea());
        }

        ImPlot::PlotBars("Integration", xCoords.data(), yCoords.data(), (int)xCoords.size(), 0.01);

        ImPlotPoint hoverPoint(-1, -1);
        if (ImPlot::IsPlotHovered())
        {
            // In plot coordinates
            hoverPoint = ImPlot::GetPlotMousePos();
        }

        if (hoverPoint.x > 0 && hoverPoint.y > 0 && segments.size() > 0)
        {
            size_t xIndex = (size_t)std::floor(hoverPoint.x * numTotalSamples / segments.size());
            xIndex = CurveLib::Clamp(xIndex, 0ULL, sampleAreas.size() - 1);

            ImPlot::Annotation(hoverPoint.x, hoverPoint.y, ImVec4(0, 0, 0.5, 1), ImVec2(20, 20), false, "Sample: %u\nArea: %2.f", xIndex, sampleAreas[xIndex]);
        }
    }

    // Blocking function to open a Windows save dialog and save a curve as a .cvb binary file.
    void CurveEditor::DrawSaveDialog()
    {
        OPENFILENAME ofn;

        char szFileName[MAX_PATH] = "";

        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = (LPCSTR)"CurveLib Files (*.cvb)\0*.cvb\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = (LPSTR)szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = (LPCSTR)"cvb";

        BOOL isOK = GetSaveFileName(&ofn);
        if (isOK)
        {
            std::ofstream fileOut(ofn.lpstrFile);
            defaultCurve.ToBinary(fileOut);
            fileOut.close();
        }
    }

    void CurveEditor::DrawOpenDialog()
    {
        OPENFILENAME ofn;

        char szFileName[MAX_PATH] = "";

        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = (LPCSTR)"CurveLib Files (*.cvb)\0*.cvb\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = (LPSTR)szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = (LPCSTR)"cvb";

        BOOL isOK = GetOpenFileName(&ofn);
        if (isOK)
        {
            std::ifstream fileIn(ofn.lpstrFile);
            defaultCurve = CurveLib::BezierCurve<CurveLib::Double2>::FromBinary(fileIn);
            fileIn.close();
        }
    }
}

int main(int, char**)
{
    CurveLib::CurveEditor curveEditor;

    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Curve Editor", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Curve Editor", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize OpenGL
    if (!CreateDeviceWGL(hwnd, &gMainWindow))
    {
        CleanupDeviceWGL(hwnd, &gMainWindow);
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    wglMakeCurrent(gMainWindow.hDC, ghRC);

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Set up scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_InitForOpenGL(hwnd);
    ImGui_ImplOpenGL3_Init();

    

    while (true)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                return 0;
            }
        }

        if (::IsIconic(hwnd))
        {
            ::Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (!curveEditor.Tick())
        {
            break;
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    CleanupDeviceWGL(hwnd, &gMainWindow);
    wglDeleteContext(ghRC);
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!ghRC)
        ghRC = wglCreateContext(data->hDC);
    return true;
}

void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    wglMakeCurrent(nullptr, nullptr);
    ::ReleaseDC(hWnd, data->hDC);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            gWidth = LOWORD(lParam);
            gHeight = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

