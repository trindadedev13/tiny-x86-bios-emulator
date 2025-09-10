#ifndef __txbe_event_h__
#define __txbe_event_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

  enum txbe_event_type
  {
    EVENT_START,
    EVENT_END,
    EVENT_UNKNOWN_OPCODE,
    EVENT_SET_VIDEOMODE,
    EVENT_SET_TEXTMODE_CURSOR,
    EVENT_SET_CURSOR_POS,
    EVENT_GET_CURSOR_POS,
    EVENT_READ_LIGHTPEN_POS,
    EVENT_SELECT_DISPLAYPAGE,
    EVENT_SCROLL_UP,
    EVENT_SCROLL_DOWN,
    EVENT_READ_CHARACTER,
    EVENT_WRITE_CHARACTER,
    EVENT_WRITE_CHARACTER_ONLY,
    EVENT_SET_PALETTE,
    EVENT_WRITE_PIXEL,
    EVENT_READ_PIXEL,
    EVENT_TELETYPE_OUTPUT,
    EVENT_GET_VIDEOMODE,
    EVENT_VIDEO_SERVICES
  };

  struct txbe_event
  {
    enum txbe_event_type type;
    union
    {
      struct
      {
        uint8_t opcode;
      } unknown_opcode;

      struct
      {
        uint8_t mode; // AL
      } set_videomode;

      struct
      {
        uint8_t page; // BH
        uint8_t row;  // CH
        uint8_t col;  // CL
      } set_textmode_cursor;

      struct
      {
        uint8_t page; // BH
        uint8_t row;  // DH
        uint8_t col;  // DL
      } set_cursor_pos;

      struct
      {
        uint8_t page; // BH
        uint8_t *row; // DH
        uint8_t *col; // DL
      } get_cursor_pos;

      struct
      {
        uint16_t *x; // CX
        uint16_t *y; // DX
      } read_lightpen;

      struct
      {
        uint8_t page; // BH
      } select_displaypage;

      struct
      {
        uint8_t lines;     // Number of lines
        uint8_t attribute; // Attribute
        uint16_t start;    // CX
        uint16_t end;      // DX
      } scroll;

      struct
      {
        uint8_t page;       // BH
        uint8_t *character; // AL
        uint8_t *attribute; // AH
      } read_character;

      struct
      {
        uint8_t character; // AL
        uint8_t attribute; // AH
        uint8_t page;      // BH
        uint16_t count;    // CX
      } write_character;

      struct
      {
        uint8_t character; // AL
        uint8_t page;      // BH
        uint16_t count;    // CX
      } write_character_only;

      struct
      {
        uint8_t page;  // BH
        uint8_t color; // BL
      } set_palette;

      struct
      {
        uint8_t x;     // DH
        uint8_t y;     // DL
        uint8_t color; // AL
        uint8_t page;  // BH
      } write_pixel;

      struct
      {
        uint8_t x;      // DH
        uint8_t y;      // DL
        uint8_t *color; // AL
      } read_pixel;

      struct
      {
        uint8_t character; // AL
        uint8_t page;      // BH
        uint8_t attribute; // AH
      } teletype_output;

      struct
      {
        uint8_t *mode; // AL
        uint8_t *page; // BH
      } get_videomode;

      struct
      {
        uint8_t function;    // AH
        uint8_t subfunction; // AL
        uint8_t page;        // BH
        uint8_t color;       // BL
      } video_services;
    };
  };

#define EVENT_QUEUE_SIZE 128

#ifdef __cplusplus
#define EVENT_QUEUE_PROCESS_FUNC()                                            \
  extern "C" void txbe_backend_event_process_queue ()
#else
#define EVENT_QUEUE_PROCESS_FUNC() void txbe_backend_event_process_queue ()
#endif

  extern void txbe_backend_event_process_queue ();

  void txbe_event_push (struct txbe_event *);
  int txbe_event_pool (struct txbe_event *);

#ifdef __cplusplus
}
#endif

#endif