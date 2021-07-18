/*  Trim fat from windows*/
#define WIN32_LEAN_AND_MEAN 
#pragma comment(linker, "/subsystem:windows")

/*  Pre-processor directives*/
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

#include "Project.h"

#define FORMAT long long
#define CALC_ICON L"Project1.ico"
#define DEBUG(outputData) OutputDebugString(outputData)

// Menu items
#define ID_About 1000
#define ID_Exit 2000

class Calculator
{
    bool isInfinite = false;
    bool isTooLarge = false;
    bool operationComplete = false;
    std::wstring equationString = L"0";

    HINSTANCE hInst = nullptr;

    HWND calcHwnd = 0;

    // Define all the calculator buttons and display field
    HWND BTN_1 = 0, BTN_2 = 0, BTN_3 = 0, BTN_4 = 0, BTN_5 = 0, BTN_6 = 0, BTN_7 = 0, BTN_8 = 0, BTN_9 = 0, BTN_0 = 0,
        BTN_C = 0, BTN_CE = 0, BTN_ADD = 0, BTN_DIV = 0, BTN_MUL = 0, BTN_SUB = 0, BTN_EQ = 0, DISPLAY_FIELD = 0;
public:
    bool IsLastCharOperator(std::wstring opString)
    {
        bool outputResult = false;

        if (opString.size() > 0)
        {
            std::vector<std::wstring> operators = { L"+", L"\u2012", L"x", L"\u00F7" };

            for (auto i = 0; i < operators.size(); ++i)
            {
                std::wstring opFound = operators[i];
                size_t opLocation = opString.find(opFound);

                if (
                    opLocation != std::wstring::npos &&
                    opLocation == (opString.size() - 1)
                    )
                {
                    if (
                        opFound == L"+" ||
                        opFound == L"x" ||
                        opFound == L"\u2012" ||
                        opFound == L"\u00F7"
                        )
                    {
                        outputResult = true;
                    }
                }
            }
        }

        return outputResult;
    }

    HINSTANCE getHInstance()
    {
        return hInst;
    }

    void doInputNumber(LPCWSTR num)
    {
        if (num == L"C")
        {
            if (equationString != L"0")
            {
                equationString.pop_back();

                if (!equationString.size())
                {
                    isInfinite = false;
                    isTooLarge = false;
                    equationString = L"0";
                }
            }
        }
        else if (num == L"CE")
        {
            if (equationString.size() && equationString != L"0")
            {
                isInfinite = false;
                isTooLarge = false;
                equationString = L"";
                equationString += L"0";
            }
        }
        else {
            if (num != L"0" && equationString == L"0")
            {
                equationString = L"";
            }

            if (
                (num != L"0" && !operationComplete) ||
                (num == L"0" && equationString != L"0" && !IsLastCharOperator(equationString))
                )
            {
                if (equationString.size() < 12)
                {
                    equationString += num;
                }
                else {
                    MessageBox(calcHwnd, L"You cannot exceed 12 integers!", L"Error", MB_ICONERROR | MB_OK);
                }
            }
            else if (operationComplete)
            {
                isInfinite = false;
                equationString = num;
                operationComplete = false;
            }
        }

        SetWindowText(DISPLAY_FIELD, equationString.c_str());
    }

	// Get the number
    FORMAT getNumber(std::wstring str, LPCWSTR opr, int size = -1)
    {
        FORMAT number = 0;

        std::wstring extractFirstNumber = str.substr(0, str.find(opr));

        if (size != -1)
        {
            std::wstring extractSecondNumber = str.substr((str.find(opr) + 1), str.size());

            if (extractSecondNumber.size())
            {
                number = std::stoll(extractSecondNumber);
            }
            else {
                number = std::stoll(extractFirstNumber);
            }
        }
        else {
            number = std::stoll(extractFirstNumber);
        }

        return number;
    }

