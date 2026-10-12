#include "session.hpp"

namespace nomi
{

const std::vector<Character>& characters()
{
    static const std::vector<Character> list = {
        { "Kit",    "kit_from_firefox.png" },
        { "Gnu",    "gnu_from_gnu.png" },
        { "Tux",    "tux_from_linux.png" },
        { "Wilber", "wilber_from_gimp.png" },
        { "Kisi",   "kisi_from_konsolscript.png" },
    };
    return list;
}

} // namespace nomi
