#ifndef SWOMBAT3_MIF_H
  #define SWOMBAT3_MIF_H

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
  } swbt3_mif_t;

  // Create a mif (Memory Initialization File) file:
  swbt3_mif_t *swombat3_mif_create (const char *filepath, const char *filemode,
                                  int depth, int width, wbt2_radix_t addr_radix,
                                  wbt2_radix_t data_radix);

  // Write in a mif (Memory Initialization File) file:
  swbt3_mif_t *swombat3_mif_write(swbt3_mif_t *wbt2, int address, int data, char *comment);

  // Destroy a mif (Memory Initialization File) file:
  void swombat3_mif_destroy(swbt3_mif_t *wbt2);

  // Convert hexadecimal to Binary string to print:
  char *print_me(char *buff, int data);

#endif // SWOMBAT3_MIF_H
