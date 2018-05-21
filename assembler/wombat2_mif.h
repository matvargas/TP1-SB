#ifndef WOMBAT2_MIF_H
  #define WOMBAT2_MIF_H

	typedef FILE file_t;

  // mif radix options
  typedef enum {BIN_RADIX = 0, HEX_RADIX, DEC_RADIX} wbt2_radix_t;

  // mif data structure:
  typedef struct {
    int depth;
    int width;
    int last_addr;
    wbt2_radix_t addr_radix;
    wbt2_radix_t data_radix;
    file_t *fstream;
  } wbt2_mif_t;

  // Create a mif (Memory Initialization File) file:
  wbt2_mif_t *wombat2_mif_create (const char *filepath, const char *filemode,
                                  int depth, int width, wbt2_radix_t addr_radix,
                                  wbt2_radix_t data_radix);

  // Write in a mif (Memory Initialization File) file:
  wbt2_mif_t *wombat2_mif_write(wbt2_mif_t *wbt2, int address, int data, char *comment);

  // Destroy a mif (Memory Initialization File) file:
  void wombat2_mif_destroy(wbt2_mif_t *wbt2);

  // Convert hexadecimal to Binary string to print:
  char *print_me(char *buff, int data);

#endif // WOMBAT2_MIF_H
