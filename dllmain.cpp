#include <windows.h>

// DllMain debe ser inerte: SDRuno controla el ciclo de vida del plugin
// mediante CreatePlugin/DestroyPlugin. No crear ni destruir objetos aquí.
// Esto evita dobles destrucciones y problemas de orden de descarga.
BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}
