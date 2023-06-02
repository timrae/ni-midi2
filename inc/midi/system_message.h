#pragma once

//--------------------------------------------------------------------------

#include <midi/types.h>
#include <midi/universal_packet.h>

//--------------------------------------------------------------------------

#include <cassert>
#include <optional>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

struct system_message : universal_packet
{
    constexpr system_message();
    constexpr system_message(group_t, status_t, uint7_t data1 = 0, uint7_t data2 = 0);
    ~system_message() = default;
};

//--------------------------------------------------------------------------

constexpr bool is_system_message(const universal_packet&);

//--------------------------------------------------------------------------

struct system_message_view
{
    constexpr explicit system_message_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::system);
    }

    constexpr group_t  group() const { return p.group(); }
    constexpr status_t status() const { return p.status(); }
    constexpr uint7_t  data_byte_1() const { return p.byte3() & 0x7Fu; }
    constexpr uint7_t  data_byte_2() const { return p.byte4() & 0x7Fu; }

    constexpr uint14_t get_song_position() const;

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<system_message_view> as_system_message_view(const universal_packet&);

//--------------------------------------------------------------------------

constexpr system_message make_system_message(group_t, status_t, uint7_t data1 = 0, uint7_t data2 = 0);
constexpr system_message make_song_position_message(group_t, uint14_t position);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr system_message::system_message()
  : universal_packet(0x10000000)
{
}
constexpr system_message::system_message(group_t group, status_t status, uint7_t data1, uint7_t data2)
  : universal_packet(0x10000000u | ((group & 0x0F) << 24) | (status << 16) | (data1 << 8) | data2)
{
}

//--------------------------------------------------------------------------

constexpr bool is_system_message(const universal_packet& p)
{
    return (p.type() == packet_type::system);
}

//--------------------------------------------------------------------------

constexpr uint14_t system_message_view::get_song_position() const
{
    if (p.status() == system_status::song_position)
    {
        return data_byte_1() | (data_byte_2() << 7);
    }

    return 0;
}

//--------------------------------------------------------------------------

constexpr std::optional<system_message_view> as_system_message_view(const universal_packet& p)
{
    if (is_system_message(p))
        return system_message_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

constexpr system_message make_system_message(group_t group, status_t status, uint7_t data1, uint7_t data2)
{
    return system_message{ group, status, data1, data2 };
}

constexpr system_message make_song_position_message(group_t group, uint14_t position)
{
    return make_system_message(group,
                               system_status::song_position,
                               static_cast<uint7_t>(position & 0x7F),
                               static_cast<uint7_t>((position >> 7) & 0x7F));
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
