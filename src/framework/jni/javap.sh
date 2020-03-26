#!/bin/sh

#javap -classpath ../bin/classes -s com.sixrooms.rtmp.player.Event

javap -classpath $ANDROID_SDK/platforms/android-23/android.jar:../../../build/intermediates/classes/release -s $1
