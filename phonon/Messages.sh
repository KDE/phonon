#! /usr/bin/env bash
#$EXTRACTRC `find $dirs -maxdepth 1 \( -name \*.rc -o -name \*.ui -o -name \*.ui3 -o -name \*.ui4 -o -name \*.kcfg \) ` >> rc.cpp || exit 11
$EXTRACT_TR_STRINGS $(find . -maxdepth 1 -name "*.cpp" -o -name "*.h") -o $podir/libphonon_qt.pot
