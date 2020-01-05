# File author is Ítalo Lima Marconato Matias
#
# Created on January 05 of 2020, at 16:51 BRT
# Last edited on January 05 of 2020, at 16:54 BRT

$(OUTPUT): $(PLATFORM_OBJECTS) $(OBJECTS)
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)
	$(NOECHO)$(TARGET)-gcc -shared -nostdlib -o $@ $(PLATFORM_OBJECTS) $(OBJECTS) $(PLATFORM_LDFLAGS) -lchicago