    void doInputOperator(LPCWSTR opr)
    {
        size_t npos = std::wstring::npos;

        if (opr == L"=")
        {
            if (
                equationString.find(L"+") != npos ||
                equationString.find(L"\u2012") != npos ||
                equationString.find(L"x") != npos ||
                equationString.find(L"\u00F7") != npos
                ) // arithmetic
            {
                FORMAT operation = 0;

                if (equationString.find(L"+") != npos)
                {
                    FORMAT firstNumber = getNumber(equationString, L"+");
                    FORMAT secondNumber = getNumber(equationString, L"+", static_cast<int>(equationString.size()));

                    operation = firstNumber + secondNumber;
                }
                else if (equationString.find(L"\u2012") != npos)
                {
                    FORMAT firstNumber = getNumber(equationString, L"\u2012");
                    FORMAT secondNumber = getNumber(equationString, L"\u2012", static_cast<int>(equationString.size()));

                    operation = firstNumber - secondNumber;
                }
                else if (equationString.find(L"x") != npos)
                {
                    FORMAT firstNumber = getNumber(equationString, L"x");
                    FORMAT secondNumber = getNumber(equationString, L"x", static_cast<int>(equationString.size()));

                    operation = firstNumber * secondNumber;
                }
                else if (equationString.find(L"\u00F7") != npos)
                {
                    FORMAT firstNumber = getNumber(equationString, L"\u00F7");
                    FORMAT secondNumber = getNumber(equationString, L"\u00F7", static_cast<int>(equationString.size()));

                    if (firstNumber == 0 || secondNumber == 0)
                    {
                        operation = 0;
                        isInfinite = true;
                    }
                    else {
                        operation = firstNumber / secondNumber;
                    }
                }

                // Is result too big??
                if (operation != 0 && operation > trunc(1e+12)) // Larger than 1 trillion?
                {
                    std::wstring errorMessage = L"Number is too big - ";
                    std::wstring opToString = std::to_wstring(operation);

                    errorMessage.append(opToString);

                    // If result is too big...
                    std::wstring trimmedResult = L"";
                    for (auto j = 0; j < opToString.length(); ++j)
                    {
                        auto digit = opToString[j];

                        if (digit != '0')
                        {
                            trimmedResult.push_back(digit);
                        }
                        else {
                            trimmedResult.push_back('e');
                            trimmedResult.append(std::to_wstring(opToString.substr(j).length()));

                            break;
                        }
                    }

                    isTooLarge = true;
                    SetWindowText(DISPLAY_FIELD, trimmedResult.c_str());
                    MessageBox(calcHwnd, errorMessage.c_str(), L"Information", MB_ICONERROR | MB_OK); // 4
                }
                else {
                    std::wstring opStr = L"";
                    std::wstring opStrNoZero;

                    if (operation == 0 && isInfinite)
                    {
                        opStr = L"You cannot divide by zero!";
                    }
                    else {
                        opStr = std::to_wstring(operation);

                        std::reverse(opStr.begin(), opStr.end());

                        bool zeroFlag = false;

                        for (unsigned int j = 0; j < opStr.size(); ++j)
                        {
                            if (!zeroFlag && opStr[j] != '0')
                            {
                                zeroFlag = true;
                                opStrNoZero += opStr[j];
                            }
                            else if (zeroFlag)
                            {
                                opStrNoZero += opStr[j];
                            }
                        }

                        std::reverse(opStrNoZero.begin(), opStrNoZero.end());
                        opStr = opStrNoZero;
                    }

                    equationString = opStr;
                    operationComplete = true;
                }
            }
        }
        else {
            if (equationString.find(opr) == npos)
            {
                equationString += opr;
                operationComplete = false;
            }
            else {
                doInputOperator(L"=");
                doInputOperator(opr);
            }
        }

        if (!isTooLarge)
        {
            SetWindowText(DISPLAY_FIELD, equationString.c_str());
        }
    }

