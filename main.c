#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "openxc.pb.h"
#include "union.pb.h"

MODULE_LICENSE("GPL");

void test_openxc(void)
{
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

    kfree(buf);
}

const pb_field_t* decode_unionmessage_type(pb_istream_t *stream)
{
    pb_wire_type_t wire_type;
    uint32_t tag;
    bool eof;

    while (pb_decode_tag(stream, &wire_type, &tag, &eof)) {
        if (wire_type == PB_WT_STRING) {
            const pb_field_t *field;
            for (field = UnionMessage_fields; field->tag != 0; field++) {
                if (field->tag == tag && (field->type & PB_LTYPE_SUBMESSAGE)) {
                    /* Found our field. */
                    return field->ptr;
                }
            }
        }

        /* Wasn't our field.. */
        pb_skip_field(stream, wire_type);
    }

    return NULL;
}

bool decode_unionmessage_contents(pb_istream_t *stream, const pb_field_t fields[], void *dest_struct)
{
    pb_istream_t substream;
    bool status;
    if (!pb_make_string_substream(stream, &substream))
        return false;

    status = pb_decode(&substream, fields, dest_struct);
    pb_close_string_substream(stream, &substream);
    return status;
}

bool encode_unionmessage(pb_ostream_t *stream, const pb_field_t messagetype[], const void *message)
{
    const pb_field_t *field;
    for (field = UnionMessage_fields; field->tag != 0; field++) {
        if (field->ptr == messagetype) {
            /* This is our field, encode the message using it. */
            if (!pb_encode_tag_for_field(stream, field))
                return false;

            return pb_encode_submessage(stream, messagetype, message);
        }
    }

    /* Didn't find the field for messagetype */
    return false;
}

int test_union(void)
{
    uint8_t buffer[128];
    size_t len;

    {
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	MsgType1 msg1 = {42};
        encode_unionmessage(&stream, MsgType1_fields, &msg1);
	MsgType2 msg2 = {true};
        encode_unionmessage(&stream, MsgType2_fields, &msg2);
	MsgType3 msg3 = {3, 1415};
        encode_unionmessage(&stream, MsgType3_fields, &msg3);
	len = stream.bytes_written;
    }

    {
	pb_istream_t stream = pb_istream_from_buffer(buffer, len);

	const pb_field_t *type;
	while ((type = decode_unionmessage_type(&stream)) != NULL) {
            bool status = false;

            if (type == MsgType1_fields) {
                MsgType1 msg = {};
                status = decode_unionmessage_contents(&stream, MsgType1_fields, &msg);
                printk(KERN_INFO "Got MsgType1: %d\n", msg.value);
            }
            else if (type == MsgType2_fields) {
                MsgType2 msg = {};
                status = decode_unionmessage_contents(&stream, MsgType2_fields, &msg);
                printk(KERN_INFO "Got MsgType2: %s\n", msg.value ? "true" : "false");
            }
            else if (type == MsgType3_fields) {
                MsgType3 msg = {};
                status = decode_unionmessage_contents(&stream, MsgType3_fields, &msg);
                printk(KERN_INFO "Got MsgType3: %d %d\n", msg.value1, msg.value2);
            }
            if (!status) {
                printk(KERN_ERR "Decode failed: %s\n", PB_GET_ERROR(&stream));
                return 1;
            }
        }
    }

    return 0;
}

int init_module() {
    test_openxc();
    test_union();

    return 0;
}

void cleanup_module() {
}
