//
// Created by Alexey Pashinov on 10/09/20
//

#pragma once

// system includes
#include <optional>
#include <string>
#include <variant>

// proto
#include <lion.pb.h>

namespace sys
{
    namespace power
    {
        int set(lion_protocol::PowerType type, bool value);
    }

    namespace sysinfo
    {
        std::optional<std::variant<bool, double, std::string, std::uint32_t, std::int32_t>> get(lion_protocol::SysInfoType type);
    }
}
