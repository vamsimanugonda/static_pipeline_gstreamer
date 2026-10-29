#include <gst/gst.h>

int main(int argc, char* argv[]) 
{
    GstElement* pipeline, * source, * filter, * sink;
    GstBus* bus;
    GstMessage* msg;

    /* 1. Initialize GStreamer */
    gst_init(&argc, &argv);

    /* 2. Create pipeline elements */
    source = gst_element_factory_make("videotestsrc", "source");
    filter = gst_element_factory_make("videoconvert", "filter");
    sink = gst_element_factory_make("autovideosink", "sink");

    /* Create the empty pipeline container */
    pipeline = gst_pipeline_new("test-pipeline");

    if (!pipeline || !source || !filter || !sink) 
    {
        g_printerr("Failed to create all elements.\n");
        return -1;
    }

    /* 3. Configure element properties */
    g_object_set(source, "pattern", 1, NULL); /* 0 = SMPTE color bars */

    /* 4. Add elements to pipeline bin and link them */
    gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);
    if (!gst_element_link_many(source, filter, sink, NULL)) 
    {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* 5. Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* 6. Listen to the bus for EOS (End of Stream) or Error */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* 7. Handle bus messages */
    if (msg != NULL) 
    {
        GError* err;
        gchar* debug_info;

        switch (GST_MESSAGE_TYPE(msg)) 
        {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error: %s\nDebug info: %s\n", err->message, debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            break;
        }
        gst_message_unref(msg);
    }

    /* 8. Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}