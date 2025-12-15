/**
 * This file contain test code for extension development.
 * It will not be included or compiled when this package is added to a project as an extension
 */

basic.forever(() => {
    basic.showString("Help")
    ice_t.bananaMultiplier(5)
    ice_t.Checkpoint()
    ice_t.Restore()
    basic.showString("now")
    ice_t.Checkpoint()
})
