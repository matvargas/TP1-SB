#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <bitset>
#include <fstream>
#include <iostream>
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
  char bin_data[9] = {'\0'};
  std::fprintf(wbt2->fstream, "%02X : %s;\n", address, print_me(bin_data, data));
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
    std::fprintf(wbt2->fstream, "%02X : %d;\n", wbt2->last_addr++, UNUSED_DATA);
  }
  std::fprintf(wbt2->fstream, "END;");
  std::fclose(wbt2->fstream);
  std::free(wbt2);
}

//-----------------------------------------------------------------------------
// Convert hexadecimal to Binary string to print:
//-----------------------------------------------------------------------------
char *print_me(char *buff, int data) {
  
  for(int i = 0; i < 2; i++) {
    switch((data & 0xF0)>>4) {
              case 0x0: strcat(buff, "0000"); break;
              case 0x1: strcat(buff, "0001"); break;
              case 0x2: strcat(buff, "0010"); break;
              case 0x3: strcat(buff, "0011"); break;
              case 0x4: strcat(buff, "0100"); break;
              case 0x5: strcat(buff, "0101"); break;
              case 0x6: strcat(buff, "0110"); break;
              case 0x7: strcat(buff, "0111"); break;
              case 0x8: strcat(buff, "1000"); break;
              case 0x9: strcat(buff, "1001"); break;
              case 0xA: strcat(buff, "1010"); break;
              case 0xB: strcat(buff, "1011"); break;
              case 0xC: strcat(buff, "1100"); break;
              case 0xD: strcat(buff, "1101"); break;
              case 0xE: strcat(buff, "1110"); break;
              case 0xF: strcat(buff, "1111"); break;
              default: break;
          }
    data <<= 4;
  }
  return buff;
}