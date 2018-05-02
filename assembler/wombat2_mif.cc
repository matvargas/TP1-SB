#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "wombat2_mif.h"

//-----------------------------------------------------------------------------
// Create a mif (Memory Initialization File) file:
//-----------------------------------------------------------------------------
wbt2_mif_t *wombat2_mif_create (const char *filepath, const char *filemode, 
                                int depth, int width, wbt2_radix_t addr_radix,
                                wbt2_radix_t data_radix) {
  // TODO: check allocation and file open!
  wbt2_mif_t *wbt2 = (wbt2_mif_t *)malloc(sizeof(wbt2_mif_t));
  wbt2->fstream    = std::fopen(filepath, filemode);
  wbt2->depth      = depth;
  wbt2->width      = width;
  wbt2->last_addr  = 0;
  wbt2->addr_radix = addr_radix;
  wbt2->data_radix = data_radix;
  std::fprintf(wbt2->fstream, "DEPTH = %i;\nWIDTH = %i;\nADDRESS_RADIX = %s;\nDATA_RADIX = %s;\n"
               "CONTENT\nBEGIN\n\n", wbt2->depth, wbt2->width, 
               wbt2->addr_radix == BIN_RADIX ? "BIN" : wbt2->addr_radix == HEX_RADIX ? "HEX" : "DEC",
               wbt2->data_radix == BIN_RADIX ? "BIN" : wbt2->data_radix == HEX_RADIX ? "HEX" : "DEC");
  return wbt2;
}

//-----------------------------------------------------------------------------
// Write in a mif (Memory Initialization File) file:
//-----------------------------------------------------------------------------
wbt2_mif_t *wombat2_mif_write(wbt2_mif_t *wbt2, int address, int data, char *comment) {
  // TODO: Put assembly program comment line into the output file!
  comment = comment; // makes compiler happy!
  std::fprintf(wbt2->fstream, "%02X : %02X;\n", address, data);
  wbt2->last_addr = address + 1;
  return NULL;
}

//-----------------------------------------------------------------------------
// Destroy a mif (Memory Initialization File) file:
//-----------------------------------------------------------------------------
void wombat2_mif_destroy(wbt2_mif_t *wbt2) {
  // Fill not used memory with constant:
  const unsigned char UNUSED_DATA = 0x00;
  while(wbt2->last_addr < wbt2->depth) {
    std::fprintf(wbt2->fstream, "%02X : %02X;\n", wbt2->last_addr++, UNUSED_DATA);
  }
  std::fprintf(wbt2->fstream, "END;");
  std::fclose(wbt2->fstream);
  std::free(wbt2);
}

