# File author is Ítalo Lima Marconato Matias
#
# Created on December 20 of 2018, at 10:49 BRT
# Last edited on November 17 of 2019, at 10:15 BRT

arch-clean:
	$(NOECHO)rm -f build/chicago-$(ARCH)_$(SUBARCH).iso

finish: kernel/build/chkrnl-$(ARCH)_$(SUBARCH)
	$(NOECHO)echo Generating build/chicago-$(ARCH)_$(SUBARCH).iso
	$(NOECHO)if [ ! -d build ]; then mkdir -p build; fi
	$(NOECHO)if [ -d build/iso ]; then rm -rf build/iso; fi
	$(NOECHO)cp -RT toolchain/$(ARCH)-$(SUBARCH)/sysroot build/iso
	$(NOECHO)echo -e '\xFF\xFE' > build/iso/System/Boot/bootmgr.conf
	$(NOECHO)echo 'Language=$(LANGUAGE)' >> build/iso/System/Configurations/System.conf
	$(NOECHO)echo '"Test Driver"=test.chd' >> build/iso/System/Configurations/Drivers.conf
	$(NOECHO)cp kernel/build/chkrnl-$(ARCH)_$(SUBARCH) build/iso/System/Boot/chkrnl.elf
	$(NOECHO)dd if=/dev/zero of=build/iso/System/Boot/bootmgr.img bs=1k count=2880 2>/dev/null
	$(NOECHO)mformat -i build/iso/System/Boot/bootmgr.img -f 2880 ::
	$(NOECHO)mmd -i build/iso/System/Boot/bootmgr.img ::/EFI
	$(NOECHO)mmd -i build/iso/System/Boot/bootmgr.img ::/EFI/BOOT
ifeq ($(SUBARCH),64)
	$(NOECHO)mcopy -i build/iso/System/Boot/bootmgr.img arch/$(ARCH)/build/bootmgr$(SUBARCH) ::/EFI/BOOT/BOOTX64.EFI
else
	$(NOECHO)mcopy -i build/iso/System/Boot/bootmgr.img arch/$(ARCH)/build/bootmgr$(SUBARCH) ::/EFI/BOOT/BOOTIA32.EFI
endif
	$(NOECHO)mcopy -i build/iso/System/Boot/bootmgr.img kernel/build/chkrnl-$(ARCH)_$(SUBARCH) ::/EFI/BOOT/chkrnl.elf
	$(NOECHO)mcopy -i build/iso/System/Boot/bootmgr.img build/iso/System/Boot/bootmgr.conf ::/EFI/BOOT/bootmgr.conf
	$(NOECHO)rm build/iso/System/Boot/bootmgr.conf
	$(NOECHO)xorriso -as mkisofs -R -c System/Boot/boot.cat -e System/Boot/bootmgr.img -U -no-emul-boot -o build/chicago-$(ARCH)_$(SUBARCH).iso build/iso 2>/dev/null
	$(NOECHO)rm -rf build/iso
