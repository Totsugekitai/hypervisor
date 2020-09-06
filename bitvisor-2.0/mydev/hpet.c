#include "../core/mm.h"
#include "../include/share/vmm_types.h"
#include "../core/spinlock.h"
#include "../core/mmio.h"
#include "../core/printf.h"
#include "../core/current.h"
#include "../core/gmm_access.h"
#include "../core/initfunc.h"

union hpet_gcidr {
    struct {
        unsigned int rev_id : 8;
        unsigned int num_tim_cap : 5;
        unsigned int count_size_cap : 1;
        unsigned int reserved : 1;
        unsigned int leg_route_cap : 1;
        unsigned int vender_id : 16;
    } s;
    u32 v;
} typedef hpet_gcidr;

static int
hpet_mm_handler (void *data, phys_t gphys, bool wr, void *buf, uint len, u32 flags)
{
    unsigned int ret_data;
    phys_t hphys;
    hpet_gcidr gcidr;
    printf("guest's physical address is 0x%016x\n", (unsigned int)gphys);
    hphys = current->gmm.gp2hp(gphys, NULL);
    printf("VMM's physical address is 0x%016x\n", (unsigned int)hphys);
    printf("I/O type: %s\n", (wr) ? "WRITE" : "READ");
    printf("Length: %d\n", len);
    printf("Flag: 0x%016x\n", flags);

    if (buf == NULL) {
        printf("buf is NULL pointer\n");
        return 0;
    }

    if (!wr) {
        if (len == 1) {
            read_hphys_b(hphys, &ret_data, flags);
            *((unsigned char *)buf) = (unsigned char)ret_data;
        } else if (len == 2) {
            read_hphys_w(hphys, &ret_data, flags);
            *((unsigned short *)buf) = (unsigned short)ret_data;
        } else if (len == 4) {
            read_hphys_l(hphys, &ret_data, flags);
            printf("[by totsugeki] Read data is 0x%08x\n", ret_data);
            gcidr.v = (u32)ret_data;
            printf("[by totsugeki] number of Timer is 0x%d\n", gcidr.s.num_tim_cap);
            gcidr.s.num_tim_cap--;
            *((unsigned int *)buf) = (unsigned int)gcidr.v;
            printf("[by totsugeki] return data is 0x%08x\n", (unsigned int)gcidr.v);
        } else if (len == 8) {
            read_hphys_q(hphys, &ret_data, flags);
            *((unsigned long int *)buf) = (unsigned long int)ret_data;
        }
        return 1;
    } else {
        printf("[by totsgueki] debug print : I/O type is WRITE\n");
        return 0;
    }
}

void
reg_hpet_mm_handler(void)
{
    mmio_register(0xFED00000, 0x4, hpet_mm_handler, NULL);
}