	// Draw the menu toolbar
    void drawMenu(HWND hwnd)
    {
        HMENU hMenubar = CreateMenu();
        HMENU fileMenu = CreateMenu();
        HMENU helpMenu = CreateMenu();

        AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)fileMenu, L"&File");
        AppendMenuW(fileMenu, MF_STRING, ID_Exit, L"&Quit");

        AppendMenuW(helpMenu, MF_STRING, ID_About, L"&About");
        AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)helpMenu, L"&Help");

        SetMenu(hwnd, hMenubar);
    }

	// Draw interface - buttons, and display field (screen)
    void drawLayout(HINSTANCE hInstance, HWND hwnd)
    {
        calcHwnd = hwnd;
        hInst = hInstance;

        // Buttons and screen
        BTN_1 = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"1",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            10,         // x position 
            60,         // y position 
            50,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            //NULL,       // No menu.
            (HMENU)100,
            hInstance,
            NULL);      // Pointer not needed.

        BTN_2 = CreateWindow(L"BUTTON", L"2", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 70, 60, 50, 50, hwnd, (HMENU)200, hInstance, NULL);
        BTN_3 = CreateWindow(L"BUTTON", L"3", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 60, 50, 50, hwnd, (HMENU)300, hInstance, NULL);
        BTN_ADD = CreateWindow(L"BUTTON", L"+", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 190, 60, 50, 120, hwnd, (HMENU)10, hInstance, NULL);
        BTN_EQ = CreateWindow(L"BUTTON", L"=", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 310, 60, 50, 260, hwnd, (HMENU)90, hInstance, NULL);

        BTN_4 = CreateWindow(L"BUTTON", L"4", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 130, 50, 50, hwnd, (HMENU)400, hInstance, NULL);
        BTN_5 = CreateWindow(L"BUTTON", L"5", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 70, 130, 50, 50, hwnd, (HMENU)500, hInstance, NULL);
        BTN_6 = CreateWindow(L"BUTTON", L"6", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 130, 50, 50, hwnd, (HMENU)600, hInstance, NULL);
        BTN_SUB = CreateWindow(L"BUTTON", L"\u2012", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 190, 200, 50, 120, hwnd, (HMENU)30, hInstance, NULL);

        BTN_7 = CreateWindow(L"BUTTON", L"7", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 200, 50, 50, hwnd, (HMENU)700, hInstance, NULL);
        BTN_8 = CreateWindow(L"BUTTON", L"8", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 70, 200, 50, 50, hwnd, (HMENU)800, hInstance, NULL);
        BTN_9 = CreateWindow(L"BUTTON", L"9", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 200, 50, 50, hwnd, (HMENU)900, hInstance, NULL);
        BTN_MUL = CreateWindow(L"BUTTON", L"x", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 250, 60, 50, 120, hwnd, (HMENU)50, hInstance, NULL);

        BTN_C = CreateWindow(L"BUTTON", L"C", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 270, 50, 50, hwnd, (HMENU)925, hInstance, NULL);
        BTN_0 = CreateWindow(L"BUTTON", L"0", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 70, 270, 50, 50, hwnd, (HMENU)950, hInstance, NULL);
        BTN_CE = CreateWindow(L"BUTTON", L"CE", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 270, 50, 50, hwnd, (HMENU)975, hInstance, NULL);
        BTN_DIV = CreateWindow(L"BUTTON", L"\u00F7", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 250, 200, 50, 120, hwnd, (HMENU)70, hInstance, NULL);

        DISPLAY_FIELD = CreateWindow(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | WS_DISABLED | ES_LEFT, 10, 10, 350, 40, hwnd, (HMENU)1234, hInstance, 0);

        // Setting the font styles of elements
        HFONT hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Verdana");
        WPARAM setFont = WPARAM(hFont);

        std::vector<HWND> allHwndObjects = { BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9, BTN_0,
        BTN_C, BTN_CE, BTN_ADD, BTN_DIV, BTN_MUL, BTN_SUB, BTN_EQ, DISPLAY_FIELD };

        for (auto i = 0; i < allHwndObjects.size(); ++i)
        {
            SendMessage(allHwndObjects[i], WM_SETFONT, setFont, TRUE);
        }
    }
    Calculator() {}; // Constructor
    ~Calculator() {}; // Destructor
};

