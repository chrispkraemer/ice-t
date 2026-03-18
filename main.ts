/**
 * This file contain test code for extension development.
 * It will not be included or compiled when this package is added to a project as an extension
 */

serial.setBaudRate(BaudRate.BaudRate115200)
let var1 = 0
let var2 = 0xDEADBEEF
let var3 = 0xDEADBEEF
let var4 = 0xDEADBEEF
let var5 = 0xDEADBEEF
let var6 = 0xDEADBEEF
let var7 = 0xDEADBEEF
let var8 = 0xDEADBEEF
let var9 = 0xDEADBEEF
let var10 = 0xDEADBEEF
ice_t.init_FRAM()
if (input.buttonIsPressed(Button.A)) {
    ice_t.clear_FRAM()
}
ice_t.Restore()
serial.writeLine("hello")
while (1) {
    //basic.showString("Help")
    //let var3 = 0xffffffff
    //var2++
    basic.showNumber(var1)
    var1++;
    var2++;
    ice_t.Breakpoint()
    var3++;
    var4++;
    var5++;
    ice_t.Checkpoint()
    basic.pause(1000)
    //basic.showString("now")
    //ice_t.Checkpoint()
    if (input.buttonIsPressed(Button.A)) {
       ice_t.clear_FRAM() 
    }

}    
