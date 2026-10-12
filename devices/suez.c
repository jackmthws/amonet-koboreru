#include <arb.h>
#include <atf.h>
#include <bootmode.h>
#include <device.h>
#include <mmio.h>
#include <patch.h>
#include <preloader.h>
#include <tee.h>

void apply_patches(void)
{
    // Ensure we can load unsigned DAs
    patch_ret(0x000d9080, 0); // sec_auth
    patch_ret(0x000d90c4, 0); // da_auth_init

    // Do not override USB descriptors with the originals
    patch_word(0x000dc764, 0xBF00447A);

    // Replace the ARB check with a routine that clears the counters
    patch_branch(0x000c1934, clear_rpmb_arb);

    // Replace the TEE image loader with our own
    patch_branch(0x000cee38, bldr_load_tee_part);

    // Hand a mainline BL31 the boot information it expects
    patch_branch(0x000cf224, bldr_jump64);
}

// TODO: suez kpd
uint8_t usbdl_detect_key(void)
{
    if (readl(G_BOOT_MODE_ADDR) != FACTORY_BOOT)
        return 0;

    writel(NORMAL_BOOT, G_BOOT_MODE_ADDR);
    return 1;
}
