/**
 * C++ functions as part of the extension.
 * 
 * A function defined here will run on the micro:bit, there is also a "shim"
 * in the main.ts file that will be run in the simulator.
 * Info about shims: https://makecode.com/simshim
 */
#include "pxt.h"

using namespace pxt;

namespace ice_t {

    /**
     * function to checkpoint the user's code
     */
    //%
    int ice_t_checkpoint() {
        uBit.display.image.clear();
        return 1;
    }

    /**
     * This function uses the C preprocessor to compile different code
     * depending on the version of the micro:bit.
     */
    //%
    int ice_t_restore() {
        uBit.display.image.clear();
        return 1;
    }

}
