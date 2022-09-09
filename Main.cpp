//License: GPL-2.0
//Macht was ihr wollt!!
//Es handelt sich hierbei um ein Gegenmittel gegen die ILLEGALE Software Proctorio!!
//Für ihre in die Privatsspähre intrusive Vorgehensweise hat sie 2021 bereits den Big Brother Award bekommen
//https://bigbrotherawards.de/en/2021/education-proctorio
//Sowohl Zoom, als auch Proctorio wurden vom hessischen Datenschutzbeauftragten verboten
//Wer meint, das ignorieren zu können, muss mit Gegenwind rechnen
//Proctorio verstößt gegen jegliche Datenschutzprinzipien und sollte SOFORT verboten werden


//Das Programm funktioniert erst ab Windows 10 Version 2004. Davor KANN es NICHT klappen
//und das unsichtbare Fenster ist für Bildschirmübertragungen trotzdem sichtbar
//Teste dieses Featur daher davor unbedingt aus (z.B. Zoom, Discord, OBS, oÄ)
//Im Idealfall schließt sich das Fenster sofort silently falls es diesen Fehler oder ähnlichen gibt
//Den Fehlercode kannst du nur über den Rückgabewert der Kommandozeile rausfinden

//Funktionsweise
// - Dieses Programm erzeugt ein für Bildschirmübertragungen unsichtbare Fenster  (Probiert es aus!!!
// - In das Fenster können Bilder mit beliebigem Inhalt angezeigt werden (z.B. Notizen)
// - unterstützt werden .bmp, .png, .jpeg, .gif(keine Animationen), .tif Bilder
// - Auf die nächste Folie(Slide) kommt man in der default Config mit Alt + ArrowLeft bzw ArrowRight
// - Shortcuts kann man in der default Config mit F4 de-/aktivieren
// - Mit ESC wird das Programm beendet
// - (linkes)ALt + Tab wechselt die Sichtbarkeit des Fensters
// - Verschoben und vergrößert wird das Fenster nur durch relative Mausbewegungen
// - toggle Window Grab : (linkes)Strg + (linkes)Alt + Backspace
// - toggle Window Size : (linkes)Strg + (linkes)Alt + Space
// - Außer im Konfigurationsmodus ist das Fenster auch für den Curosr transparent
// - Beachte, dass die geöffnete Dropdownlisten nicht für Bildschirmaufnahemen transparent sind
// - Mit der Dropdownliste kannst du ein Programm auswählen und dich als dieses im Taskmanager tarnen
// - Somit ist das Programm selbst mit geöffnetem Taskmanager fast nicht auffindbar
// - Wähle dafür ein Programm aus, drücke "Deploy" und dann "Launch". Es startet sich nun neu unter falscher Identität
// - Die Transparenz kannst du unter der Eigenschaft INITIAL_OPACITY zwischen 0 und 255 auswählen.
// - bestimmte Shortcuts werden von dem Programm rausgefiltert und nicht an darunterliegenede Programme durchgelassen, 
//	 obwohl diese den Keyboardfokus haben. Somit können eventuelle Keylogger nicht mitbekommen, welche Shortcuts du gerade bedienst
// - Eine genaue Liste der Shortcuts findest du in der Variable "applicationShortcuts". Alle Keystrokes nach dem Element "0" werden nicht 
// - an andere Programme durchgelassen
// - Teste auch dies vorher aus mit z.B. https://unixpapa.com/js/testkey.html. Wenn es hier klappt, wird es auch mit allen anderen Programmen klappen
// - Das Programm meldet sich, wenn nicht mehr alle nötigen Keystrokes unterdrücken kann "keyboard Hook failed. Key Supression not guaranted"
//   In diesem Fall solltest du keinen Sussy Keystrokes nutzen, da darunterliegende Keylogger diese mitbekommen könnte. Nutze in diesem Fall nur noch Esc,
//   um das Programm zu schließen
// - Pass auf dein Antivirusprogramm auf. Da es sich um unsichtbares Fenster mit der Möglichkeit, Keystrokes abzufangen ist, meckern viele AV's
// - Schließe es lieber vom Scan aus, auch wenn man dies normalerweise nie machen sollte. Somit bekommst du nicht in einer unangenehmen Situation
// - Probleme mit dem AV
// - Nach KEYBOARD_IDLE_TERMINATION (default 90) Sekunden keinem festgestelltem Keyboardinput schließt sich das Programm automatisch.
// - Falls du merkst, dass das Programm sich aufgehangen hat, drücke weiter unbedingt Tastendrücke und warte die 90s ab. Wenn du keine Tasten
//   drückst, wird sich das Programm auch nicht schließen.
// - Beachte, dass die Shortcuts im Configscreen nicht klappen, wenn das Fenster den Fokus hat. Damit du die Shortcuts bedienen kannst, musst du einmal
//   in den Hintergrund (nicht das Fenster) klicken - dann müsste es klappen
// - Das Fenster für den Pfad der zu importierenden Bilder akzeptiert sowohl relative als auch absolute Pfade. Des weiteren werden Environmentvars akzeptiert
//   Diese sind idR: %USERPROFILE%, %APPDATA%, %CommonProgramFiles%, %HOMEPATH%, %LOCALAPPDATA%, %ALLUSERSPROFILE%, %TEMP%
//   Weitere: https://www.computerhope.com/issues/ch000088.htm


//verschlüsselte Nachricht vom Autor: qkVHvsUhfVbZD3vs0d3Ej7WXKHFOpaxgF+z831RXcT2kaI0t6vbaB1W9kLc69s3x/Kqt3hvvnFHxKtflSajdtQ==

#pragma region Includes

#define NDEBUG
#define WIN32

#include <Windows.h>
#include <windowsx.h>
#include <hidusage.h>
#include <Shlwapi.h>
#include <Gdiplus.h>
#include <gdiplusheaders.h>
#include <commctrl.h>

#include <winternl.h>

//C++ CRT
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <psapi.h>
#include <tchar.h>
#include <assert.h>
#include <memory>
#include <atomic>



#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Gdiplus.lib")

#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Comctl32.lib")

#pragma endregion



#pragma region Config

#define ON 1
#define OFF 0


#define APP_IDENTIFIER "Der Hacker ist ein Macher"
#define INITIAL_SEARCH_PATH L"%USERPROFILE%\\pictures\\"

// AV can remove all keystroke notifications 
// which will make the window completely unresponsive
// Use this timer to show a warning after 
// KEYBOARD_IDLE_WARNING seconds of no detected keyboard input.
// Application will terminate itself after KEYBOARD_IDLE_TERMINATION
// of no detected keyboard input
#define KEYBOARD_IDLE_WARNING 60
#define KEYBOARD_IDLE_TERMINATION 90


#define EMPTY_CMDARGS OFF
#define SPOOF_CMDARGS ON

#define SAFE_DEL_SUSSY_ARGS ON
#define CMD_ARGS_IMGNAME_USE_PARENTHESIS ON

//Self deletion can cause AV flagging
#define SELFDELETE_AFTER_CLONE ON
#define SELFDELETE_USE_RUNTIME_LINKING ON

// 0(transparent) - 255(opaque)
#define INITIAL_OPACITY 140

//use a random fake window name of fakeWindowNames
#define RANDOM_FAKE_WINDOW_NAMES ON
//preferred index of fakeWindowNames if random fake names are off
#define FAKE_WINDOW_NAME_INDEX 2

#define FORCE_QUIT -INT_MAX

const float grabScaleFactor = 1.1f;
const float resizeScaleFactor = 1.1f;


#define FILE_DEL_ARG_NAME L"-fileDelete"

const std::pair<const wchar_t*, size_t> sussyArgs[] = {
	std::make_pair(FILE_DEL_ARG_NAME, 1),
};

enum class HotkeyActionType {
	TOGGLE_HIDE,
	QUIT_WINDOW,
	TOGGLE_GRAB,
	TOGGLE_RESIZE,
	DISABLE_SHORTCUTS,
	TOGGLE_FOCUS, //not yet implemented
	SWITCH_MONITOR, //not yet implemented
	NEXT_SLIDE,
	PREV_SLIDE
};

//Keystrokes after the 0 will get supressed for all underlying application if all buttons before are pressed
const std::pair<HotkeyActionType, std::vector<DWORD>> applicationShortcuts[] = {
	//Esc -> Quit Application
	std::make_pair(HotkeyActionType::QUIT_WINDOW,	std::vector<DWORD>({ VK_ESCAPE })),
	//F4 -> Disables ALL Application shortcuts and their supression
	std::make_pair(HotkeyActionType::DISABLE_SHORTCUTS,	std::vector<DWORD>({ VK_F4 })),
	//Left Control + Tab -> toggle window visible. Tab keystroke gets supressed while left control is pressed
	std::make_pair(HotkeyActionType::TOGGLE_HIDE,	std::vector<DWORD>({ VK_LCONTROL, 0, VK_TAB })),
	//Left Control + Left Arrow > Previous Slide. Left Arrow supressed
	std::make_pair(HotkeyActionType::PREV_SLIDE,	std::vector<DWORD>({ VK_LMENU, 0,  VK_LEFT })),
	//Left Control + Right Arrow > Next Slide. Right Arrow supressed
	std::make_pair(HotkeyActionType::NEXT_SLIDE,	std::vector<DWORD>({ VK_LMENU, 0,  VK_RIGHT })),
	//Strg + Left Alt + Backspace -> toggle window grab. Backspace supressed
	std::make_pair(HotkeyActionType::TOGGLE_GRAB,	std::vector<DWORD>({ VK_LCONTROL, VK_LMENU, 0, VK_BACK })),
	//Strg + Left Alt + Space -> toggle window resize. Space supressed
	std::make_pair(HotkeyActionType::TOGGLE_RESIZE, std::vector<DWORD>({ VK_LCONTROL, VK_LMENU, 0, VK_SPACE })),
};

const std::pair<const TCHAR*, const TCHAR*> fakeWindowNames[] = {
	std::make_pair(TEXT("tooltips_class32"), TEXT("")),
	std::make_pair(TEXT("IME"), TEXT("Default IME")),
	std::make_pair(TEXT("MSCTFIME UI"), TEXT("MSCTFIME UI")),
	std::make_pair(TEXT("WorkerW"), TEXT("")),
	std::make_pair(TEXT("DummyDWMListenerWindow"), TEXT("")),
};

#pragma endregion




#pragma region ConfigGuards
template <typename T, const size_t S>
constexpr size_t getSize(T(&)[S]) {
	return S;
}

