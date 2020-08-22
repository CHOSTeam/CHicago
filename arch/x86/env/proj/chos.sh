#!/bin/bash

after_build() {
	# Create the list of the files we are going to use to create the ISO, set the ISO
	# filename and check if we really need to create it
	
	local components=(
		"$ROOT_DIR/kernel/build/chkrnl-$FULL_ARCH-$([ \"$DEBUG\" == \"yes\" ] && printf dbg || printf rel)"
		"$ROOT_DIR/arch/$ARCH/build/osldr-$FULL_ARCH-$([ \"$DEBUG\" == \"yes\" ] && printf dbg || printf rel).efi"
	)
	
	local bname=$ROOT_DIR/chos/build/chicago-$FULL_ARCH-$([ "$DEBUG" == "yes" ] && printf dbg || printf rel).iso
	local bdir=$(dirname $bname)/$FULL_ARCH-$([ "$DEBUG" == "yes" ] && printf dbg || printf rel)
	local do=0
	
	if [ ! -f $bname ]; then
		do=1
	elif [ $ROOT_DIR/arch/$ARCH/env/proj/chos.sh -nt $bname ]; then
		do=1
	fi
	
	for i in "${components[@]}"; do
		if [ $do == 1 ]; then
			break
		elif [ -f $i ] && [ $i -nt $bname ]; then
			do=1
		fi
	done
	
	if [ $do == 0 ]; then
		return
	fi
	
	# Determine the architecture string (for the boot .efi file)
	
	if [ "$SUBARCH" == "32" ]; then
		local arch="IA32"
	else
		local arch="X64"
	fi
	
	# And create it
	
	printf "ISO: $(basename $bname)\n"
	
	mkdir -p $bdir/System/Boot $bdir
	dd if=/dev/zero of=$bdir/System/Boot/boot.img bs=1k count=2880 1>/dev/null 2>/dev/null
	mformat -i $bdir/System/Boot/boot.img -f 2880 ::
	mmd -i $bdir/System/Boot/boot.img ::/EFI
	mmd -i $bdir/System/Boot/boot.img ::/EFI/BOOT
	mmd -i $bdir/System/Boot/boot.img ::/EFI/CHOS
	mcopy -i $bdir/System/Boot/boot.img ${components[1]} ::/EFI/BOOT/BOOT$arch.EFI
	
	# Create the kernel/initrd SIA file
	
	mkdir -p $bdir/initrd
	sia-create $bdir/BOOT.SIA ${components[0]} $bdir/initrd
	mcopy -i $bdir/System/Boot/boot.img $bdir/BOOT.SIA ::/EFI/CHOS/BOOT.SIA
	rm -rf $bdir/initrd $bdir/BOOT.SIA
	
	xorriso -as mkisofs -R -c System/Boot/boot.cat -e System/Boot/boot.img -U -no-emul-boot -o $bname $bdir 1>/dev/null 2>/dev/null
	rm -rf $bdir
	
	return 0
}
