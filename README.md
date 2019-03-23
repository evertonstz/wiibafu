# Why fork?
TL;DR: splited files weren't working on FAT32 partitions using default spliting settings, now they do.

Long:
As documented in [this ticket](https://sourceforge.net/p/wiibafu/tickets/9/#c208 "this ticket") in the original repository,  and extensively discussed in [this thread](https://gbatemp.net/threads/wii-backup-managers-in-gnu-linux-wbfs-conversion-issues.508655/ "this thread") over GBATemp, there's a crucial default setting in Wiibafu that'll break splited games for people using FAT32 partions, it can be manually fixed by people who really know what they're doing, but most people will just be induced to error. Since the development in the original repo has stoped since 2013, I've forked the project to fix this simple problem.

## The bug
Back when wiibafu was developed most people used the WBFS Partition system, the original dev, either knowing it or not, did set the default split size for the WBFS FILES at 4G (GiB), since WBFS Partitions doesn't have a maximum filesize cap, spliting files worked fine for people using this type of partition. Nowdays most people FAT32 partitions, as it easier to manage and mount, that's when the problem arrives: for FAT32 partitions the maximum filesize you can have is 4GB (4G-32K), and since 4G is bigger than 4GB, using wiibafu to split WBFS FILES will result in files bigger than FAT32 can read.

## The fix
I just changed the default split size wiibafu will offer the user fom "4G" to "4G-32K", therefore the default setting will output splited WBFS FILES everyone can use.

# Installation

## Arch Based
It's on [AUR](https://aur.archlinux.org/packages/wiibafu/ "AUR")

Binaries on [Releases](https://github.com/evertonstz/wiibafu/releases "Releases") can be installed with "sudo pacman -U wiibafu*.pacman", for now only x86_64 binaries are available

# Build From Source
More info [here](https://github.com/evertonstz/wiibafu/blob/master/INSTALL "here")

# Original README
This document contains important notes for Wii Backup Fusion


### Look & Feel of Qt applications

You can configure the look & feel of Qt applications, incl. Wii Backup Fusion.
This contains GUI styles, 3-D effects, fonts, interface settings and so on.
Use Qt's own setting editor 'qtconfig', it's in "<Qt-Dir>/qt/bin/"!


### WBFS partitions/hdds

WiiBaFu needs root/admin permissions to access WBFS hdds/partitions!
If WiiBaFu don't work with your user permissions, start WiiBaFu with
'sudo WiiBaFu' or in admin mode!


### DVD drive

The dvd drive path is platform specific. For Linux use /dev/sr0 for the
first S-ATA drive (must be adapted to your drive), for Mac OS X use normally
/dev/disk2 (Mac with integrated hdd and dvd drive). On Windows, WIT use cygwin
to access the drive, the drive letters doesn't work! Use /dev/sr0 for the
first S-ATA drive (must be adapted to your drive).


### Languages

Wii Backup Fusion supports different languages. It uses the system locale to
switch the languages. For example, if your system is set to german (it is in
german language), Wii Backup Fusion will be switch to german language also.
