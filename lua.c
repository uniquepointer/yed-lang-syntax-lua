#include <yed/plugin.h>
#include <yed/highlight.h>

#define ARRAY_LOOP(a) for (__typeof((a)[0]) *it = (a); it < (a) + (sizeof(a) / sizeof((a)[0])); ++it)

highlight_info hinfo;

void
unload(yed_plugin* self);
void
syntax_c_line_handler(yed_event* event);
void
syntax_c_frame_handler(yed_event* event);
void
syntax_c_buff_mod_pre_handler(yed_event* event);
void
syntax_c_buff_mod_post_handler(yed_event* event);


int
yed_plugin_boot(yed_plugin* self)
{
    yed_event_handler frame, line, buff_mod_pre, buff_mod_post;
    char*              kwds[] =
    {
        "end",
        "local",
        "in",

    };
    char*              pp_kwds[] =
    {
        "not",
        "or",
        "and",

    };
    char*              control_flow[] =
    {
        "break",
        "do",
        "else",
        "elseif",
        "until",
        "while",
        "for",
        "if",
        "repeat",
        "return",
        "then",

    };
    char*              typenames[] =
    {
        "nil",
        "boolean",
        "number",
        "string",
        "function",
        "userdata",
        "thread",
        "table",

    };

    YED_PLUG_VERSION_CHECK();

    yed_plugin_set_unload_fn(self, unload);


    frame.kind          = EVENT_FRAME_PRE_BUFF_DRAW;
    frame.fn            = syntax_c_frame_handler;
    line.kind           = EVENT_LINE_PRE_DRAW;
    line.fn             = syntax_c_line_handler;
    buff_mod_pre.kind   = EVENT_BUFFER_PRE_MOD;
    buff_mod_pre.fn     = syntax_c_buff_mod_pre_handler;
    buff_mod_post.kind  = EVENT_BUFFER_POST_MOD;
    buff_mod_post.fn    = syntax_c_buff_mod_post_handler;

    yed_plugin_add_event_handler(self, frame);
    yed_plugin_add_event_handler(self, line);
    yed_plugin_add_event_handler(self, buff_mod_pre);
    yed_plugin_add_event_handler(self, buff_mod_post);


    highlight_info_make(&hinfo);

    ARRAY_LOOP(kwds)
    highlight_add_kwd(&hinfo, *it, HL_KEY);
    ARRAY_LOOP(pp_kwds)
    highlight_add_prefixed_kwd(&hinfo, '#', *it, HL_PP);
    ARRAY_LOOP(control_flow)
    highlight_add_kwd(&hinfo, *it, HL_CF);
    ARRAY_LOOP(typenames)
    highlight_add_kwd(&hinfo, *it, HL_TYPE);
    highlight_add_kwd(&hinfo, "true", HL_CON);
    highlight_add_kwd(&hinfo, "false", HL_CON);
    highlight_suffixed_words(&hinfo, '(', HL_CALL);
    highlight_numbers(&hinfo);
    highlight_within(&hinfo, "\"", "\"", '\\', -1, HL_STR);
    highlight_within(&hinfo, "'", "'", '\\', 1, HL_CHAR);
    highlight_to_eol_from(&hinfo, "--", HL_COMMENT);

    ys->redraw = 1;

    return 0;
}

void
unload(yed_plugin* self)
{
    highlight_info_free(&hinfo);
    ys->redraw = 1;
}

void
syntax_c_frame_handler(yed_event* event)
{
    yed_frame* frame;

    frame = event->frame;

    if (!frame
            ||  !frame->buffer
            ||  frame->buffer->kind != BUFF_KIND_FILE
            ||  frame->buffer->ft != yed_get_ft("Lua"))
    {
        return;
    }

    highlight_frame_pre_draw_update(&hinfo, event);
}

void
syntax_c_line_handler(yed_event* event)
{
    yed_frame* frame;

    frame = event->frame;

    if (!frame
            ||  !frame->buffer
            ||  frame->buffer->kind != BUFF_KIND_FILE
            ||  frame->buffer->ft != yed_get_ft("Lua"))
    {
        return;
    }

    highlight_line(&hinfo, event);
}

void
syntax_c_buff_mod_pre_handler(yed_event* event)
{
    if (event->buffer == NULL
            ||  event->buffer->kind != BUFF_KIND_FILE
            ||  event->buffer->ft != yed_get_ft("Lua"))
    {
        return;
    }

    highlight_buffer_pre_mod_update(&hinfo, event);
}

void
syntax_c_buff_mod_post_handler(yed_event* event)
{
    if (event->buffer == NULL
            ||  event->buffer->kind != BUFF_KIND_FILE
            ||  event->buffer->ft != yed_get_ft("Lua"))
    {
        return;
    }

    highlight_buffer_post_mod_update(&hinfo, event);
}
