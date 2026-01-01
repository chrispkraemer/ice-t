/**
 * This file contain test code for extension development.
 * It will not be included or compiled when this package is added to a project as an extension
 */

serial.setBaudRate(BaudRate.BaudRate115200)
let var1 = 0
ice_t.Restore()
serial.writeLine("hello")
basic.forever(() => {
    //basic.showString("Help")
    basic.showNumber(var1)
    var1++;
    ice_t.Checkpoint()
    basic.pause(1000)
    //basic.showString("now")
    //ice_t.Checkpoint()
})
