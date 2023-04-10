#include <stdio.h>
#include <stdlib.h>

#include <dbus/dbus.h>
#include <X11/Xlib.h>

#define giveup(...) \
{ \
    fprintf(stderr, __VA_ARGS__); \
    exit(1); \
}

static DBusConnection *conn;
static DBusError err;

// I don't like magic numbers.
#define TABLE_SIZE 8

// Button 4 = scroll up
// Button 5 = scroll down
static int button_table[TABLE_SIZE] = {
    4,
    4,
    4,
    4,
    5,
    5,
    5,
    5,
};

// Mod1: Assumed to be (left or right) control.
// Mod2: Num lock. Both sets here so this works regardless of num lock state.
// LockMask: Caps lock, for the same reason.
static int mod_table[TABLE_SIZE] = {
    Mod1Mask,
    Mod1Mask | LockMask,
    Mod1Mask | LockMask | Mod2Mask,
    Mod1Mask | Mod2Mask,
    Mod1Mask,
    Mod1Mask | LockMask,
    Mod1Mask | LockMask | Mod2Mask,
    Mod1Mask | Mod2Mask,
};

static const char *action_table[TABLE_SIZE] = {
    "view_zoom_in",
    "view_zoom_in",
    "view_zoom_in",
    "view_zoom_in",
    "view_zoom_out",
    "view_zoom_out",
    "view_zoom_out",
    "view_zoom_out",
};

static void init_dbus(void)
{
    dbus_error_init(&err);

    // connect to the system bus and check for errors
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

    if (dbus_error_is_set(&err))
        giveup("Can't connect to dbus: %s\n", err.message);

    // request our name on the bus
    int reply = dbus_bus_request_name(conn, "box.plasma.zoom", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);

    if (dbus_error_is_set(&err))
        giveup("Can't get a name: %s\n", err.message);

    if (reply != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        giveup("Error: Huh? This program's name is already in use on the session bus.\n")
}

static void dbus_call(const char *param)
{
    DBusMessage *msg;
    DBusMessageIter args;
    DBusPendingCall *pending;
    Bool stat;
    dbus_uint32_t level;

    msg = dbus_message_new_method_call(
        "org.kde.kglobalaccel",
        "/component/kwin",
        "org.kde.kglobalaccel.Component",
        "invokeShortcut"
    );

    if (msg == NULL)
        giveup("Can't create a message.\n");

    dbus_message_iter_init_append(msg, &args);

    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param))
        giveup("No memory.\n");

    if (!dbus_connection_send_with_reply (conn, msg, &pending, -1))
        giveup("No memory.\n");

    if (pending == NULL)
        giveup("Can't call shortcut.\n");

    // Make sure this message sends before selecting for more input.
    dbus_connection_flush(conn);
    dbus_message_unref(msg);
    dbus_pending_call_block(pending);
    dbus_pending_call_unref(pending);
}

static int on_x_error(Display *d, XErrorEvent *e)
{
    giveup("Error: Another program has grabbed the keys this one needs.\n");
    return 0;
}

int main(int argc, char **argv)
{
    Display *d = XOpenDisplay(NULL);

    if (d == NULL)
        giveup("Can't open X display.\n");

    init_dbus();
    XAllowEvents(d, AsyncBoth, CurrentTime);

    int screen;

    for (screen = 0;screen < ScreenCount(d);screen++) {
        Window root = RootWindow(d, screen);
        int i;

        XSelectInput(d, root, KeyPressMask);

        for (i = 0; i < TABLE_SIZE; i++)
            XGrabButton(d, button_table[i], mod_table[i], root,
                        False, ButtonPressMask,
                        GrabModeAsync, GrabModeAsync, None, None);
    }

    XSetErrorHandler((XErrorHandler) on_x_error);

    for (;;) {
        XEvent e;
        int i;

        XNextEvent(d, &e);

        if (e.type != ButtonPress)
            continue;

        for (i = 0; i < TABLE_SIZE; i++) {
            if (e.xbutton.button == button_table[i] &&
                e.xbutton.state == mod_table[i]) {
                dbus_call(action_table[i]);
            }
        }
    }

    // Unreachable since the select loop never returns.
    return EXIT_SUCCESS;
}
