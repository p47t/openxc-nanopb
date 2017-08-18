#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "openxc.pb.h"

MODULE_LICENSE("GPL");

int init_module() {
    char* buf = kmalloc(128, GFP_KERNEL);
    size_t len;

    {
        pb_ostream_t stream = pb_ostream_from_buffer(buf, 128);

        openxc_CanMessage msg = openxc_CanMessage_init_default;
        msg.bus = 87; msg.has_bus = true;
        msg.id = 9487; msg.has_id = true;

        pb_encode(&stream, openxc_CanMessage_fields, &msg);
        len = stream.bytes_written;
    }
    {
        openxc_CanMessage msg = openxc_CanMessage_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(buf, len);

        pb_decode(&stream, openxc_CanMessage_fields, &msg);

        printk(KERN_INFO "bus = %d", msg.bus);
        printk(KERN_INFO "id = %d", msg.id);
    }

    return 0;
}

void cleanup_module() {
}
