// Auto-generated. Do not edit.


    /**
     * C++ functions as part of the extension.
     * 
     * A function defined here will run on the micro:bit, there is also a "shim"
     * in the main.ts file that will be run in the simulator.
     * Info about shims: https://makecode.com/simshim
     */

declare namespace ice_t {

    /**
     * function to checkpoint the user's code
     */
    //% shim=ice_t::checkpoint
    function checkpoint(): void;

    /**
     * This function uses the C preprocessor to compile different code
     * depending on the version of the micro:bit.
     */
    //% shim=ice_t::banana_multiplier
    function banana_multiplier(bananas: int32): int32;

    /**
     * function to restore a checkpoint
     */
    //% shim=ice_t::restore
    function restore(): void;
}

// Auto-generated. Do not edit. Really.
