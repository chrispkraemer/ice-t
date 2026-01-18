/**
 * This file contain test code for extension development.
 * It will not be included or compiled when this package is added to a project as an extension
 */

serial.setBaudRate(BaudRate.BaudRate115200)
let var1 = 0
ice_t.Restore()
serial.writeLine("hello")
while (1) {
    //basic.showString("Help")
    //let var2 = 0xDEADBEEE
    //let var3 = 0xffffffff
    //var2++
    basic.showNumber(var1)
    var1++;
    ice_t.Checkpoint()
    basic.pause(1000)
    //basic.showString("now")
    //ice_t.Checkpoint()
}    
