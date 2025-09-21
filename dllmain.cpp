#include <windows.h>

// DllMain inerte: SDRuno controla el ciclo de vida vía CreatePlugin/DestroyPlugin.
// No crear/destruir instancias aquí para evitar dobles destrucciones o problemas de descarga.
BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}