#ifndef FAKE_WINDOW_NAME_INDEX
#error "FAKE_WINDOW_NAME_INDEX must be defined and between 0 and fakeWindowNames.length - 1"
#endif

static_assert(INITIAL_OPACITY >= 0 && INITIAL_OPACITY <= 255, "INITIAL_OPACITY must be between 0 and 255");

static_assert(KEYBOARD_IDLE_WARNING > 0, "KEYBOARD_IDLE_WARNING must be greater than 0");
static_assert(KEYBOARD_IDLE_TERMINATION > 0 && KEYBOARD_IDLE_TERMINATION >= KEYBOARD_IDLE_WARNING, "KEYBOARD_IDLE_WARNING must be greater than 0 and >= KEYBOARD_IDLE_WARNING");

static_assert(FAKE_WINDOW_NAME_INDEX >= 0 && (int)(sizeof(fakeWindowNames) / sizeof(fakeWindowNames[0]) > FAKE_WINDOW_NAME_INDEX) > 0,
	"FAKE_WINDOW_NAME_INDEX must be between 0 and fakeWindowNames.length - 1");

#define SETTING_ON(s) (s == 1)

#ifdef NDEBUG
#define DEBUG_ERROR(m) (void)0;
#else
#define DEBUG_ERROR(m) MessageBoxA(NULL, m, "Debug Error", MB_ICONERROR | MB_OK)
#endif // !NDEBUG

#pragma endregion


bool grabEnabled = false;
bool resizeEnabled = false;
HWND windowHandle;


//POINT windowMoveStartPos;
//POINT cursorGrabStartPos;
//POINT cursorResizeStartPos;

RECT windowActionStartPos;
POINT cursorActionStartPos;

RECT initialWindowPosition;



UINT forceQuitMessageId;



HHOOK keyboardHook;

HANDLE directoryChangeEvent = 0;

RECT desktopRect;





struct HotkeyCombination {
	struct HotkeyItem {
		DWORD keyCode = 0;
		union {
			struct {
				unsigned int supressKey		: 1;
				unsigned int keyActive		: 1;
				unsigned int keyLingering	: 1;
				unsigned int stickyKey		: 1;
			};
			void* data = nullptr;
		};

		HotkeyItem* nextItem = nullptr;
	} *hotkeys = nullptr;
	HotkeyActionType actionType{};
	HotkeyCombination* nextHotkey = nullptr;
} *hotkeyList = nullptr;

struct ProcessIdentifier {
	std::basic_string<TCHAR> fullImageName;
	DWORD processId = 0;
	HICON icon = NULL;
};


DWORD mainThreadId = 0;
DWORD hookThreadId = 0;

int handleQuit(DWORD otherThread, DWORD exitCode) {
	PostQuitMessage(exitCode);
	otherThread&& PostThreadMessage(otherThread, WM_QUIT, exitCode, 0);
	if (keyboardHook && UnhookWindowsHookEx(keyboardHook))
		keyboardHook = nullptr;
	if (exitCode == FORCE_QUIT)
		ExitProcess(FORCE_QUIT);
	return exitCode;
}

Gdiplus::Bitmap* currentImage = nullptr;

std::vector<std::wstring> imagePathList;
int32_t currentSlideIndex = -1;

RECT toCroppedWindowRect(RECT initialWindow) {

	if (currentImage == nullptr)
		return initialWindow;

	RECT croppedRect{};


	UINT width = currentImage->GetWidth();
	UINT height = currentImage->GetHeight();

	float scaleFactorX = (float)(initialWindowPosition.right - initialWindowPosition.left) / (float)width;
	float scaleFactorY = (float)(initialWindowPosition.bottom - initialWindowPosition.top) / (float)height;


	croppedRect = initialWindow;
	if (scaleFactorY < scaleFactorX) {

		float fNewWidth = scaleFactorY * width;
		int newWidth = (int)roundf(fNewWidth);
		int currentWidth = initialWindowPosition.right - initialWindowPosition.left;
		croppedRect.right -= (currentWidth - newWidth) / 2;
		croppedRect.left += (currentWidth - newWidth) / 2;
	}
	else {
		float fNewHeight = scaleFactorX * height;
		int newHeight = (int)roundf(fNewHeight);
		int currentHeight = initialWindowPosition.bottom - initialWindowPosition.top;
		croppedRect.bottom -= (currentHeight - newHeight) / 2;
		croppedRect.top += (currentHeight - newHeight) / 2;
	}


	return croppedRect;
}

void adjustWindowSize() {

	RECT croppedWindowRect = (resizeEnabled || grabEnabled || currentSlideIndex == -1) ? initialWindowPosition : toCroppedWindowRect(initialWindowPosition);


	//std::string output = "X: " + std::to_string(croppedWindowRect.left) + " Y: " + std::to_string(croppedWindowRect.top) +
	//	" CX: " + std::to_string((croppedWindowRect.right - 0)) + " CY: " + std::to_string((croppedWindowRect.bottom - 0)) + "\n";
	//OutputDebugStringA(output.c_str());

	MoveWindow(windowHandle,
		croppedWindowRect.left, croppedWindowRect.top,
		croppedWindowRect.right - croppedWindowRect.left,
		croppedWindowRect.bottom - croppedWindowRect.top,
		TRUE);

	/*SetWindowPos(windowHandle, HWND_TOPMOST,
		croppedWindowRect.left, croppedWindowRect.top,
		(croppedWindowRect.right - croppedWindowRect.left),
		(croppedWindowRect.bottom - croppedWindowRect.top),
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);*/

}


void toggleHide() {
	ShowWindow(windowHandle, IsWindowVisible(windowHandle) ? SW_HIDE : SW_RESTORE);
}
void quitWindow() {
	handleQuit(hookThreadId, EXIT_SUCCESS);
	handleQuit(mainThreadId, EXIT_SUCCESS);
}
void toggleGrab() {

	if (IsWindowVisible(windowHandle)) {

		GetCursorPos(&cursorActionStartPos);
		windowActionStartPos = initialWindowPosition;

		HWND desktopWindow = GetDesktopWindow();
		GetWindowRect(desktopWindow, &desktopRect);

		grabEnabled = !grabEnabled;
		resizeEnabled = false;

		adjustWindowSize();
		RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE);
	}
}
void toggleResize() {

	if (IsWindowVisible(windowHandle)) {

		GetCursorPos(&cursorActionStartPos);
		windowActionStartPos = initialWindowPosition;
		//GetWindowRect(windowHandle, &windowActionStartPos);
		//RECT startPos;
		//GetWindowRect(windowHandle, &startPos);
		//windowMoveStartPos = { startPos.left, startPos.top };

		HWND desktopWindow = GetDesktopWindow();
		GetWindowRect(desktopWindow, &desktopRect);

		resizeEnabled = !resizeEnabled;
		grabEnabled = false;

		adjustWindowSize();
		RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE);
	}
}

const wchar_t* currentWarningMessage = nullptr;


std::atomic<ULONG64> lastHookUpdateTimestamp = GetTickCount64();
std::atomic<ULONG64> lastRawInputUpdateTimestamp = GetTickCount64();

enum class Direction { Left, Right };

int loadNextSlideIndex(Direction direction, int currentIndex) {
	if (!imagePathList.size()) return -1;

	if (direction == Direction::Left) {
		int imageIndex = currentIndex - 1;
		if (imageIndex == -1) return -1;
		if (imageIndex < -1) {
			return (int)imagePathList.size() - 1;
		}
		return imageIndex % (int)imagePathList.size();

	}
	else {
		return (currentIndex + 1) % (int)imagePathList.size();
	}
	//return ((direction == Direction::Left ? max(-1, currentIndex - 1) : currentIndex + 1) + (int)imagePathList.size()) % (int)imagePathList.size();
}



bool loadNextImage(Direction direction) {
	if (imagePathList.size() <= 0) return false;

	int imageIndex = currentSlideIndex;


	bool imageFound = false;
	while (!imageFound) {
		imageIndex = loadNextSlideIndex(direction, imageIndex);

		if (imageIndex == currentSlideIndex || imageIndex == -1) return false;

		Gdiplus::Bitmap* oldImage = currentImage;
		currentImage = nullptr;

		//wchar_t wideFilePath[MAX_PATH] = { };
		//int conversionResult = MultiByteToWideChar(CP_UTF8, 0, imagePathList.at(imageIndex).c_str(), -1, wideFilePath, MAX_PATH - 1);
		currentImage = currentImage->FromFile(imagePathList.at(imageIndex).c_str(), false);
		if (currentImage != nullptr && currentImage->GetLastStatus() == Gdiplus::Ok) {
			delete oldImage;
			oldImage = nullptr;
			currentSlideIndex = imageIndex;
			imageFound = true;
		}
		else {
			delete currentImage;
			currentImage = oldImage;
		}
	}
	return true;
}

HWND comboBoxControl = NULL;
HWND deployButton = NULL;
HWND pathCheckButton = NULL;
HWND imageDirectoryEdit = NULL;

std::wstring expandedSearchPathBuffer;
std::wstring userSearchPathBuffer;

