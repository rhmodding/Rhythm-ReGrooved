#pragma once

#include "aloha/Stage.hpp"

class StageRing00 : public Stage {
    public:
        StageRing00(long arg1, int arg2, int arg3, int arg4, int arg5, int arg6);
        virtual ~StageRing00();

    private:
        char pad[0x7ad0];
};

static_assert(sizeof(StageRing00) == 0x7ad8, "you done fucked up");