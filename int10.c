#include "int10.h"

#include <stdint.h>

#include "cpu.h"
#include "event.h"
#include "x86_16b.h"

void txbe_int10h_handle() {
  uint8_t ah = txbe_x86_16_getah();

  struct txbe_event e;

  if (ah == 0x00) {
    uint8_t al = txbe_x86_16_getal();
    e.type = EVENT_SET_VIDEOMODE;
    e.set_videomode.mode = al;
    txbe_event_push(&e);

  } else if (ah == 0x01) {
    uint8_t bh = txbe_x86_16_getbh();
    uint8_t ch = txbe_x86_16_getch();
    uint8_t cl = txbe_x86_16_getcl();
    e.type = EVENT_SET_TEXTMODE_CURSOR;
    e.set_textmode_cursor.page = bh;
    e.set_textmode_cursor.row = ch;
    e.set_textmode_cursor.col = cl;
    txbe_event_push(&e);

  } else if (ah == 0x02) {
    uint8_t bh = txbe_x86_16_getbh();
    uint8_t dh = txbe_x86_16_getdh();
    uint8_t dl = txbe_x86_16_getdl();
    e.type = EVENT_SET_CURSOR_POS;
    e.set_cursor_pos.page = bh;
    e.set_cursor_pos.row = dh;
    e.set_cursor_pos.col = dl;
    txbe_event_push(&e);

  } else if (ah == 0x03) {
    uint8_t bh = txbe_x86_16_getbh();
    uint8_t dh, dl;
    e.type = EVENT_GET_CURSOR_POS;
    e.get_cursor_pos.page = bh;
    e.get_cursor_pos.row = &dh;
    e.get_cursor_pos.col = &dl;
    txbe_event_push(&e);
    cpu.dx = (dh << 8) | dl;

  } else if (ah == 0x04) {
    uint16_t cx, dx;
    e.type = EVENT_READ_LIGHTPEN_POS;
    e.read_lightpen.x = &cx;
    e.read_lightpen.y = &dx;
    txbe_event_push(&e);
    cpu.cx = cx;
    cpu.dx = dx;

  } else if (ah == 0x05) {
    uint8_t bh = txbe_x86_16_getbh();
    e.type = EVENT_SELECT_DISPLAYPAGE;
    e.select_displaypage.page = bh;
    txbe_event_push(&e);

  } else if (ah == 0x06) {
    uint8_t al = txbe_x86_16_getal();
    uint8_t bh = txbe_x86_16_getbh();
    uint16_t cx = cpu.cx;
    uint16_t dx = cpu.dx;
    e.type = EVENT_SCROLL_UP;
    e.scroll.lines = al;
    e.scroll.attribute = bh;
    e.scroll.start = cx;
    e.scroll.end = dx;
    txbe_event_push(&e);

  } else if (ah == 0x07) {
    uint8_t al = txbe_x86_16_getal();
    uint8_t bh = txbe_x86_16_getbh();
    uint16_t cx = cpu.cx;
    uint16_t dx = cpu.dx;
    e.type = EVENT_SCROLL_DOWN;
    e.scroll.lines = al;
    e.scroll.attribute = bh;
    e.scroll.start = cx;
    e.scroll.end = dx;
    txbe_event_push(&e);

  } else if (ah == 0x08) {
    uint8_t bh = txbe_x86_16_getbh();
    uint8_t al, ah_a;
    e.type = EVENT_READ_CHARACTER;
    e.read_character.page = bh;
    e.read_character.character = &al;
    e.read_character.attribute = &ah_a;
    txbe_event_push(&e);
    cpu.ax = (ah_a << 8) | al;

  } else if (ah == 0x09) {
    uint8_t al = txbe_x86_16_getal();
    uint8_t bl = txbe_x86_16_getbl();
    uint8_t bh = txbe_x86_16_getbh();
    uint16_t cx = cpu.cx;
    e.type = EVENT_WRITE_CHARACTER;
    e.write_character.character = al;
    e.write_character.attribute = bl;
    e.write_character.page = bh;
    e.write_character.count = cx;
    txbe_event_push(&e);

  } else if (ah == 0x0A) {
    uint8_t al = txbe_x86_16_getal();
    uint8_t bh = txbe_x86_16_getbh();
    uint16_t cx = cpu.cx;
    e.type = EVENT_WRITE_CHARACTER_ONLY;
    e.write_character_only.character = al;
    e.write_character_only.page = bh;
    e.write_character_only.count = cx;
    txbe_event_push(&e);

  } else if (ah == 0x0B) {
    uint8_t bh = txbe_x86_16_getbh();
    uint8_t bl = txbe_x86_16_getbl();
    e.type = EVENT_SET_PALETTE;
    e.set_palette.page = bh;
    e.set_palette.color = bl;
    txbe_event_push(&e);

  } else if (ah == 0x0C) {
    uint8_t dh = txbe_x86_16_getdh();
    uint8_t dl = txbe_x86_16_getdl();
    uint8_t al = txbe_x86_16_getal();
    uint8_t bh = txbe_x86_16_getbh();
    e.type = EVENT_WRITE_PIXEL;
    e.write_pixel.x = dh;
    e.write_pixel.y = dl;
    e.write_pixel.color = al;
    e.write_pixel.page = bh;
    txbe_event_push(&e);

  } else if (ah == 0x0D) {
    uint8_t dh = txbe_x86_16_getdh();
    uint8_t dl = txbe_x86_16_getdl();
    uint8_t al;
    e.type = EVENT_READ_PIXEL;
    e.read_pixel.x = dh;
    e.read_pixel.y = dl;
    e.read_pixel.color = &al;
    txbe_event_push(&e);
    cpu.ax = (cpu.ax & 0xFF00) | al;

  } else if (ah == 0x0E) {
    uint8_t al = txbe_x86_16_getal();
    uint8_t bh = txbe_x86_16_getbh();
    uint8_t bl = txbe_x86_16_getbl();
    e.type = EVENT_TELETYPE_OUTPUT;
    e.teletype_output.character = al;
    e.teletype_output.page = bh;
    e.teletype_output.attribute = bl;
    txbe_event_push(&e);

  } else if (ah == 0x0F) {
    uint8_t al, bh_o;
    uint8_t ah_o;
    e.type = EVENT_GET_VIDEOMODE;
    e.get_videomode.mode = &al;
    e.get_videomode.page = &bh_o;
    txbe_event_push(&e);
    cpu.ax = (ah_o << 8) | al;
    cpu.bx = (bh_o << 8) | (cpu.bx & 0x00FF);

  } else if (ah == 0x10) {
    uint8_t al = txbe_x86_16_getal();
    uint8_t bh = txbe_x86_16_getbh();
    uint8_t bl = txbe_x86_16_getbl();
    e.type = EVENT_VIDEO_SERVICES;
    e.video_services.function = ah;
    e.video_services.subfunction = al;
    e.video_services.page = bh;
    e.video_services.color = bl;
    txbe_event_push(&e);
  }
}