void loadConfigSlide() {

	HINSTANCE currentInstance = GetModuleHandle(NULL);

	comboBoxControl = CreateWindow(WC_COMBOBOXEX, TEXT(""),
		WS_BORDER | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL,
		20, 20, 200, 140, windowHandle, 0, currentInstance, 0);

	deployButton = CreateWindow(WC_BUTTON, TEXT("Deploy"),
		WS_CHILD | WS_VISIBLE | WS_DISABLED,
		235, 20, 55, 25, windowHandle, 0, currentInstance, 0);

	imageDirectoryEdit = CreateWindowW(WC_EDITW, userSearchPathBuffer.c_str(),
		WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT,
		20, 55, 200, 22, windowHandle, 0, currentInstance, 0);

	pathCheckButton = CreateWindow(WC_BUTTON, TEXT("Check"),
		WS_CHILD | WS_VISIBLE,
		235, 55, 55, 25, windowHandle, 0, currentInstance, 0);

	HFONT editFont = CreateFont(18, 0, 0, 0, FW_MEDIUM, false, false, false, ANSI_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI"));
	SendMessage(imageDirectoryEdit, WM_SETFONT, WPARAM(editFont), TRUE);


	SetWindowLongPtr(windowHandle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

	BLENDFUNCTION blendFunction{};
	blendFunction.BlendOp = AC_SRC_OVER;
	blendFunction.BlendFlags = 0;
	blendFunction.SourceConstantAlpha = INITIAL_OPACITY;
	blendFunction.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(windowHandle, NULL, nullptr, nullptr, NULL, nullptr, RGB(50, 50, 50), &blendFunction, ULW_ALPHA);
}
void unloadConfigSlide() {

	comboBoxControl		&&	DestroyWindow(comboBoxControl);
	deployButton		&&	DestroyWindow(deployButton);
	imageDirectoryEdit	&&	DestroyWindow(imageDirectoryEdit);
	pathCheckButton		&&	DestroyWindow(pathCheckButton);

	SetWindowLongPtr(windowHandle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);

	BLENDFUNCTION blendFunction{};
	blendFunction.BlendOp = AC_SRC_OVER;
	blendFunction.BlendFlags = 0;
	blendFunction.SourceConstantAlpha = INITIAL_OPACITY;
	blendFunction.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(windowHandle, NULL, nullptr, nullptr, NULL, nullptr, RGB(50, 50, 50), &blendFunction, ULW_ALPHA);
}


void clearWarningText(const wchar_t* warningText) {

	if (warningText && currentWarningMessage && !wcscmp(warningText, currentWarningMessage)) {
		currentWarningMessage = nullptr;
		windowHandle&& RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE);
	}
}
void setWarningText(const wchar_t* warningText, int duration) {

	bool timerExisting = KillTimer(windowHandle, 99);

	if ((warningText && !timerExisting) ||
		(warningText != currentWarningMessage &&
		(!warningText || !currentWarningMessage || 
			wcscmp(warningText, currentWarningMessage)))) {
		currentWarningMessage = warningText;
		windowHandle && RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE);

	}

	if (duration > 0)
		SetTimer(windowHandle, 99, duration, NULL);
}

UINT_PTR terminationTimerHandle = NULL;
void setSupressionWarning(bool supressionState) {

	static bool previousSupressionState = false;
	if (supressionState != previousSupressionState) {

		HWND foregroundWindow = GetForegroundWindow();


		if (supressionState)
			setWarningText(L"keyboard Hook failed. Key Supression not guaranted", -1);
		else
			clearWarningText(L"keyboard Hook failed. Key Supression not guaranted");


		//if (supressionState)
			//DEBUG_ERROR("Keyboard Hook failed");

		previousSupressionState = supressionState;
	}
}


void loadNextSlide(Direction direction) {
	int nextSlideIndex = loadNextSlideIndex(direction, currentSlideIndex);
	if (nextSlideIndex != currentSlideIndex) {
		if (currentSlideIndex == -1) {
			unloadConfigSlide();
		}


		if (nextSlideIndex == -1 || !loadNextImage(direction)) {
			if (nextSlideIndex > -1)
				setWarningText(L"no image found in folder", 1500);

			currentSlideIndex = -1;
			loadConfigSlide();
		}
		adjustWindowSize();
		RedrawWindow(windowHandle, nullptr, NULL, RDW_INVALIDATE);
	}
	else {
		setWarningText(L"no more image files found", 1000);
	}
}


bool shortcutsDisabled = false;
void triggerAction(HotkeyActionType action, bool keyDown) {
	if (action == HotkeyActionType::DISABLE_SHORTCUTS) {
		shortcutsDisabled ^= !keyDown;
	}

	if (shortcutsDisabled)
		return;


	int i = 0;
	switch (action)
	{
	case HotkeyActionType::TOGGLE_HIDE:
		toggleHide();
		break;
	case HotkeyActionType::QUIT_WINDOW:
		quitWindow();
		break;
	case HotkeyActionType::TOGGLE_GRAB:
		toggleGrab();
		break;
	case HotkeyActionType::TOGGLE_RESIZE:
		toggleResize();
		break;
	case HotkeyActionType::TOGGLE_FOCUS:
		break;
	case HotkeyActionType::SWITCH_MONITOR:
		break;
	case HotkeyActionType::NEXT_SLIDE:
		loadNextSlide(Direction::Right);
		break;
	case HotkeyActionType::PREV_SLIDE:
		loadNextSlide(Direction::Left);
		break;
	default:
		break;
	}
}

void handleHotkeyComplete(HotkeyCombination* hotkey, bool keyDown) {

	HotkeyCombination::HotkeyItem* lastKeyItem = hotkey->hotkeys;
	while (lastKeyItem) {
		lastKeyItem->keyLingering = true;
		lastKeyItem = lastKeyItem->nextItem;
	}



	switch (hotkey->actionType)
	{
	case HotkeyActionType::QUIT_WINDOW:
		PostQuitMessage(1);
		break;
	case HotkeyActionType::DISABLE_SHORTCUTS:
		if (!keyDown) return;
		break;
	case HotkeyActionType::TOGGLE_HIDE:
		if (!keyDown) return;
		//toggleHide();
		break;

	case HotkeyActionType::TOGGLE_GRAB:
		if (keyDown) return;
		//toggleGrab();
		break;
	case HotkeyActionType::TOGGLE_RESIZE:
		if (keyDown) return;
		//toggleResize();
		break;
	case HotkeyActionType::TOGGLE_FOCUS:
		break;
	case HotkeyActionType::SWITCH_MONITOR:
		break;
	case HotkeyActionType::NEXT_SLIDE:
		if (keyDown) return;
	case HotkeyActionType::PREV_SLIDE:
		if (keyDown) return;
		break;
	default:
		break;
	}

	PostMessageA(windowHandle, WM_USER, 100, (LPARAM)hotkey->actionType);
}
bool handleKeyStroke(bool keyDownEvent, DWORD vKey, bool isHookThread) {

	static std::atomic<unsigned int> brokenHookCertainty = 0;
	ULONG64 currentTickCount = GetTickCount64();
	(isHookThread ? lastHookUpdateTimestamp : lastRawInputUpdateTimestamp).store(currentTickCount);
	if (isHookThread) {
		//OutputDebugString("WM_HOOK2\n");
		if (terminationTimerHandle) {
			if (KillTimer(windowHandle, terminationTimerHandle))
				terminationTimerHandle = NULL;
		}
		setSupressionWarning(false);
		clearWarningText(L"Keyboard Idle detected - Press a key");
		//setWarningText(nullptr, -1);
		brokenHookCertainty.store(0);
	}
	else if (!isHookThread && currentTickCount - lastHookUpdateTimestamp.load() > 2000) {

		//LL_KEYBOARD_PROC didnt receive a message in the last second.
		//AV probably removed the Hook
		//Warn user about missing supressability
		if (GetForegroundWindow() != windowHandle) {
			brokenHookCertainty += 1;
			if (brokenHookCertainty >= 5) {
				//display message to user
				setSupressionWarning(true);

				//DEBUG_ERROR("Keyboard Hook is probably broken");
			}		
			//OutputDebugString("broken hook?\n");
		}
		else {
			brokenHookCertainty.store(0);
			lastHookUpdateTimestamp.store(currentTickCount);
			lastRawInputUpdateTimestamp.store(currentTickCount);
		}		
	}
	else {
		//OutputDebugString("discarding WM_KEYBOARD\n");
		return false;

	}


	HotkeyCombination* activeHotkey = hotkeyList;

	//std::string output = "Pressed key: " + std::to_string(vKey) + " wParam: " + std::to_string(keyDownEvent) + "\n";
	//OutputDebugStringA(output.c_str());

	while (activeHotkey) {
		HotkeyCombination::HotkeyItem* currentKeyCode = activeHotkey->hotkeys;
		while (currentKeyCode && (
			currentKeyCode->keyActive ||
			currentKeyCode->keyLingering ||
			vKey == currentKeyCode->keyCode)) {

			if (vKey == currentKeyCode->keyCode) {
				bool prevKeyState = currentKeyCode->keyActive;
				currentKeyCode->keyActive = keyDownEvent;

				if (!currentKeyCode->nextItem && prevKeyState != currentKeyCode->keyActive)
					handleHotkeyComplete(activeHotkey, keyDownEvent);

				if (!keyDownEvent) {
					if (!currentKeyCode->nextItem ||
						(currentKeyCode->nextItem && !currentKeyCode->nextItem->keyLingering)) {

						currentKeyCode->keyLingering = false;
					}
				}

				if (!shortcutsDisabled && currentKeyCode->supressKey)
					return true;
			}
			else if (
				currentKeyCode->keyLingering && !currentKeyCode->keyActive &&
				currentKeyCode->nextItem && !currentKeyCode->nextItem->keyLingering) {

				currentKeyCode->keyLingering = false;
			}

			currentKeyCode = currentKeyCode->nextItem;
		}
		activeHotkey = activeHotkey->nextHotkey;
	}
	return false;
}
void handleMouseInput(RAWMOUSE& mouseData) {
	if (!grabEnabled && !resizeEnabled)
		return;

	if ((mouseData.usFlags & 0x01) == MOUSE_MOVE_RELATIVE || (mouseData.usFlags & 0x01) == MOUSE_MOVE_ABSOLUTE) {
		long deltaX = mouseData.lLastX;
		long deltaY = mouseData.lLastY;

		//GetWindowPlacement

		POINT cursorPos;
		GetCursorPos(&cursorPos);

		HMONITOR activeMonitor = MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONULL);

		MONITORINFO monitorInfo = { };
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfoA(activeMonitor, &monitorInfo);


		if (grabEnabled) {

			POINTFLOAT cursorOffset = {
				grabScaleFactor * (cursorPos.x - cursorActionStartPos.x),
				grabScaleFactor * (cursorPos.y - cursorActionStartPos.y)
			};

			int windowWidth = windowActionStartPos.right - windowActionStartPos.left;
			int windowHeight = windowActionStartPos.bottom - windowActionStartPos.top;

			int newPosX, newPosY;
			newPosX = windowActionStartPos.left + (int)cursorOffset.x;
			newPosY = windowActionStartPos.top + (int)cursorOffset.y;


			if (newPosX < desktopRect.left - windowWidth) {
				newPosX = desktopRect.left - windowWidth - 1;
				//cursorActionStartPos.x = -(((desktopRect.left - windowWidth) - windowActionStartPos.right - windowActionStartPos.left) / grabScaleFactor) + cursorPos.x;
			}
			if (newPosY < desktopRect.top - windowHeight) {
				newPosY = desktopRect.top - windowHeight - 1;
				//cursorActionStartPos.y = -(((desktopRect.top - windowHeight) - windowActionStartPos.top - windowActionStartPos.bottom) / grabScaleFactor) + cursorPos.y;
			}

			if (newPosX - windowWidth > desktopRect.right) {
				newPosX = desktopRect.right + 1;
				//cursorActionStartPos.x = -((desktopRect.right - windowActionStartPos.right - windowActionStartPos.left) / grabScaleFactor) + cursorPos.x;
			}
			if (newPosY - windowHeight > desktopRect.bottom) {
				newPosY = desktopRect.bottom + 1;
				//cursorActionStartPos.y = -((desktopRect.top - windowActionStartPos.bottom - windowActionStartPos.top) / grabScaleFactor) + cursorPos.y;
			}

			initialWindowPosition = {
				newPosX,
				newPosY,
				newPosX + windowWidth,
				newPosY + windowHeight
			};

			adjustWindowSize();
		}


		if (resizeEnabled) {
			POINTFLOAT cursorOffset = {
				resizeScaleFactor * (cursorPos.x - cursorActionStartPos.x),
				resizeScaleFactor * (cursorPos.y - cursorActionStartPos.y)
			};

			int newWidth = int((windowActionStartPos.right - windowActionStartPos.left) + cursorOffset.x);
			int newHeight = int((windowActionStartPos.bottom - windowActionStartPos.top) + cursorOffset.y);

			initialWindowPosition = {
				windowActionStartPos.left + min(0, newWidth),
				windowActionStartPos.top + min(0, newHeight),
				windowActionStartPos.left + max(0, newWidth),
				windowActionStartPos.top + max(0, newHeight)
			};

			adjustWindowSize();
		}
	}
}

