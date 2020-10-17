#!/bin/bash

after_build() {
	# Determine the architecture string (for the boot .efi file)
	
	if [ "$SUBARCH" == "32" ]; then
		local arch="IA32"
	else
		local arch="X64"
	fi
	
	local bname=$ROOT_DIR/chos/build/chicago-$FULL_ARCH-$([ "$DEBUG" == "yes" ] && printf dbg || printf rel).iso
	local bdir=$(dirname $bname)/$FULL_ARCH-$([ "$DEBUG" == "yes" ] && printf dbg || printf rel)
	
	# Create the list of the files we are going to use to create the ISO, set the ISO filename and check if we really need to create it
	
	local bdrvssrc=( "$ROOT_DIR/drivers/test/build/test-drv-$FULL_ARCH-$([ \"$DEBUG\" == \"yes\" ] && printf dbg || printf rel).chd" )
	local odrvssrc=()
	local componentssrc=(
		"$ROOT_DIR/kernel/build/chkrnl-$FULL_ARCH-$([ \"$DEBUG\" == \"yes\" ] && printf dbg || printf rel)"
		"$ROOT_DIR/arch/$ARCH/build/osldr-$FULL_ARCH-$([ \"$DEBUG\" == \"yes\" ] && printf dbg || printf rel).efi"
		${bdrvssrc[@]}
		${odrvssrc[@]}
	)
	
	local bdrvsdst=( "$bdir/initrd/Drivers/test.chd" )
	local odrvsdst=()
	local componentsdst=(
		"The kernel doesn't have any path, so this first field is pretty much useless"
		"::/EFI/BOOT/BOOT$arch.EFI"
		${bdrvssrc[@]}
		${odrvssrc[@]}
	)
	
	local do=0
	
	if [ ! -f $bname ] || [ $ROOT_DIR/arch/$ARCH/env/proj/chos.sh -nt $bname ]; then
		do=1
	fi
	
	for i in "${componentssrc[@]}"; do
		if [ $do == 1 ]; then
			break
		elif [ -f $i ] && [ $i -nt $bname ]; then
			do=1
		fi
	done
	
	if [ $do == 0 ]; then
		return
	fi
	
	# And create it
	
	printf "ISO: $(basename $bname)\n"
	
	mkdir -p $bdir/System/Boot
	mkdir -p $bdir/initrd
	
	for (( i=0; i<${#bdrvssrc[@]}; i++ )); do
		mkdir -p $(dirname ${bdrvsdst[$i]})
		cp ${bdrvssrc[$i]} ${bdrvsdst[$i]}
	done
	
	for (( i=0; i<${#odrvssrc[@]}; i++ )); do
		mkdir -p $(dirname ${odrvsdst[$i]})
		cp ${odrvssrc[$i]} ${odrvsdst[$i]}
	done
	
	sia-create $bdir/BOOT.SIA ${componentssrc[0]} $bdir/initrd
	rm -rf $bdir/initrd
	
	dd if=/dev/zero of=$bdir/System/Boot/boot.img bs=1k count=2880 1>/dev/null 2>/dev/null
	mformat -i $bdir/System/Boot/boot.img -f 2880 ::
	mmd -i $bdir/System/Boot/boot.img ::/EFI
	mmd -i $bdir/System/Boot/boot.img ::/EFI/BOOT
	mmd -i $bdir/System/Boot/boot.img ::/EFI/CHOS
	mcopy -i $bdir/System/Boot/boot.img ${componentssrc[1]} ${componentsdst[1]}
	mcopy -i $bdir/System/Boot/boot.img $bdir/BOOT.SIA ::/EFI/CHOS/BOOT.SIA
	xorriso -as mkisofs -R -c System/Boot/boot.cat -e System/Boot/boot.img -U -no-emul-boot -o $bname $bdir 1>/dev/null 2>/dev/null
	rm -rf $bdir
	
	return 0
}
