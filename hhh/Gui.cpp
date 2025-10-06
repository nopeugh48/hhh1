// Gui.cpp
#include "Gui.h"
#include "Necklace.h"
#include "GemFactory.h"
#include "resource.h"

#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

// Глобальные объекты
HINSTANCE g_hInst = nullptr;
Necklace g_necklace;
DivisionCalculator g_calc;

// Идентификаторы элементов главного окна
constexpr int IDC_LISTBOX = 1001;
constexpr int IDC_ADD = 1002;
constexpr int IDC_REMOVE = 1003;
constexpr int IDC_EDIT = 1004;
constexpr int IDC_LABEL = 1005;

// Список типов камней — синхронизирован с GemFactory (чтобы createGem не выбрасывал)
static const std::vector<std::wstring> g_gemTypes = {
    L"Diamond",
    L"Ruby",
    L"Amethyst",
    L"Topaz"
};

// ----------------- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ -----------------

// UTF-8 (std::string) -> UTF-16 (std::wstring)
static std::wstring s2ws(const std::string& s) {
    if (s.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    std::wstring wstr(size_needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &wstr[0], size_needed);
    if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back();
    return wstr;
}

// UTF-16 (std::wstring) -> UTF-8 (std::string)
static std::string ws2s(const std::wstring& w) {
    if (w.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(size_needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &str[0], size_needed, NULL, NULL);
    if (!str.empty() && str.back() == '\0') str.pop_back();
    return str;
}

// ----------------- ДАННЫЕ ДИАЛОГА -----------------

struct GemDialogData {
    std::wstring type;
    std::wstring name;
    double price = 0.0;
    double weight = 0.0;
    bool accepted = false;
};

static GemDialogData g_dialogData;

// ----------------- UI: обновление списка и метки -----------------

static void RefreshUI(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LISTBOX);
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);

    for (size_t i = 0; i < g_necklace.count(); ++i) {
        auto gem = g_necklace.getAt(i);
        std::wstring item = s2ws(gem->typeName()) + L" (" + s2ws(gem->getName()) + L")";
        SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
    }

    double totalWeight = g_calc.totalWeight(g_necklace);
    double totalPrice = g_calc.totalPrice(g_necklace);

    std::wstringstream ss;
    ss << L"Общий вес: " << std::fixed << std::setprecision(2) << totalWeight
        << L" карат | Общая цена: " << std::fixed << std::setprecision(2) << totalPrice << L" у.е.";
    SetWindowTextW(GetDlgItem(hwnd, IDC_LABEL), ss.str().c_str());
}

// ----------------- Диалог добавления/редактирования -----------------