LRESULT __stdcall keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	//OutputDebugString("WM_HOOK\n");
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
	bool keyDownEvent = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
	bool keyUpEvent = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

	if (nCode == HC_ACTION && (keyDownEvent || keyUpEvent)) {
		if (handleKeyStroke(keyDownEvent, key->vkCode, true)) {
			return 1;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
void handleRawInput(HRAWINPUT rawInputHandle) {

	RAWINPUT inputData = {};
	UINT inputDataSize = sizeof(RAWINPUT);
	if (GetRawInputData(rawInputHandle, RID_INPUT, &inputData, &inputDataSize, sizeof(RAWINPUTHEADER)) != -1) {
		if (inputData.header.dwType == RIM_TYPEMOUSE) {
			handleMouseInput(inputData.data.mouse);
		}
		else if (inputData.header.dwType == RIM_TYPEKEYBOARD) {
			//OutputDebugStringA("WM_KEYBOARD received\n");

			RAWKEYBOARD& rawKeyboard = inputData.data.keyboard;
			bool keyDownEvent = (rawKeyboard.Flags & 1) == RI_KEY_MAKE;

			handleKeyStroke(keyDownEvent, rawKeyboard.VKey, false);

		}
	}
	////failure retriving data :_((
}


void registerHotkey(HotkeyActionType action, std::vector<DWORD> keyCodes) {

	HotkeyCombination* newHotkey = new HotkeyCombination();
	newHotkey->actionType = action;
	newHotkey->nextHotkey = nullptr;

	HotkeyCombination::HotkeyItem* lastItem = nullptr;
	bool startSupression = false;
	for (unsigned int i = 0; i < keyCodes.size(); i++) {
		HotkeyCombination::HotkeyItem* newItem = new HotkeyCombination::HotkeyItem();
		if (keyCodes.at(i) != 0) {
			newItem->keyCode = keyCodes.at(i);
			newItem->supressKey = startSupression;
			newItem->keyActive = false;
			newItem->keyLingering = false;
			newItem->nextItem = nullptr;
			if (lastItem == nullptr) {
				newHotkey->hotkeys = lastItem = newItem;
			}
			else {
				lastItem = lastItem->nextItem = newItem;
			}
		}
		else {
			startSupression = true;
		}

	}

	HotkeyCombination* lastHotkeyList = hotkeyList;
	while (lastHotkeyList && lastHotkeyList->nextHotkey) {
		lastHotkeyList = lastHotkeyList->nextHotkey;
	}
	(lastHotkeyList ? lastHotkeyList->nextHotkey : hotkeyList) = newHotkey;

	/*if (lastHotkeyList) {
		lastHotkeyList->nextHotkey = newHotkey;
	}
	else {
		hotkeyList = newHotkey;
	}*/
}
bool registerRawInputDevice(HWND windowHandle) {

	RAWINPUTDEVICE registerDeviceList[2] = { };
	registerDeviceList[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	registerDeviceList[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	registerDeviceList[0].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;//RIDEV_CAPTUREMOUSE / | 
	registerDeviceList[0].hwndTarget = windowHandle;

	
	registerDeviceList[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	registerDeviceList[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	registerDeviceList[1].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
	registerDeviceList[1].hwndTarget = windowHandle;


	if (!RegisterRawInputDevices(registerDeviceList, 2, sizeof(RAWINPUTDEVICE))) {
		DWORD error = GetLastError();
		return false;
	}
	return true;
}



bool isCompatibleImageExtension(const wchar_t* extension) {
	//No extension -> file rejected as an image
	if (!extension || !*extension) return false;

	const wchar_t* compatibleExtensions[] = {
		L".bmp", L".dib",
		L".png",
		L".jpg", L".jpeg", L".jpe", L".jif", L".jfif", L".jfi",
		L".gif",
		L".tiff", L".tif",
	};

	for (int i = 0; i < sizeof(compatibleExtensions) / sizeof(const char*); i++) {
		if (_wcsicmp(compatibleExtensions[i], extension) == 0) {
			return true;
		}
	}

	return false;
}




std::vector<ProcessIdentifier> processList;
std::basic_string<TCHAR> deployExecutablePath = std::basic_string<TCHAR>();


std::vector<ProcessIdentifier> listRunningProcesses() {
	std::vector<ProcessIdentifier> returnList;

	DWORD* processList = nullptr;
	uint32_t currentListSize = 1024 - 256;
	DWORD returnedListSize = 0;


	do {
		delete[] processList;
		currentListSize += 256;
		processList = new DWORD[currentListSize];

		if (!EnumProcesses(
			processList,
			currentListSize * sizeof(DWORD),
			&returnedListSize))
			return returnList;
	} while (currentListSize * sizeof(DWORD) == returnedListSize);


	DWORD processCount = returnedListSize / sizeof(DWORD);
	HINSTANCE instance = GetModuleHandle(NULL);
	if (instance == 0)
		return returnList;


	for (unsigned int i = 0; i < processCount; i++) {
		HANDLE processHandle = OpenProcess(
			PROCESS_QUERY_INFORMATION,
			false, processList[i]);
		if (processHandle != NULL) {
			TCHAR copyImageName[MAX_PATH + 1] = {};

			DWORD imagePathSize = MAX_PATH;
			bool result = QueryFullProcessImageName(processHandle, 0, copyImageName, &imagePathSize);

			assert(instance != 0);

			ProcessIdentifier process;
			process.processId = processList[i];
			process.fullImageName = std::basic_string<TCHAR>(copyImageName);
			process.icon = ExtractIcon(instance, copyImageName, 0);

			bool doubleFound = false;
			for (size_t j = 0; j < returnList.size(); j++) {
				if (!process.fullImageName.compare(returnList.at(j).fullImageName)) {
					doubleFound = true;
					break;
				}
			}

			if(!doubleFound)
				returnList.push_back(std::move(process));

		}
	}
	return returnList;


}

bool launchExecutable(std::basic_string<TCHAR>& launchPath) {

	if (launchPath.empty())
		return false;

	std::basic_string<TCHAR> fakeArgs = TEXT("");
#if not SETTING_ON(EMPTY_CMDARGS)

#if SETTING_ON(CMD_ARGS_IMGNAME_USE_PARENTHESIS)
	fakeArgs.append(TEXT("\""));
#endif

#if SETTING_ON(SPOOF_CMDARGS)
	fakeArgs.append(launchPath);
#else
	TCHAR* fileName = PathFindFileName(launchPath.data());
	DWORD fileNameLength = _tcslen(fileName);

	DWORD baseDirectorySize = GetCurrentDirectory(NULL, 0);
	TCHAR* baseDirectory = new TCHAR[max(MAX_PATH, baseDirectorySize + fileNameLength + 2) + 1]{};
	GetCurrentDirectory(baseDirectorySize, baseDirectory);

	PathAppend(baseDirectory, fileName);
	fakeArgs.append(baseDirectory);

	delete[] baseDirectory;
#endif

#if SETTING_ON(CMD_ARGS_IMGNAME_USE_PARENTHESIS)
	fakeArgs.append(TEXT("\""));
#endif

	//fakeArgs.append(TEXT(" "));
#endif

	STARTUPINFO startupInfo{
		sizeof(STARTUPINFO), NULL, NULL,
		NULL, 0, 0, 0, 0, 0, 0, 0,
		STARTF_FORCEOFFFEEDBACK | STARTF_PREVENTPINNING,
		0, 0, 0, 0, 0, 0
	};
	PROCESS_INFORMATION processInformation;

	if (CreateProcess(PathFindFileName(launchPath.data()),
		const_cast<TCHAR*>(fakeArgs.data()),
		NULL, NULL, FALSE,
		CREATE_NO_WINDOW | DETACHED_PROCESS,
		NULL, NULL, &startupInfo, &processInformation)) {

		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);

		handleQuit(hookThreadId, EXIT_SUCCESS);

		return true;
	}

	return false;
}
bool deployExecutable() {

	int selectedIndex = (int)SendMessage(comboBoxControl, CB_GETCURSEL, 0, 0);
	if (selectedIndex == CB_ERR)
		return false;


	ProcessIdentifier identifier = processList.at(selectedIndex);

	TCHAR* foreignImageFilename = PathFindFileName(identifier.fullImageName.c_str());
	size_t foreignImageFilenameLength = _tcslen(foreignImageFilename);



	DWORD baseDirectorySize = GetCurrentDirectory(NULL, 0);
	TCHAR* baseDirectory = new TCHAR[max(MAX_PATH, baseDirectorySize + foreignImageFilenameLength + 2) + 1]{};
	GetCurrentDirectory(baseDirectorySize, baseDirectory);

	PathAppend(baseDirectory, foreignImageFilename);

	if (PathCommonPrefix(baseDirectory, identifier.fullImageName.c_str(), nullptr) == _tcslen(baseDirectory)) {
		//avoid overwriting original file
		setWarningText(L"can't overwrite original file", 3000);
		return false;
	}

	TCHAR fileName[MAX_PATH + 1] = { };
	DWORD length = GetModuleFileName(NULL, fileName, MAX_PATH);


	bool copyResult = CopyFile(fileName, baseDirectory, false);
	if (!copyResult) {
		setWarningText(L"cannot overwrite file. Is it already running?", 3000);
		return false;
	}

	HMODULE moduleHandle = LoadLibraryEx(identifier.fullImageName.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (!moduleHandle) {
		setWarningText(L"failed to load application icons", 3000);
		DWORD error = GetLastError();
		return true;
	}


	std::vector<std::pair<void*, uint32_t>> imageIconList;
	std::vector<std::pair<void*, uint32_t>> imageIconGroupList;
	std::pair<decltype(imageIconList)*, decltype(imageIconList)*> resourceList =
		std::make_pair(&imageIconList, &imageIconGroupList);

	EnumResourceTypes(moduleHandle, [](HMODULE moduleHandle, TCHAR* lpszType, LONG_PTR lParam) -> BOOL {
		if (lpszType == RT_ICON || lpszType == RT_GROUP_ICON) {
			assert(lpszType);
			EnumResourceNames(moduleHandle, lpszType, [](HMODULE moduleHandle, const TCHAR* lpType, TCHAR* lpName, LONG_PTR lParam) -> BOOL {
				if (lpType == RT_ICON || lpType == RT_GROUP_ICON) {
					assert(lpType);

					HRSRC iconResource = FindResource(moduleHandle, lpName, lpType);
					if (!iconResource)
						return true;

					HGLOBAL hGlobal = LoadResource(moduleHandle, iconResource);
					if (!hGlobal)
						return true;

					DWORD resourceSize = SizeofResource(moduleHandle, iconResource);
					void* resourceData = LockResource(hGlobal);

					void* resourceCopy = new char[resourceSize];
					memcpy(resourceCopy, resourceData, resourceSize);

					(lpType == RT_ICON ?
						((std::pair<decltype(imageIconList)*, decltype(imageIconList)*>*)lParam)->first :
						((std::pair<decltype(imageIconList)*, decltype(imageIconList)*>*)lParam)->second)
						->push_back(std::make_pair(resourceCopy, resourceSize));
					/*(lpType == RT_ICON ?
						((decltype(resourceList)*)lParam)->first :
						((decltype(resourceList)*)lParam)->second)
						->push_back(std::make_pair(resourceCopy, resourceSize));*/
				}
				return true;
			}, lParam);
		}
		return true;
	}, (LONG_PTR)&resourceList);

	FreeLibrary(moduleHandle);
	moduleHandle = 0;

	HANDLE resourceUpdateHandle = BeginUpdateResource(baseDirectory, false);
	if (!resourceUpdateHandle)
		return true;

	int countIndex;
	TCHAR* resourceType;
	std::function<void(std::pair<void*, uint32_t>)> updateFunction = [&resourceUpdateHandle, &countIndex, &resourceType](std::pair<void*, uint32_t> item) {
		bool result = resourceUpdateHandle && UpdateResource(
			resourceUpdateHandle,
			resourceType, MAKEINTRESOURCE(countIndex++), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			item.first, item.second);

		delete[] item.first;
	};


	countIndex = 1;
	resourceType = RT_ICON;
	std::for_each(imageIconList.begin(), imageIconList.end(),
		updateFunction);
	imageIconList.clear();

	countIndex = 1;
	resourceType = RT_GROUP_ICON;
	std::for_each(imageIconGroupList.begin(), imageIconGroupList.end(),
		updateFunction);
	imageIconGroupList.clear();


	if (resourceUpdateHandle)
		EndUpdateResourceA(resourceUpdateHandle, false);

	return true;
}

void warnIdleUser() {
	LASTINPUTINFO lastInput{};
	lastInput.cbSize = sizeof(lastInput);
	if (!GetLastInputInfo(&lastInput)) {
		return;
	}

	if (lastInput.cbSize != sizeof(LASTINPUTINFO))
		return;

	ULONG64 maxInputTimestamp = lastHookUpdateTimestamp;
	if (lastInput.dwTime - (DWORD)maxInputTimestamp > (1000 * KEYBOARD_IDLE_WARNING)) {

		HWND foregroundWindow = GetForegroundWindow();

		setWarningText(L"Keyboard Idle detected - Press a key", -1);
		//Self Termination Timer
		if (!terminationTimerHandle) {
			terminationTimerHandle = SetTimer(windowHandle, 666, ((KEYBOARD_IDLE_TERMINATION - KEYBOARD_IDLE_WARNING) * 1000) / 2, NULL);
		}
	}
	else {
		clearWarningText(L"Keyboard Idle detected - Press a key");
		if (terminationTimerHandle) {
			if (KillTimer(windowHandle, terminationTimerHandle))
				terminationTimerHandle = NULL;
		}
	}
}
void openDropDownList() {


	//while (SendMessage(comboBoxControl, CBEM_DELETEITEM, 0, 0) > 0);
	SendMessage(comboBoxControl, CB_RESETCONTENT, 0, 0);

	processList.clear();
	processList = listRunningProcesses();


	HIMAGELIST imageList = ImageList_Create(16, 16, ILC_COLOR24, 1, 1);

	HICON defaultIcon = LoadIcon(NULL, IDI_APPLICATION);
	ImageList_AddIcon(imageList, defaultIcon);

	int currentIconCount = 0;

	

	HINSTANCE instance = GetModuleHandle(NULL);
	if (instance == 0)
		return;

	

	for (int i = 0; i < processList.size(); i++) {
		int currentIconIndex = 0;
		TCHAR* processName = PathFindFileName(processList.at(i).fullImageName.c_str());
		HICON extractedIcon = ExtractIcon(instance, processList.at(i).fullImageName.c_str(), 0);
		if (extractedIcon != 0) {
			ImageList_AddIcon(imageList, extractedIcon);
			currentIconIndex = ++currentIconCount;
		}

		COMBOBOXEXITEM newItem{};

		newItem.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		newItem.iItem = i;
		newItem.pszText = processName;
		newItem.cchTextMax = 0;
		newItem.iImage = currentIconIndex;
		newItem.iSelectedImage = currentIconIndex;
		newItem.iIndent = 0;
		if (SendMessage(comboBoxControl, CBEM_INSERTITEM, 0, (LPARAM)&newItem) == -1)
			return;
	}
	SetWindowPos(comboBoxControl, NULL, 20, 20, 200, 140, SWP_NOACTIVATE);
	SendMessage(comboBoxControl, CBEM_SETIMAGELIST, 0, (LPARAM)imageList);


}

DWORD CALLBACK hookThread(void* parameter) {
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookCallback, NULL, 0);
	if (!keyboardHook) {
		DWORD error = GetLastError();
		ExitProcess(EXIT_FAILURE);
		//handleQuit(mainThreadId, -101);
		return EXIT_FAILURE;
	}

	MSG msg;
	BOOL bRet = false;
	while (bRet = GetMessageA(&msg, NULL, 0, 0) <= 0) {
		if (msg.message == forceQuitMessageId) {
			if (handleQuit(mainThreadId, (DWORD)msg.wParam) == FORCE_QUIT)
				return FORCE_QUIT;
		}

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}


	keyboardHook&& UnhookWindowsHookEx(keyboardHook);
	return bRet | (DWORD)msg.wParam;
}
bool createHookThread() {
	HANDLE threadHandle = CreateThread(NULL, 0, hookThread, nullptr, 0, &hookThreadId);
	if (threadHandle == NULL) {
		DEBUG_ERROR("creating hooking thread failed");
		handleQuit(0, -100);
	}
	return threadHandle != NULL;
}


void updateDropdownSelection() {

	int newSelectionIndex = (int)SendMessage(comboBoxControl, CB_GETCURSEL, 0, 0);
	if (newSelectionIndex != CB_ERR) {
		if (newSelectionIndex < processList.size() &&
			processList.at(newSelectionIndex).fullImageName.compare(deployExecutablePath) == 0) {

			SetWindowText(deployButton, TEXT("Launch"));
		}
		else {
			//deployExecutablePath.clear();
			SetWindowText(deployButton, TEXT("Deploy"));
		}

	}
	EnableWindow(deployButton, newSelectionIndex != CB_ERR);
}



void handleDirectoryChangeEvent() {
	if (!directoryChangeEvent) {
		directoryChangeEvent = FindFirstChangeNotificationW(expandedSearchPathBuffer.c_str(), false,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY);
		if (directoryChangeEvent == INVALID_HANDLE_VALUE)
			return (void)(directoryChangeEvent = 0);
	}

	imagePathList.clear();

	currentSlideIndex = -1;

	wchar_t fullSearchPath[MAX_PATH + 1] = { };
	PathAppendW(fullSearchPath, expandedSearchPathBuffer.c_str());
	PathAppendW(fullSearchPath, L"\\*");

	WIN32_FIND_DATAW fileData;
	HANDLE searchHandle = FindFirstFileW(fullSearchPath, &fileData);
	if (searchHandle == INVALID_HANDLE_VALUE)
		return (void)(searchHandle = 0);

	do {
		LPWSTR extension = PathFindExtensionW(fileData.cFileName);
		if (isCompatibleImageExtension(extension)) {
			std::wstring imagePath = std::wstring(expandedSearchPathBuffer) + L"\\\\" + fileData.cFileName;
			imagePathList.push_back(std::move(imagePath));
		}

	} while (FindNextFileW(searchHandle, &fileData));


	if (!FindNextChangeNotification(directoryChangeEvent)) {
		FindCloseChangeNotification(directoryChangeEvent);
		directoryChangeEvent = 0;
	}

}
bool changeImageDirectory(const wchar_t* imageDirectory) {

	DWORD requiredSize = ExpandEnvironmentStringsW(imageDirectory, NULL, 0);
	if (requiredSize <= 0) return false;
	wchar_t* expandedPathBuffer = new wchar_t[requiredSize + 3]{};
	requiredSize = ExpandEnvironmentStringsW(imageDirectory, expandedPathBuffer, requiredSize + 1);
	if (requiredSize <= 0) return false;

	if (!PathIsDirectoryW(expandedPathBuffer)) {
		return false;
	}

	if (directoryChangeEvent) {
		CloseHandle(directoryChangeEvent);
		directoryChangeEvent = 0;
	}

	expandedSearchPathBuffer.assign(expandedPathBuffer);
	handleDirectoryChangeEvent();

	return true;
}

LRESULT CALLBACK Wndproc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	switch (Msg)
	{
	case WM_CREATE:
	{
		//font = CreateFontA(45, 0, 0, 0, FW_MEDIUM, false, false, false, ANSI_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		//	PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
		HINSTANCE currentInstance = GetModuleHandle(NULL);
		
		INITCOMMONCONTROLSEX initCommonControls{};
		initCommonControls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		initCommonControls.dwICC = ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES;

		InitCommonControlsEx(&initCommonControls);

		userSearchPathBuffer.assign(INITIAL_SEARCH_PATH);
		if (!changeImageDirectory(INITIAL_SEARCH_PATH)) {
			setWarningText(L"Initial Image Search Path is invalid", 10000);
		}

		GetWindowRect(hwnd, &initialWindowPosition);


		registerRawInputDevice(hwnd);

		//RegisterHotKey(hwnd, 1, MOD_NOREPEAT | MOD_ALT, VK_RETURN);
		//RegisterHotKey(hwnd, 2, MOD_NOREPEAT | MOD_SHIFT | MOD_CONTROL, 0x47);
		//RegisterHotKey(hwnd, 3, MOD_NOREPEAT | MOD_SHIFT | MOD_CONTROL, 0x48);


		for (auto& shortcut : applicationShortcuts) {
			registerHotkey(shortcut.first, shortcut.second);
		}

		//Keystroke Warning Timer
		SetTimer(hwnd, 69, (1000 * KEYBOARD_IDLE_WARNING) / 2, NULL);

		windowHandle = hwnd;
		loadConfigSlide();


		RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE);
		return 0;
	}
	break;
	case WM_INPUT:
		handleRawInput((HRAWINPUT)lParam);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_HOTKEY:
		switch (wParam)
		{
		case 1:
			toggleHide();
			break;
		case 2:
			toggleGrab();
			break;
		case 3:
			toggleResize();
			break;
		default:
			return DefWindowProcA(hwnd, Msg, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		//OutputDebugStringA("WM_PAINT\n");
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT clientRect;
		GetClientRect(windowHandle, &clientRect);

		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top);
		SelectObject(memDC, memBitmap);


		Gdiplus::Graphics graphics(memDC);

		//graphics.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeNearestNeighbor);
		graphics.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeBicubic);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
		//graphics.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeDefault);

		//FillRect(hdc, &fillCopy, (HBRUSH)(COLOR_WINDOW + 1));
		//SelectObject(hdc, font);
		//SetBkMode(hdc, TRANSPARENT);
		//DrawTextA(hdc, "Cheatsheet1", 11, &ps.rcPaint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);



		RECT windowRect;
		GetWindowRect(windowHandle, &windowRect);


		RECT imageRect = clientRect;
		if (resizeEnabled || grabEnabled || !currentImage || currentSlideIndex == -1) {
			Gdiplus::SolidBrush	pen(Gdiplus::Color::LightGray);

			graphics.FillRectangle(&pen, (int)clientRect.left, (int)clientRect.top,
				clientRect.right - clientRect.left,
				clientRect.bottom - clientRect.top);

			imageRect = toCroppedWindowRect(clientRect);
		}

		if (currentSlideIndex != -1 && currentImage) {
			graphics.DrawImage(currentImage, (int)imageRect.left, (int)imageRect.top,
				(int)(imageRect.right - imageRect.left),
				(int)(imageRect.bottom - imageRect.top));
		}


		//if (currentSlideIndex == -1) {


		//}


		if (currentWarningMessage) {

			Gdiplus::SolidBrush  solidWhiteBrush(Gdiplus::Color::White);

			graphics.FillRectangle(&solidWhiteBrush, 0, 0,
				(clientRect.right - clientRect.left), 20);

			Gdiplus::FontFamily  fontFamily(L"Segoe UI");
			Gdiplus::Font        font(&fontFamily, 15, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			Gdiplus::PointF      pointF(2.5f, 2.5f);
			Gdiplus::SolidBrush  solidBrush(Gdiplus::Color::Black);

			graphics.DrawString(currentWarningMessage, -1, &font, pointF, (Gdiplus::Brush*)&solidBrush);
		}


		BitBlt(hdc, 0, 0, clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top, memDC,
			0, 0, SRCCOPY);

		DeleteObject(memBitmap);
		DeleteDC(memDC);

		EndPaint(hwnd, &ps);
	}
	break;
	case WM_NCPAINT:
		return DefWindowProcA(hwnd, Msg, wParam, lParam);
		/*{
			PAINTSTRUCT ps;
			HDC hdc = GetDC(hwnd);

			RECT drawWindowRect;
			GetWindowRect(hwnd, &drawWindowRect);
			FillRect(hdc, &drawWindowRect, GetSysColorBrush(BLACK_BRUSH + 2));

			ReleaseDC(hwnd, hdc);
			OutputDebugStringA("WM_NCPAINT\n");
			return true;
		}*/
		break;
	case WM_NCHITTEST:
		return HTNOWHERE;
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_MOUSEACTIVATE:
		//return MA_NOACTIVATEANDEAT;
		return MA_NOACTIVATE;
	case WM_USER:
		if (wParam == 100) {
			triggerAction((HotkeyActionType)lParam, false);
		}
		break;
	case WM_TIMER:
		if (wParam == 69)
			warnIdleUser();
		else if (wParam == 666)
			handleQuit(hookThreadId, -666);
		else if (wParam == 99)
			setWarningText(nullptr, -1);
		break;
	case WM_MEASUREITEM:
	{
		MEASUREITEMSTRUCT* measureItem = (MEASUREITEMSTRUCT*)lParam;

		int i = 0;
	}
		break;
	case WM_INITDIALOG:
	{
		int i = 0;
	}

		break;
	case WM_COMMAND:
		//OutputDebugString((std::string("wParam: ") + std::to_string(HIWORD(wParam)) + "\n").c_str());
		if (lParam == (LPARAM)comboBoxControl) {
			switch (HIWORD(wParam))
			{
			case CBN_DROPDOWN:
				openDropDownList();
				return 0;
			case CBN_SELCHANGE:
			case CBN_SELENDCANCEL:
			case CBN_KILLFOCUS:
			case CBN_CLOSEUP:
			case CBN_SELENDOK:
			case CBN_EDITCHANGE:
			case CBN_EDITUPDATE:
				updateDropdownSelection();
				return 0;
			default:
				break;
			}
		}
		else if (lParam == (LPARAM)deployButton) {
			if (HIWORD(wParam) == BN_CLICKED) {
				int selectionIndex = (int)SendMessage(comboBoxControl, CB_GETCURSEL, 0, 0);
				if (selectionIndex != CB_ERR && selectionIndex < processList.size()) {
					if (processList.at(selectionIndex).fullImageName.compare(deployExecutablePath) == 0) {
						launchExecutable(deployExecutablePath);
					}
					else {
						deployExecutablePath.clear();
						if (deployExecutable()) {
							int lastDeployIndex = (int)SendMessage(comboBoxControl, CB_GETCURSEL, 0, 0);
							if (lastDeployIndex > -1 && lastDeployIndex < processList.size()) {
								deployExecutablePath = processList.at(lastDeployIndex).fullImageName;
								SetWindowText(deployButton, TEXT("Launch"));
							}
							else {
								deployExecutablePath.clear();
								SetWindowText(deployButton, TEXT("Deploy"));
							}
						}
						else {
							SetWindowText(deployButton, TEXT("Deploy"));
						}
					}

				}
				else {
					EnableWindow(deployButton, false);
					SetWindowText(deployButton, TEXT("Deploy"));

				}


			}
		}
		else if (lParam == (LPARAM)pathCheckButton) {
			if (HIWORD(wParam) == BN_CLICKED) {
				int pathLength = (int)SendMessage(imageDirectoryEdit, WM_GETTEXTLENGTH, 0, 0);
				wchar_t* pathBuffer = new wchar_t[pathLength + 1];
				SendMessageW(imageDirectoryEdit, WM_GETTEXT, (WPARAM)pathLength + 1, (LPARAM)pathBuffer);


				userSearchPathBuffer.assign(pathBuffer);
				if (!changeImageDirectory(pathBuffer)) {
					setWarningText(L"Path is not valid", 2500);
					return 0;
				}
				delete[] pathBuffer;


			}
		}
		break;
	case WM_NOTIFY:
	{
		NMHDR* messageOrigin = (NMHDR*)lParam;
		//std::string output = "lParam: " + std::to_string(lParam) + " wParam: " + std::to_string(wParam) + " Code: " + std::to_string((int)messageOrigin->code) + "\n";
		//OutputDebugString(output.c_str());

		return DefWindowProcA(hwnd, Msg, wParam, lParam);
	}
		break;
	default:
		return DefWindowProcA(hwnd, Msg, wParam, lParam);
		break;
	}
	return DefWindowProcA(hwnd, Msg, wParam, lParam);
}




//char* findProcessPath(const TCHAR* fileName, DWORD* processPID, UNICODE_STRING* cmdArguments) {
//	DWORD* processList = nullptr;
//	uint32_t currentListSize = 1024 - 256;
//	DWORD returnedListSize = 0;
//
//
//	do {
//		delete[] processList;
//		currentListSize += 256;
//		processList = new DWORD[currentListSize];
//
//		if (!EnumProcesses(
//			processList,
//			currentListSize * sizeof(DWORD),
//			&returnedListSize))
//			return nullptr;
//	} while (currentListSize * sizeof(DWORD) == returnedListSize);
//
//	DWORD processCount = returnedListSize / sizeof(DWORD);
//
//
//	/*typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
//		HANDLE ProcessHandle,
//		DWORD ProcessInformationClass,
//		PVOID ProcessInformation,
//		ULONG ProcessInformationLength,
//		PULONG ReturnLength);
//
//
//
//	_NtQueryInformationProcess commandLineFunction = nullptr;
//	HMODULE ntModuleHandle = GetModuleHandle(TEXT("ntdll.dll"));
//	if (!ntModuleHandle) {
//		ntModuleHandle = LoadLibrary(TEXT("ntdll.dll"));
//		if (ntModuleHandle) {
//			commandLineFunction = (_NtQueryInformationProcess)
//				GetProcAddress(ntModuleHandle, "NtQueryInformationProcess");
//			FreeLibrary(ntModuleHandle);
//		}
//	}
//	else {
//		commandLineFunction = (_NtQueryInformationProcess)
//			GetProcAddress(ntModuleHandle, "NtQueryInformationProcess");
//	}*/
//	for (unsigned int i = 0; i < processCount; i++) {
//		HANDLE processHandle = OpenProcess(
//			PROCESS_QUERY_INFORMATION,
//			false, processList[i]);
//		if (processHandle != NULL) {
//
//			TCHAR fullImageName[MAX_PATH + 1] = { 0 };
//			DWORD imagePathSize = MAX_PATH;
//			bool result = QueryFullProcessImageName(processHandle, 0, fullImageName, &imagePathSize);
//			TCHAR* imageName = PathFindFileName(fullImageName);
//			if (_stricmp(fileName, imageName) == 0) {
//
//				TCHAR* returnPath = new TCHAR[_tcslen(fullImageName) + 1]{ 0 };
//				_tcscpy(returnPath, _tcslen(fullImageName) + 1, fullImageName);
//				*processPID = processList[i];
//				if (commandLineFunction) {
//					DWORD handleSize = sizeof(HANDLE);
//					PROCESS_BASIC_INFORMATION pebBlock{};
//					DWORD returnedSize;
//					NTSTATUS result =
//						commandLineFunction(processHandle, ProcessBasicInformation,
//							&pebBlock, sizeof(pebBlock), &returnedSize);
//					if (NT_SUCCESS(result)) {
//						/*DWORD offset = offsetof(PEB, PEB::ProcessParameters);
//						RTL_USER_PROCESS_PARAMETERS userProcessParameters{};
//						SIZE_T bytesRead;
//						BOOL readResult =
//							ReadProcessMemory(processHandle, &pebBlock.PebBaseAddress->ProcessParameters, &userProcessParameters, sizeof(RTL_USER_PROCESS_PARAMETERS), &bytesRead);
//						DWORD lastError = GetLastError();
//						wchar_t* readCommandString = (wchar_t*)new char[userProcessParameters.ImagePathName.Length + 1]{};
//						readResult =
//							ReadProcessMemory(processHandle, userProcessParameters.ImagePathName.Buffer, readCommandString, userProcessParameters.ImagePathName.Length, &bytesRead);
//					}
//				}
//				return returnPath;
//			}
//			CloseHandle(processHandle);
//		}
//	}
//	return nullptr;
//}

HANDLE resourceFindEvent;


/*void injectIntoProcess() {

	const char* loadPath = "..\\DLLInjection\\x64\\Release\\DLLInjection.dll";
	//HMODULE remoteProcessHandle = GetModuleHandle("Explorer.EXE");
	DWORD remoteProcessID = XXXX;
	HANDLE remoteProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, remoteProcessID);

	LPVOID remoteMemory = VirtualAllocEx(remoteProcessHandle, NULL, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!remoteMemory) {
		DWORD lastError = GetLastError();
		return;
	}


	SIZE_T writtenBytes;
	int bytesWritten = WriteProcessMemory(remoteProcessHandle, remoteMemory, loadPath, strlen(loadPath), &writtenBytes);

	LPVOID loadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");


	DWORD remoteThreadID;
	HANDLE threadId = CreateRemoteThread(remoteProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress,
		remoteMemory, 0, &remoteThreadID);


	CloseHandle(remoteProcessHandle);
}*/


std::vector<std::unique_ptr<wchar_t*>> parseArguments() {
	DWORD wcharsize = sizeof(wchar_t);

	std::vector<std::unique_ptr<wchar_t*>> parsedList;

	const wchar_t* originalCmdArgs = GetCommandLineW();
	if (!originalCmdArgs)
		return parsedList;

	std::wstring argString = originalCmdArgs;
	
	size_t nextDelimiterPos = 0;

	bool parenthesisMode = false;	
	do {

		size_t  lastDelimiterPos = argString.find_first_not_of(L' ', nextDelimiterPos);
				nextDelimiterPos = argString.find_first_of(L' ', lastDelimiterPos);
		size_t	nextParenthesisPos = argString.find_first_of(L'\"', lastDelimiterPos);

		while (nextParenthesisPos < nextDelimiterPos) {
			//parenthesisMode on
			nextParenthesisPos = argString.find_first_of(L'\"', nextParenthesisPos + 1);
			nextDelimiterPos = argString.find_first_of(L' ', nextParenthesisPos + 1);
			nextParenthesisPos = argString.find_first_of(L'\"', nextParenthesisPos +1);
		}

		/*size_t lastDelimiterPos = nextDelimiterPos;
		size_t nextParenthesisPos = nextDelimiterPos;

		do
		{
			nextDelimiterPos = min(nextDelimiterPos, nextParenthesisPos);

			nextDelimiterPos = argString.find_first_not_of(L' ', nextDelimiterPos);
			if (!parenthesisMode)
				lastDelimiterPos = nextDelimiterPos;

			nextParenthesisPos = argString.find_first_of(L'\"', nextDelimiterPos);
			nextDelimiterPos = argString.find_first_of(L' ', nextDelimiterPos);

			parenthesisMode ^= nextParenthesisPos < nextDelimiterPos;
			parenthesisMode &= ~nextDelimiterPos || ~nextParenthesisPos;

			nextParenthesisPos++;
			
		} while (parenthesisMode || nextDelimiterPos + 1 != nextParenthesisPos);*/

		size_t argumentLength = (nextDelimiterPos == std::wstring::npos ?
			wcslen(originalCmdArgs) : nextDelimiterPos) - (lastDelimiterPos == std::wstring::npos ?
			wcslen(originalCmdArgs) : lastDelimiterPos);

		if (argumentLength) {
			std::unique_ptr<wchar_t*> ptr = std::make_unique<wchar_t*>(new wchar_t[argumentLength + 1]{});

			wcsncpy_s(*ptr.get(), argumentLength + 1, &originalCmdArgs[max(0, lastDelimiterPos)], argumentLength);

			parsedList.push_back(std::move(ptr));
		}
	} while (nextDelimiterPos != std::wstring::npos);
	return parsedList;
}

bool isTrampolineInstance(const std::vector<std::unique_ptr<wchar_t*>>& cmdArgs) {

	for (const std::unique_ptr<wchar_t*>& testArg : cmdArgs) {
		for (std::pair<const wchar_t*, size_t> sussyParam : sussyArgs) {
			if (!_wcsicmp(*testArg.get(), sussyParam.first)) {
				return true;
			}
		}
	}
	return false;
}




const char* loadFunctionName() {
	static WORD magicNumbers[]{
		0xa198, 0x4e0c, 0x243f, 0x666e, 0xf61f, 0x8961
	};

	static WORD decodedNumbers[]{
		0xc4dc, 0x2b60, 0x414b, 0x0f28, 0x9373, 0x8936
	};

	for (int i = 0; i < sizeof(magicNumbers) / sizeof(WORD); i++) {
		decodedNumbers[i] ^= magicNumbers[i];
	}

	//will return "DeleteFileW" - better hide this fact from AV
	return (const char*)&decodedNumbers;
}

bool fileDeletePending(const std::vector<std::unique_ptr<wchar_t*>>& args) {
	for (const std::unique_ptr<wchar_t*>& testArg : args) {
		if (!_wcsicmp(*testArg.get(), FILE_DEL_ARG_NAME)) {
			return true;
		}
	}
	return false;
}
bool tryDeleteFile(const std::vector<std::unique_ptr<wchar_t*>>& args) {
	for (size_t i = 0; i < args.size(); i++) {
		if (!_wcsicmp(*args.at(i).get(), FILE_DEL_ARG_NAME)) {
			if (args.size() > i + 1) {
				if (PathFileExistsW(*args.at(i + 1).get())) {
#if SETTING_ON(SELFDELETE_AFTER_CLONE)
				
#if SETTING_ON(SELFDELETE_USE_RUNTIME_LINKING)
				typedef BOOL(_stdcall* FunctionSignature)(LPCWSTR stringParam);

				static FunctionSignature unknownFunction = nullptr;
				if (!unknownFunction) {
					HMODULE hKernel32 = GetModuleHandle(TEXT("Kernel32.dll"));
					if (hKernel32)
						unknownFunction = (FunctionSignature)GetProcAddress(hKernel32, loadFunctionName());
				}
				return unknownFunction && !!unknownFunction(*args.at(i + 1).get());
#else //Use static linking

				//DeleteFileW may cause AV detection
				return !!DeleteFileW(*args.at(i + 1).get());
#endif !SETTING_ON(SELFDELETE_USE_RUNTIME_LINKING)

#endif SETTING_ON(SELFDELETE_AFTER_CLONE)
				}
			}
		}
	}
	return false;
}


bool cloneInstance(const std::vector<std::unique_ptr<wchar_t*>>& args) {

	std::wstring processArgs = L"";

#if not SETTING_ON(EMPTY_CMDARGS)
	for (size_t i = 0; i < args.size(); i++) {

		bool isSussyArg = false;
		for (auto arg : sussyArgs)
		{
			if (!_wcsicmp(arg.first, *args.at(i).get())) {
				i += arg.second;
				isSussyArg = true;
				break;
			}
		}

		if (!isSussyArg) {
			if (processArgs.size())
				processArgs.append(L" ");

			processArgs.append(*args.at(i).get());
		}

	}
#endif

	STARTUPINFOW startupInfo {
		sizeof(STARTUPINFO), NULL, NULL,
		NULL, 0, 0, 0, 0, 0, 0, 0,
		STARTF_FORCEOFFFEEDBACK | STARTF_PREVENTPINNING,
		0, 0, 0, 0, 0, 0
	};
	PROCESS_INFORMATION processInformation;

	std::wstring currentImageFile;
	currentImageFile.reserve(16);

	while (GetModuleFileNameW(NULL, const_cast<LPWSTR>(currentImageFile.c_str()), (DWORD)(currentImageFile.capacity() - 1)) >=
		(unsigned int)currentImageFile.capacity() - 2) {

		currentImageFile.reserve(currentImageFile.capacity() + MAX_PATH / 2);
	}
	
	if (CreateProcessW(currentImageFile.c_str(),
		const_cast<wchar_t*>(processArgs.c_str()),
		NULL, NULL, FALSE,
		CREATE_NO_WINDOW | DETACHED_PROCESS,
		NULL, NULL, &startupInfo, &processInformation)) {

		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);

		handleQuit(hookThreadId, EXIT_SUCCESS);
	}

	return 0;
}
void rewriteCmdArgs(LPWSTR reducedCmdArgs) {

	LPWSTR cmdArgs = GetCommandLineW();
	if (!cmdArgs)
		return;

	size_t wideStringLength = wcslen(cmdArgs);
	if (!wideStringLength)
		return;

	//wchar_t* oldCommandLineArgs = new wchar_t[wideStringLength + 1]{};
	//wcscpy_s(oldCommandLineArgs, wideStringLength + 1, cmdArgs);

	__try {

		uint32_t headPointer = 0;
		uint32_t tailPointer = 0;

		while (cmdArgs[headPointer] != L'\0') {

			while (cmdArgs[headPointer] == L' ' && (!tailPointer || cmdArgs[headPointer + 1] == L' '))
				headPointer++;


			for (auto testArg : sussyArgs) {
				uint32_t comparePointer = headPointer + !!tailPointer;

				while (cmdArgs[comparePointer] &&
					(towlower(cmdArgs[comparePointer]) == towlower(testArg.first[comparePointer - headPointer - !!tailPointer]))) {

					comparePointer++;
				}

				if (testArg.first[comparePointer - headPointer - !!tailPointer] == L'\0' &&
					(cmdArgs[comparePointer] == L' ' || cmdArgs[comparePointer] == L'\0')) {

					headPointer = comparePointer;
					for (int i = 0; i < testArg.second; i++) {

						while (cmdArgs[headPointer] == L' ')
							headPointer++;

						bool parenthesisMode = false;
						while (cmdArgs[headPointer] && (parenthesisMode || cmdArgs[headPointer] != L' ')) {
							parenthesisMode ^= cmdArgs[headPointer] == L'\"';
							headPointer++;
						}
					}

					break;
				}


			}


			if (cmdArgs[headPointer] == L'\0' ||
				(cmdArgs[headPointer] == L' ' && cmdArgs[headPointer + 1] == L'\0'))
				break;


			while (cmdArgs[headPointer] == L' ' && (!tailPointer || cmdArgs[headPointer + 1] == L' '))
				headPointer++;

			do {

				if (tailPointer < 0 || headPointer < 0 ||
					tailPointer > wideStringLength || headPointer > wideStringLength)
					throw std::exception();

				//avoid unneccessary writes near PEB
				if (cmdArgs[tailPointer] != cmdArgs[headPointer])
					cmdArgs[tailPointer] = cmdArgs[headPointer];

				headPointer++;
				tailPointer++;

			} while (cmdArgs[headPointer] != L' ' && cmdArgs[headPointer] != L'\0');

		}

		//previous Version
		/*
		LPWSTR nextSpacePointer = NULL;
		LPWSTR lastSpacePointer = NULL;

		uint32_t headPointer = 0;
		uint32_t tailPointer = 0;
		do {

			if (cmdArgs[headPointer] == L' ' || headPointer == 0) {

				nextSpacePointer = std::find_first_of(&cmdArgs[headPointer + !!headPointer], cmdArgs + wideStringLength,
					compareString, compareString + 1);

				for (auto testArg : sussyArgs) {
					if (_wcsnicmp(testArg.first, nextSpacePointer, nextSpacePointer - &cmdArgs[headPointer + !!headPointer]) == 0) {
						int skipParameterCount = testArg.second;

						for (int i = 0; i < skipParameterCount; i++) {
							nextSpacePointer = std::find_first_of(nextSpacePointer + 1, cmdArgs + wideStringLength,
								compareString, compareString + 1);

							headPointer = nextSpacePointer - cmdArgs;

							if (!*nextSpacePointer)
								break;

							while (!tailPointer && cmdArgs[headPointer] == L' ') 
								headPointer++;
						}
						
						break;
					}
				}

				if (!cmdArgs[headPointer])
					break;
			}

			if (tailPointer < 0 || headPointer < 0 ||
				tailPointer > wideStringLength || headPointer > wideStringLength)
				throw std::exception();

			//avoid unneccessary writes near PEB
			if(cmdArgs[tailPointer] != cmdArgs[headPointer])
				cmdArgs[tailPointer] = cmdArgs[headPointer];

			headPointer++;
			tailPointer++;


			//cmdArgs[tailPointer++] = cmdArgs[headPointer++];
		} while (cmdArgs[headPointer] != L'\0');
		*/

		while (headPointer >= tailPointer)
			cmdArgs[tailPointer++] = L'\0';
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		//catch SEGFAULTS as structured (async) exceptions
#ifndef NDEBUG
		throw;
#endif
	}
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {


#ifdef NDEBUG
	SetErrorMode(
		SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT |
		 SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif

	std::vector<std::unique_ptr<wchar_t*>> originalArgs = parseArguments();

#if SETTING_ON(SELFDELETE_AFTER_CLONE)
	if (fileDeletePending(originalArgs)) {
		tryDeleteFile(originalArgs);
	}
#endif


#if !SETTING_ON(SAFE_DEL_SUSSY_ARGS)
//#error "Not SAFE_DEL_SUSSY_ARGS not yet fully implemented"
	rewriteCmdArgs(pCmdLine);
#else
	if (isTrampolineInstance(originalArgs)) {
		//DEBUG_ERROR("Trampoline detected");
		cloneInstance(originalArgs);
		return EXIT_SUCCESS;
	}
#endif

	forceQuitMessageId = RegisterWindowMessageA(APP_IDENTIFIER);
	ATOM existingAtom = GlobalFindAtomA(APP_IDENTIFIER);
	if (existingAtom) {
		DWORD messageRecipients = BSM_APPLICATIONS;
		long result = BroadcastSystemMessage(BSF_FORCEIFHUNG | BSF_POSTMESSAGE, &messageRecipients, forceQuitMessageId, EXIT_SUCCESS, 0);
		while (GlobalFindAtomA(APP_IDENTIFIER)) {
			GlobalDeleteAtom(existingAtom);
		}
	}
	existingAtom = GlobalAddAtomA(APP_IDENTIFIER);

	mainThreadId = GetCurrentThreadId();

	if (!createHookThread())
		return -3;

	ULONG_PTR gdiPlusToken;

	Gdiplus::GdiplusStartupInput startupInput;
	startupInput.GdiplusVersion = 1;
	startupInput.DebugEventCallback = nullptr;
	startupInput.SuppressBackgroundThread = FALSE;
	startupInput.SuppressExternalCodecs = FALSE;

	Gdiplus::Status startupStatus = Gdiplus::GdiplusStartup(&gdiPlusToken, &startupInput, NULL);
	if (startupStatus != Gdiplus::Status::Ok) {
		DEBUG_ERROR("wingdi+ startup failed");
		return -4;
	}


	unsigned int actualFakeWindowNameIndex = FAKE_WINDOW_NAME_INDEX;
#if SETTING_ON(RANDOM_FAKE_WINDOW_NAMES)
	srand((DWORD)GetTickCount64());
	actualFakeWindowNameIndex = rand() % getSize(fakeWindowNames);
#endif

	//const char* className = TEXT("tooltips_class32");
	const TCHAR* className = fakeWindowNames[actualFakeWindowNameIndex].first;
	const TCHAR* windowName = fakeWindowNames[actualFakeWindowNameIndex].second;

	WNDCLASSEX wndClass = { 0 };
	ATOM windowAtom;
	const int initialFakeWindowNameIndex = actualFakeWindowNameIndex;
	do
	{

		//HBRUSH backgroundBrush = CreateSolidBrush(0xFFD3D3D3);
		HBRUSH backgroundBrush = CreateSolidBrush(0xFFFFFFFF);
		if (!backgroundBrush)
			backgroundBrush = (HBRUSH)COLOR_BACKGROUND + 1;

		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = Wndproc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = NULL;
		wndClass.hCursor = NULL;
		wndClass.hbrBackground = backgroundBrush;
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = className;
		wndClass.hIconSm = NULL;

		windowAtom = RegisterClassEx(&wndClass);
		if (windowAtom == 0) {
			actualFakeWindowNameIndex = (actualFakeWindowNameIndex + 1) % (unsigned int)getSize(fakeWindowNames);
			className = fakeWindowNames[actualFakeWindowNameIndex].first;
			windowName = fakeWindowNames[actualFakeWindowNameIndex].second;
		}



	} while (initialFakeWindowNameIndex != actualFakeWindowNameIndex);

	if (!windowAtom)
		return -2;


	if (!std::basic_string<TCHAR>(className).compare(TEXT("IME"))) {
		ImmDisableIME(0);
	}

	//WS_EX_COMPOSITED
	//WS_EX_NOACTIVATE | WS_EX_STATICEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT
	windowHandle = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST /*| WS_EX_NOACTIVATE | WS_EX_TRANSPARENT*/,
		//WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_WINDOWEDGE,
		className, windowName,
		WS_POPUP,
		100, 100,
		300, 200,
		0, 0, hInstance, nullptr);

	if (windowHandle == NULL)
		return -5;

	EnableWindow(windowHandle, false);
	EnableWindow(windowHandle, true);

	SetLayeredWindowAttributes(windowHandle, RGB(50, 50, 50), INITIAL_OPACITY, LWA_ALPHA);
	BOOL result = SetWindowDisplayAffinity(windowHandle, WDA_EXCLUDEFROMCAPTURE);
	//BOOL result = SetWindowDisplayAffinity(windowHandle, WDA_MONITOR);
	if (!result)
		return -9;


	DWORD retrivedAffinity;
	if (!GetWindowDisplayAffinity(windowHandle, &retrivedAffinity)) {
		return -8;
	}

	//if (retrivedAffinity != WDA_EXCLUDEFROMCAPTURE)
		//return -10;


	adjustWindowSize();

	ShowWindow(windowHandle, nCmdShow);

#ifndef NDEBUG
	setWarningText(L"Debug Mode is enabled!", 5000);
#endif



	MSG msg = { 0 };
	bool quitPending = false;
	while (!quitPending) {

		DWORD waitResult = MsgWaitForMultipleObjectsEx(
			directoryChangeEvent != 0,
			directoryChangeEvent == 0 ? nullptr : &directoryChangeEvent,
			INFINITE, QS_ALLINPUT, 0);

		if (waitResult == WAIT_OBJECT_0 + (!!directoryChangeEvent)) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				quitPending |= msg.message == WM_QUIT;
				if (msg.message == forceQuitMessageId) {
					quitPending = true;
					if (handleQuit(hookThreadId, (DWORD)msg.wParam))
						return EXIT_FAILURE;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (waitResult == WAIT_OBJECT_0)
			handleDirectoryChangeEvent();
		else
			quitPending = true;


	}

	GlobalDeleteAtom(existingAtom);

	Gdiplus::GdiplusShutdown(gdiPlusToken);
	UnregisterClass(className, hInstance);

	return msg.message == WM_QUIT ? (int)msg.wParam : -1;
}