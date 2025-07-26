#include "mapper.hpp"

Mapper::Mapper(u8 prgBanks, u8 chrBanks)
{
    prgBankCount = prgBanks;
    chrBankCount = chrBanks;
}

Mapper::~Mapper()
{
}
