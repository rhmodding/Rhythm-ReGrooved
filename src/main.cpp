#include "hk/gfx/DebugRenderer.h"
#include "hk/hook/Trampoline.h"
#include "hk/svc/api.h"

#include "nn/fs.h"

HkTrampoline<void, void*> sdFileCreate = hk::hook::trampoline([](void* a1) -> void {
    sdFileCreate.orig(a1);
    nn::fs::MountSdCard("sd");

    nn::fs::CreateFile("sd://Hi_there.txt", 0);

});

HkTrampoline<int, void*, int> LMS_GetTextHook = hk::hook::trampoline([](void* a1, int a2) -> int {
    return LMS_GetTextHook.orig(a1, 5);
});

// Runs every frame
HkTrampoline<void, int> testHook = hk::hook::trampoline([](int a1) -> void {
    testHook.orig(a1);
    hk::svc::OutputDebugString("Hi!\n", 5);
});

extern "C" void hkMain() {
    testHook.installAtMainOffset(0x509e10);
    sdFileCreate.installAtOffset(hk::ro::getMainModule(), 0x50c9c0);
    LMS_GetTextHook.installAtMainOffset(0x4f0ba0);

   // hk::gfx::DebugRenderer::instance()->installHooks();
}
