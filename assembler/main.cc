#include <cctype>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include "wombat2_parser.h"
#include "wombat2as.h"

//-----------------------------------------------------------------------------
// main function:
//-----------------------------------------------------------------------------
int main (int argc, char **argv) {
	try {
    char *fname_in, *fname_out;

		opcode_tbl_t       *opcode_tbl      = opcode_tbl_create();       	// opcode dict creation
		sfr_tbl_t          *sfr_tbl         = sfr_tbl_create();          	// special function registers dict creation
		symbol_dict_t      *symbol_tbl      = symbol_tbl_create();				// symbols dict creation
		pseudo_inst_dict_t *pseudo_inst_tbl = pseudo_inst_tbl_create();		// pseudo-instr dict creation

    if(argc < 2) {            // an input file must be provided
			std::fprintf(stderr, "exiting...\nerror: check again the argument line!!!\n"
                           "  format: $./mont <in-file.a> <out-file.mif>\n");
			std::exit(EXIT_FAILURE);
    }else if(argc == 2) {     // only input file is given, output file is: a.mif
      fname_out = (char *)("a.mif");
    }else {                   // input and output file are given
      fname_out = argv[2];
      int len = std::strlen(fname_out);
      char *mif_extension = (char *)".mif";
      int len_ext = std::strlen(mif_extension);
      if(len > len_ext) {
        if(0 != std::strcmp(&fname_out[len - len_ext], mif_extension)) goto L1_EXIT;
      }else {
      L1_EXIT:
			  std::fprintf(stderr, "exiting...\nerror: check again the argument line!!!\n"
                             "  incorrect output file extension should be <out-file.mif>\n");
			  std::exit(EXIT_FAILURE);
      }
    }

    fname_in = argv[1];
		file_t *file_in = fopen(fname_in, "r");
    if(file_in == NULL) {
			std::fprintf(stderr, "exiting...\nerror: Unable to read input file: %s\n", fname_in);
			std::exit(EXIT_FAILURE);
    }

		pass_one(symbol_tbl, opcode_tbl, pseudo_inst_tbl, fname_in, file_in);
		pass_two(symbol_tbl, opcode_tbl, sfr_tbl, pseudo_inst_tbl, fname_in, file_in, fname_out);

		std::fclose(file_in);

		pseudo_inst_tbl_destroy(pseudo_inst_tbl);
		symbol_tbl_destroy(symbol_tbl);
		sfr_tbl_destroy(sfr_tbl);
		opcode_tbl_destroy(opcode_tbl);

	}catch(...) { // TODO: A not so naive capture exception must be provide in the future!!!
		std::fprintf(stderr, "exiting...\nerror: god fucking dammit, a really nasty thing occurred!!!\n");
		std::exit(EXIT_FAILURE);
	}

	std::exit(EXIT_SUCCESS);
}
