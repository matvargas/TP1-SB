#ifndef WOMBAT2AS_H
  #define WOMBAT2AS_H

  // Assembler pass one:
  void pass_one (symbol_dict_t *symbol_tbl, opcode_tbl_t *opcode_tbl, 
                 pseudo_inst_dict_t *pseudo_inst_tbl, 
                 const char *file_path, file_t *file_in);

  // Assembler pass two:
  void pass_two (symbol_dict_t *symbol_tbl, opcode_tbl_t *opcode_tb, 
                 sfr_tbl_t *sfr_tbl, pseudo_inst_dict_t *pseudo_inst_tbl, 
                 const char *fname_in, file_t *file_in, const char *fname_out);

#endif // WOMBAT2AS_H
