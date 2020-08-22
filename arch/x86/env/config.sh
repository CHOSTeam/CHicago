#!/bin/bash

_list_config() {
	printf "SUBARCH\t\t- Set if the destination architecture is x86 or amd64 (32 = x86, 64 = amd64)\n"
}

_get_config() {
	printf "Invalid configuration '$1'"
}

_set_config() {
	printf "Invalid configuration '$1'.\n"
}
