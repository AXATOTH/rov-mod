#!/system/bin/sh

PKG="com.garena.game.kgth"

ui_print ""
ui_print "  ╔═══════════════════════════════════╗"
ui_print "  ║   ROV Minimap ESP - Enemy Vision  ║"
ui_print "  ╚═══════════════════════════════════╝"
ui_print ""
ui_print "  Target Package: $PKG"
ui_print ""

if ! pm list packages | grep -q "$PKG"; then
    ui_print "  ! Warning: ROV package not found"
    ui_print "  Module will activate when game is installed"
fi

set_perm_recursive $MODPATH 0 0 0755 0644
set_perm $MODPATH/lib/libminimap.so 0 0 0755

ui_print "  Installation complete!"
ui_print "  Open menu: Tap 3 fingers in-game"
ui_print ""
