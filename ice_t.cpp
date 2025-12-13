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
    #define V1_MULTIPLIER 2
    #define V2_MULTIPLIER 3

    /**
     * function to checkpoint the user's code
     */
    //%
    void checkpoint() {
        codal::ManagedString hello = "there";
        uBit.display.print(hello);
        uBit.wait(100);
    }

     /**
     * This function uses the C preprocessor to compile different code
     * depending on the version of the micro:bit.
     */
    //%
    int banana_multiplier(int bananas) {
        #if MICROBIT_CODAL
            // On micro:bit V2 the multiplier is higher than V1
            return bananas * V2_MULTIPLIER;
        #else
            // On micro:bit V1 the multiplier is lower than V2
            return bananas * V1_MULTIPLIER;
        #endif
    }

    /**
     * function to restore a checkpoint
     */
    //%
    void restore() {
        codal::ManagedString hello ="goodbye";
        uBit.display.print(hello);
        uBit.wait(100);
    }

}