INT_PTR CALLBACK GemDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    (void)lParam; // параметр не используется, чтобы избежать предупреждений
    switch (msg) {
    case WM_INITDIALOG: {
        HWND hCombo = GetDlgItem(hDlg, IDC_TYPE);
        // Заполняем ComboBox доступными типами
        for (const auto& t : g_gemTypes) {
            SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)t.c_str());
        }

        // Устанавливаем выбранный тип (если редактирование — установим текущий)
        if (!g_dialogData.type.empty()) {
            for (size_t i = 0; i < g_gemTypes.size(); ++i) {
                if (g_gemTypes[i] == g_dialogData.type) {
                    SendMessageW(hCombo, CB_SETCURSEL, static_cast<WPARAM>(i), 0);
                    break;
                }
            }
        }
        else {
            SendMessageW(hCombo, CB_SETCURSEL, 0, 0); // выбрать первый по умолчанию
        }

        SetDlgItemTextW(hDlg, IDC_NAME, g_dialogData.name.c_str());

        {
            std::wostringstream oss;
            oss << std::fixed << std::setprecision(2) << g_dialogData.price;
            SetDlgItemTextW(hDlg, IDC_PRICE, oss.str().c_str());
        }
        {
            std::wostringstream oss;
            oss << std::fixed << std::setprecision(2) << g_dialogData.weight;
            SetDlgItemTextW(hDlg, IDC_WEIGHT, oss.str().c_str());
        }
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            HWND hCombo = GetDlgItem(hDlg, IDC_TYPE);
            int sel = static_cast<int>(SendMessageW(hCombo, CB_GETCURSEL, 0, 0));
            if (sel >= 0 && sel < static_cast<int>(g_gemTypes.size())) {
                g_dialogData.type = g_gemTypes[sel];
            }
            else {
                g_dialogData.type.clear();
            }

            wchar_t buf[512];
            GetDlgItemTextW(hDlg, IDC_NAME, buf, (int)std::size(buf));
            g_dialogData.name = buf;

            GetDlgItemTextW(hDlg, IDC_PRICE, buf, (int)std::size(buf));
            g_dialogData.price = _wtof(buf);

            GetDlgItemTextW(hDlg, IDC_WEIGHT, buf, (int)std::size(buf));
            g_dialogData.weight = _wtof(buf);

            // Валидация
            if (g_dialogData.name.empty()) {
                MessageBoxW(hDlg, L"Введите название камня.", L"Ошибка", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;
            }
            // Проверка вводимой цены (должно быть положительное число)
            if (!(g_dialogData.price > 0.0)) {
                MessageBoxW(hDlg, L"Цена должна быть положительным числом.", L"Ошибка", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            g_dialogData.accepted = true;
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            g_dialogData.accepted = false;
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// ----------------- Главное окно -----------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // ListBox — cast ID to HMENU via INT_PTR to avoid 64-bit warnings
        CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", nullptr,
            WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL,
            10, 10, 320, 200, hwnd, (HMENU)(INT_PTR)IDC_LISTBOX, g_hInst, nullptr);

        CreateWindowW(L"BUTTON", L"Добавить", WS_CHILD | WS_VISIBLE,
            350, 12, 120, 28, hwnd, (HMENU)(INT_PTR)IDC_ADD, g_hInst, nullptr);

        CreateWindowW(L"BUTTON", L"Удалить", WS_CHILD | WS_VISIBLE,
            350, 52, 120, 28, hwnd, (HMENU)(INT_PTR)IDC_REMOVE, g_hInst, nullptr);

        CreateWindowW(L"BUTTON", L"Редактировать", WS_CHILD | WS_VISIBLE,
            350, 92, 120, 28, hwnd, (HMENU)(INT_PTR)IDC_EDIT, g_hInst, nullptr);

        CreateWindowW(L"STATIC", L"Общий вес: 0.00 карат | Общая цена: 0.00 у.е.",
            WS_CHILD | WS_VISIBLE,
            10, 220, 480, 24, hwnd, (HMENU)(INT_PTR)IDC_LABEL, g_hInst, nullptr);

        // Опционный начальный набор (можно убрать)
        g_necklace.addGem(GemFactory::createGem("Diamond", "Brilliant Diamond", 5000.0, 1.20));
        g_necklace.addGem(GemFactory::createGem("Ruby", "Pigeon Blood Ruby", 3000.0, 0.80));
        RefreshUI(hwnd);
        return 0;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case IDC_ADD: {
            g_dialogData = GemDialogData(); // сброс перед диалогом
            INT_PTR res = DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDD_GEM_DIALOG), hwnd, GemDialogProc, 0);
            (void)res; // результат можно игнорировать, используем g_dialogData.accepted
            if (g_dialogData.accepted) {
                try {
                    auto gem = GemFactory::createGem(ws2s(g_dialogData.type),
                        ws2s(g_dialogData.name),
                        g_dialogData.price,
                        g_dialogData.weight);
                    g_necklace.addGem(gem);
                    RefreshUI(hwnd);
                }
                catch (const std::exception& ex) {
                    std::wstring msg = s2ws(std::string("Ошибка создания камня: ") + ex.what());
                    MessageBoxW(hwnd, msg.c_str(), L"Ошибка", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }

        case IDC_REMOVE: {
            HWND hList = GetDlgItem(hwnd, IDC_LISTBOX);
            int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR) {
                g_necklace.removeAt((size_t)sel);
                RefreshUI(hwnd);
            }
            else {
                MessageBoxW(hwnd, L"Выберите элемент для удаления.", L"Информация", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }

        case IDC_EDIT: {
            HWND hList = GetDlgItem(hwnd, IDC_LISTBOX);
            int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
            if (sel == LB_ERR) {
                MessageBoxW(hwnd, L"Выберите элемент для редактирования.", L"Информация", MB_OK | MB_ICONINFORMATION);
                break;
            }

            auto existing = g_necklace.getAt((size_t)sel);
            g_dialogData.type = s2ws(existing->typeName());
            g_dialogData.name = s2ws(existing->getName());
            g_dialogData.price = existing->getPrice();
            g_dialogData.weight = existing->getWeight();

            INT_PTR res = DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDD_GEM_DIALOG), hwnd, GemDialogProc, 0);
            (void)res;
            if (g_dialogData.accepted) {
                try {
                    auto newgem = GemFactory::createGem(ws2s(g_dialogData.type),
                        ws2s(g_dialogData.name),
                        g_dialogData.price,
                        g_dialogData.weight);
                    g_necklace.updateAt((size_t)sel, newgem);
                    RefreshUI(hwnd);
                }
                catch (const std::exception& ex) {
                    std::wstring msg = s2ws(std::string("Ошибка создания камня: ") + ex.what());
                    MessageBoxW(hwnd, msg.c_str(), L"Ошибка", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }

        } // switch
        break;
    } // WM_COMMAND

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ----------------- Запуск приложения -----------------

int RunApplication(HINSTANCE hInstance, int nCmdShow) {
    g_hInst = hInstance;

    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"GemNecklaceWnd";
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, wc.lpszClassName,
        L"Лабораторная: Камни — ожерелье (Win32 GUI)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 520, 340,
        nullptr, nullptr, g_hInst, nullptr);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}
