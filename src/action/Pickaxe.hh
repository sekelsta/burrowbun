#ifndef PICKAXE_HH
#define PICKAXE_HH

#include "Block.hh"

/* Items that can damage blocks. */
class Pickaxe: public Block {
    int blockDamage;
    int tier;
public:
    /* Constructor. */
    Pickaxe(ActionType type, std::string path);

    /* What to do when used. */
    virtual bool use_internal(InputType type, int x, int y, World &world);
};

#endif
