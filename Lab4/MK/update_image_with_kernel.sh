#!/bin/bash
if [ $# -ne 2 ]
then
echo Usage: $0  hd.img src/kernel
exit 1
fi

# save these to chmod files back
var_user=laface
var_group=laface

losetup /dev/loop1 hd.img
fdisk -u -l /dev/loop1
blocks=$(fdisk -u -l /dev/loop1|tail -1|tr -s " "|cut -d " " -f 5|cut -d "+" -f 1)
echo $blocks
losetup -d /dev/loop1

losetup -o $(echo $((63*512))) --sizelimit  $(($blocks*1024)) /dev/loop0  $1


# mount /dev/loop0 to mount_point
mkdir -p  mount_point
mount /dev/loop0 mount_point 
mkdir -p mount_point/boot/grub

#
# cp kernel
cp -v $2  mount_point/boot/minimal_linux_like_kernel 
# umount mount_point
sleep 3
umount mount_point 
rm -r mount_point 

# detach /dev/loop0 
losetup -d /dev/loop0  

chown ${var_user}:${var_group} hd.img
