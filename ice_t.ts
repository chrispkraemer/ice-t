/**
 * Provides access to all the banana functionality.
 *
 * Function docstrings appear as info the in JavaScript toolbox.
 *
 * A function defined here with a "shim" entry will run this code in the
 * simulator, and the C++ code will run in the micro:bit.
 *
 * Info about shims: https://makecode.com/simshim
 * Info about defining blocks: https://makecode.com/defining-blocks
 */

// Icon unicode characters can be found at: http://fontawesome.io/icons/
//% color=#c2b711 weight=100 icon="\uf1ec" block="ice_t" advanced=false
namespace ice_t {



    /**
     * Checkpoint the device
     */
    //% blockId=checkpoint
    //% block="checkpoint the device"
    //% shim=ice_t::checkpoint
    export function Checkpoint(): void {
        //this does something
    }

    /**
     * Restore a checkpoint
     */
    //% blockId=restore
    //% block="Restore a checkpoint"
    //% shim=ice_t::restore
    export function Restore(): void { 
        // The simulator does not differentiate between board version
        // so the TypeScript code will always run
    }
}
