//
// Created by root on 17-11-6.
//

#ifndef ENGINE_ENDIAN_H
#define ENGINE_ENDIAN_H

#endif //ENGINE_ENDIAN_H

#include <endian.h>

namespace sockets
{

    inline uint16_t hostToNetwork16(uint16_t host16)
    {
        return htobe16(host16);
    }

    inline uint32_t hostToNetwork32(uint32_t host32)
    {
        return htobe32(host32);
    }

    inline uint16_t networkToHost16(uint16_t net16)
    {
        return be16toh(net16);
    }

}