#include "hk/gfx/DebugRenderer.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "hk/svc/api.h"

#include "nn/fs.h"
#include "hk/nvn/nvn_CppMethods.h"

HkTrampoline sdFileCreate = [](TrampolineStatic(), void* a1) -> void {
    orig(a1);
    nn::fs::MountSdCard("sd");

    nn::fs::CreateFile("sd://Hi_there.txt", 0);

};

HkTrampoline LMS_GetTextHook = [](TrampolineStatic(), void* a1, int a2) -> int {
    return orig(a1, 5);
};

struct someRenderClass {
    char unk_0[0xb0 - 0x0];
    nvn::CommandBuffer* commandBuffer;
    // ...

    void endCommandBufferRecording();
};

// Runs every frame
void renderHook(someRenderClass* _this) {
   // defer {ret = cmdBuf->EndRecording(); };

   // hk::svc::OutputDebugString("Hi!\n", 5);

    /* DebugRenderer */
    auto* renderer = hk::gfx::DebugRenderer::instance();

    renderer->clear();
    renderer->begin(_this->commandBuffer);

    renderer->setGlyphSize(0.45);

    renderer->drawQuad(
        { { 30, 30 }, { 0, 0 }, 0xef000000 },
        { { 300, 30 }, { 1.0, 0 }, 0xef000000 },
        { { 300, 100 }, { 1.0, 1.0 }, 0xef000000 },
        { { 30, 100 }, { 0, 1.0 }, 0xef000000 });

    renderer->setCursor({ 50, 50 });
    renderer->printf("Rhythm ReGrooved\n\n by techmuse, et al.");

    renderer->end();

    _this->endCommandBufferRecording();

   // return _this->commandBuffer->EndRecording();

}



extern "C" void hkMain() {
    sdFileCreate.installAtMainOffset(0x50cba0); // GameRun::GameRun(GameRun*)
    // LMS_GetTextHook.installAtMainOffset(0x4f0ce0); // LMS_GetText
    
    // this breaks
    // hk::hook::writeBranchLinkAtMainOffset(0x510b6c, &renderHook); // NXGraphicsImpl::FUN_7100510af0(NXGraphicsImpl*)

    hk::gfx::DebugRenderer::instance()->installHooks();
}