// Create a new instance of the calculator class
Calculator calc;

/*  Windows Procedure Event Handler*/
long long __stdcall WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paintStruct;
    /*  Device Context*/
    HDC hDC;
    /*  Text for display*/
    char greeting[] = "Hello, World!";

    /*  Switch message, condition that is met will execute*/
    switch (message)
    {
        /*  Window is being created*/
    case WM_CREATE:
        // Menu
        calc.drawMenu(hwnd);
        break;
        /*  Window is closing*/
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
        break;
    case WM_NOTIFY:
        break;
        /*  Window needs update*/
    case WM_PAINT:
        hDC = BeginPaint(hwnd, &paintStruct);
        /*  Set txt color to blue*/
        SetTextColor(hDC, COLORREF(0x00FF0000));
        /*  Display text in middle of window*/
        EndPaint(hwnd, &paintStruct);
        return 0;
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_About:
            MessageBox(
                hwnd,
                L"Voyager 89 Calculator\n---------------------\n\nVersion 1.1\n---\nReleased 13 July 2021\n\n---\nPlease report any issues: https://voyager89.net/contact-en",
                L"About...",
                MB_ICONINFORMATION | MB_OK
            );
            break;
        case ID_Exit:
            PostQuitMessage(0);

            break;

        case 10: calc.doInputOperator(L"+"); break;
        case 30: calc.doInputOperator(L"\u2012"); break;
        case 50: calc.doInputOperator(L"x"); break;
        case 70: calc.doInputOperator(L"\u00F7"); break;
        case 90: calc.doInputOperator(L"="); break;

        case 100: calc.doInputNumber(L"1"); break;
        case 200: calc.doInputNumber(L"2"); break;
        case 300: calc.doInputNumber(L"3"); break;
        case 400: calc.doInputNumber(L"4"); break;
        case 500: calc.doInputNumber(L"5"); break;
        case 600: calc.doInputNumber(L"6"); break;
        case 700: calc.doInputNumber(L"7"); break;
        case 800: calc.doInputNumber(L"8"); break;
        case 900: calc.doInputNumber(L"9"); break;
        case 925: calc.doInputNumber(L"C"); break;
        case 950: calc.doInputNumber(L"0"); break;
        case 975: calc.doInputNumber(L"CE"); break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        break;
    default:
        break;
    }
    return (DefWindowProc(hwnd, message, wParam, lParam));
}

/*  Main function*/
int __stdcall WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX  windowClass;        //window class
    HWND        hwnd;               //window handle
    MSG         msg;                //message

    /*  Fill out the window class structure*/
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_PROJECT1));
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = L"MyClass";
    windowClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    DEBUG(L"Test output\n");
    
    /*  Register window class*/
    if (!RegisterClassEx(&windowClass))
    {
        return 0;
    }

    /*  Class registerd, so now create window*/
    hwnd = CreateWindowEx(NULL,     //extended style
        L"MyClass",          //class name
        L"Calculator - Voyager 89",       //app name
        WS_OVERLAPPEDWINDOW |       //window style
        WS_VISIBLE |
        WS_SYSMENU,
        100, 100,            //x/y coords
        400, 400,            //width,height
        NULL,               //handle to parent
        NULL,               //handle to menu
        hInstance,          //application instance
        NULL);              //no extra parameter's

    /*  Check if window creation failed*/
    if (!hwnd)
    {
        return 0;
    }

    /*
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    */

    calc.drawLayout(hInstance, hwnd);

    /*  main message loop*/
    while (GetMessage(&msg, nullptr, NULL, NULL))
    {
        /*  Translate and dispatch to event queue*/
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}