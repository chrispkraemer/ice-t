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
    void checkpoint() {
        codal::ManagedString hello = "there";
        uBit.display.print(hello);
        uBit.wait(100);
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
