#include "hk/gfx/DebugRenderer.h"
#include "hk/hook/Trampoline.h"
#include "hk/svc/api.h"

#include "nn/fs.h"

HkTrampoline sdFileCreate = [](TrampolineStatic(), void* a1) -> void {
    orig(a1);
    nn::fs::MountSdCard("sd");

    nn::fs::CreateFile("sd://Hi_there.txt", 0);

};

HkTrampoline LMS_GetTextHook = [](TrampolineStatic(), void* a1, int a2) -> int {
    return orig(a1, 5);
};

// Runs every frame
HkTrampoline testHook = [](TrampolineStatic(), int a1) -> void {
    orig(a1);
    hk::svc::OutputDebugString("Hi!\n", 5);
};

extern "C" void hkMain() {
    testHook.installAtMainOffset(0x509e10);
    sdFileCreate.installAtOffset(hk::ro::getMainModule(), 0x50c9c0);
    LMS_GetTextHook.installAtMainOffset(0x4f0ba0);

   // hk::gfx::DebugRenderer::instance()->installHooks();
}
