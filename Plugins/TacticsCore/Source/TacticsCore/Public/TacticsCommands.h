#pragma once

#include "TacticsCoreTypes.h"
#include <cstdint>

namespace TacticsCore {

    using EntityId = uint32_t;

    enum class CommandType : uint8_t {
        None,
        Move,
        EndTurn
    };

    struct MoveCommand {
        EntityId eneity = 0;
        TilePos dest{};
    };

    struct Command {
        CommandType type = CommandType::None;

        MoveCommand move{};
    };

    inline Command MakeMove(EntityId entity, TilePos dest) noexcept {
        Command c;
        c.type = CommandType::Move;
        c.move = MoveCommand{entity, dest};
        return c;
    }

    inline Command MakeEndTurn() noexcept
    {
        Command C;
        C.type = CommandType::EndTurn;
        return C;
    }
}