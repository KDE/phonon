#!/usr/bin/env bash
$EXTRACT_TR_STRINGS $(find . -name \*.cpp -o -name \*.h -o -name \*.ui -o -name \*.qml) -o $podir/phononsettings6_qt.pot
$EXTRACT_TR_STRINGS $(find . -name \*.cpp -o -name \*.h -o -name \*.ui -o -name \*.qml) -o $podir/phononsettings_qt.pot
