#include "wombat2_parser.h"
#include "wombat2as.h"
#include "wombat2_mif.h"

//-----------------------------------------------------------------------------
// Checks whether it is a valid SFR (Special Function Register):
//-----------------------------------------------------------------------------
bool check_if_valid_sfr (sfr_tbl_t *sfr_tbl, unsigned *num, 
                         char *str_op, unsigned displacement) {
  int aux;
  if((aux = get_sfr_param_by_name(sfr_tbl, str_op, 0)) != -1) {
    *num |= (unsigned)aux << displacement;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Checks whether it is a valid literal number (signalizes or not):
//-----------------------------------------------------------------------------
bool check_if_valid_literal_num (unsigned *num, char *str_op, 
                                 unsigned displacement) {
  literal_t literal = literal_parser(str_op);
  if(literal.type != IS_STRING && literal.type != IS_INVALID) {
    *num |= literal.num << displacement;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Assembler first pass:
//-----------------------------------------------------------------------------
void pass_one (symbol_dict_t *symbol_tbl, opcode_tbl_t *opcode_tbl, 
               pseudo_inst_dict_t *pseudo_inst_tbl, 
               const char *file_path, file_t *file_in) {
  //int nline = 1, ncol = 1;                 // line and column counter
  int linec = 1;                             // line and column counter
	int ilc   = 0;                             // instruction location counter
	char *line;                                // text line pointer
  char *nlines[3];

  if(file_in == NULL) {
    std::fprintf(stderr, "exiting...\nerror: unable to open file!\n");
    std::exit(EXIT_FAILURE);
  }
	while((line = get_next_line(file_in))) {   // until there lines to be consumed
    nlines[0] = (char *)malloc(sizeof(char)*(strlen(line) + 1));
    nlines[1] = (char *)malloc(sizeof(char)*(strlen(line) + 1));
    nlines[2] = (char *)malloc(sizeof(char)*(strlen(line) + 1));
    if(NULL == nlines[0] || NULL == nlines[1] || NULL == nlines[2]) {
      std::fprintf(stderr, "exiting...\nerror: environment memory allocation problem!\n");
      std::exit(EXIT_FAILURE);
    }
    std::strcpy(nlines[0], line);
    std::strcpy(nlines[1], line);
    std::strcpy(nlines[2], line);
		char *token = std::strtok(line, "\t ");                     // extract token by <tab> or <space>
		while(token) {
			if(token[0] == ';') {                                     // just code comment
				break;
			}
			if(token[0] == '_' && token[strlen(token) - 1] == ':') {  // label
        char *token2 = strtok(nlines[0], "\t ");
        if(token2) {
    			token2 = strtok(NULL, "\t ");
          if(is_valid_instruction(opcode_tbl, token2)) {        // instruction
				    token[strlen(token) - 1] = '\0';
            insert_in_symbol_tbl(symbol_tbl, token, ilc, 0);
            ilc += get_opcode_param_by_name(opcode_tbl, token2, 1);
            break;
          }else if(is_valid_lpseudo_instruction(token2)) {      // "label" pseudo instruction
            if(!std::strcmp(".data", token2)) {
              pseudo_inst::type_t type = pseudo_inst::DATA_SECT_NUM;
              token[strlen(token) - 1] = '\0';
              token2 = strtok(NULL, "\t ");
              //token2 = strtok(NULL, "\t ");
              int nbytes = strtol(token2, NULL, 10);
              insert_in_pseudo_inst_tbl(pseudo_inst_tbl, token, type, nlines[1], ilc, 0, nbytes);
              ilc += nbytes;
              break;
            }else {
              std::fprintf(stderr, "exiting...\nerror: sorry, but at time only "
                "\".data\" pseudo-instruction are recognized!!!\n"
                "check again in: %s at: %d\n", file_path, linec);
              std::fprintf(stderr,"  %s\n", nlines[2]);
              std::exit(EXIT_FAILURE);
            }
          }else {
            std::fprintf(stderr,"exiting...\nerror: invalid "
              "instruction/pseudo-instruction in: %s at: %d\n", file_path, linec);
            std::fprintf(stderr,"  %s\n", nlines[2]);
            std::exit(EXIT_FAILURE);
          }
        }else {
          std::fprintf(stderr, "exiting...\nerror: need a valid "
            "instruction/pseudo-instruction in: %s at: %d\n", file_path, linec);
          std::fprintf(stderr,"  %s\n", nlines[2]);
          std::exit(EXIT_FAILURE);
        }
      }else {
#if 0
        if(get_opcode_param_by_name(opcode_tbl, token, 0) >= 0) {
          ilc += get_opcode_param_by_name(opcode_tbl, token, 1);
        }else {

          std::fprintf(stderr, "#exiting...\nerror: invalid instruction, "
            "pseudo-instruction or label: %s at: %d\n", file_path, linec);
          std::fprintf(stderr,"  %s\n", nlines[2]);
          std::exit(EXIT_FAILURE);
        }
#endif
        if(is_valid_instruction(opcode_tbl, token)) {
          ilc += get_opcode_param_by_name(opcode_tbl, token, 1);
          break;
        }else {
          std::fprintf(stderr, "#exiting...\nerror: invalid instruction, "
            "pseudo-instruction or label: %s at: %d\n", file_path, linec);
          std::fprintf(stderr,"  %s\n", nlines[2]);
          std::exit(EXIT_FAILURE);

        }
      }

			token = std::strtok(NULL, "\t ");
		}
    linec++;

		std::free(line);
    std::free(nlines[0]);
    std::free(nlines[1]);
    std::free(nlines[2]);
	}
}

//-----------------------------------------------------------------------------
// Assembler second pass:
//-----------------------------------------------------------------------------
void pass_two (symbol_dict_t *symbol_tbl, opcode_tbl_t *opcode_tb, sfr_tbl_t *sfr_tbl, 
               pseudo_inst_dict_t *pseudo_inst_tbl, const char *fname_in, 
               file_t *file_in, const char *fname_out) {

  unsigned instruction;
  int opcode, aux, addr_c = 0, linec = 1;
  char *line, *token, *op1, *op2;
  fname_in = fname_in;                    // makes compiler happy!
	std::fseek(file_in, 0, SEEK_SET);
  wbt2_mif_t *wbt2 = wombat2_mif_create(fname_out, "w+", 256, 8, HEX_RADIX, HEX_RADIX);
	while((line = get_next_line(file_in))) {
		token = std::strtok(line, "\t ");
    while(token) {
			if(token[0] == ';') {
				break;
			}

      if((opcode = get_opcode_param_by_name(opcode_tb, token, 0)) != -1) {
        instruction = ((unsigned)(opcode) & 0x1F) << 11;
        switch(opcode) {
          // Instruction Format:
          // 16 bits = 0
          // ------------------------------------------
          // | op: 5 | un: 11                         |
          // ------------------------------------------
          case instruction::STOP:
          case instruction::RETURN:
            break;

          // Instruction Format:
          // Not used 3 bits or 11 bits of addr
          // ------------------------------------------
          // | op: 5 | un: 3    | addr: [7-15]        |
          // ------------------------------------------
          case instruction::JUMP:
          case instruction::CALL:
						op1 = std::strtok(NULL, "\t ");
            if(op1 == NULL) {
              std::fprintf(stderr, "exiting...\nerror: instruction: %s require one numeric literal or a reachable label as operand\n"
                      "check again in: %s at line: %d\n", token, fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_literal_num(&instruction, op1, 0)) {
              if((aux = get_symbol_tbl_address(symbol_tbl, op1)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else if((aux = get_sfr_param_by_name(sfr_tbl, op1, 0)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else if((aux = get_pseudo_inst_tbl_address(pseudo_inst_tbl, op1)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else {
                std::fprintf(stderr, "exiting...\nerror: operand is not a valid numeric literal or reachable label\n"
                        "check again in: %s at line: %d\n", fname_in, linec);
            		std::free(line);
                wombat2_mif_destroy(wbt2);
                return;
              }
            }
            break;

          // Instruction Format:
          // ---------------------------------
          // | op: 5 | un: 9  | addr: [14-15]|
          // ---------------------------------
          case instruction::PUSH:
          case instruction::POP:
						op1 = std::strtok(NULL, "\t ");
            if(op1 == NULL) {
              std::fprintf(stderr, "exiting...\nerror: instruction: %s require one SFR's (A0...A3, IO) as operand\n"
                      "check again in: %s at line: %d\n", token, fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_sfr(sfr_tbl, &instruction, op1, 0)) {
              std::fprintf(stderr, "exiting...\nerror: operand is not a valid SFR (A0...A3, IO)\n"
                      "check again in: %s at line: %d\n", fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }
            break;
 
          // Instruction Format:
          // ------------------------------------------
          // | op: 5 | reg: 2   | addr: 9             |
          // ------------------------------------------
          //case instruction::LOADI: --- Muito Diferentes
          //case instruction::STOREI: --- Muito Diferentes
          case instruction::STORE:
          case instruction::LOAD:
          case instruction::JMPZ:
          case instruction::JMPN:
						op1 = std::strtok(NULL, "\t ");
						op2 = std::strtok(NULL, "\t ");
            if(op1 == NULL || op2 == NULL) {
              std::fprintf(stderr, "exiting...\nerror: instruction: %s require two operands, " 
                      "one SFR (A0...A3, IO) and one numeric literal or reachable label\n"
                      "check again in: %s at line: %d\n", token, fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_sfr(sfr_tbl, &instruction, op1, 9)) {
              std::fprintf(stderr, "exiting...\nerror: operand is not a valid SFR (A0...A3, IO)\n"
                      "check again in: %s at line: %d\n", fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_literal_num(&instruction, op2, 0)) {
              if((aux = get_symbol_tbl_address(symbol_tbl, op2)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else if((aux = get_sfr_param_by_name(sfr_tbl, op2, 0)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else if((aux = get_pseudo_inst_tbl_address(pseudo_inst_tbl, op2)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else {
                std::fprintf(stderr, "exiting...\nerror: operand is not a valid numeric literal or reachable label\n"
                        "check again in: %s at line: %d\n", fname_in, linec);
            		std::free(line);
                wombat2_mif_destroy(wbt2);
                return;
              }
            }
            break;

          // Instruction Format:
          // ------------------------------------------
          // | op: 5 | reg: 2   | sqn: 9              |
          // ------------------------------------------
          case instruction::LOADC:
						op1 = strtok(NULL, "\t ");
						op2 = strtok(NULL, "\t ");
            if(op1 == NULL || op2 == NULL) {
              std::fprintf(stderr, "exiting...\nerror: instruction: %s require two operands, " 
                      "one SFR (A0...A3, IO) and one signed literal\n"
                      "check again in: %s at line: %d\n", token, fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_sfr(sfr_tbl, &instruction, op1, 9)) {
              std::fprintf(stderr, "exiting...\nerror: operand is not a valid SFR (A0...A7, IO)\n"
                      "check again in: %s at line: %d\n", fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_literal_num(&instruction, op2, 0)) {
              if((aux = get_symbol_tbl_address(symbol_tbl, op2)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else if((aux = get_sfr_param_by_name(sfr_tbl, op2, 0)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else if((aux = get_pseudo_inst_tbl_address(pseudo_inst_tbl, op2)) != -1) {
                instruction |= (unsigned)(aux) & 0xFF;
              }else {
                std::fprintf(stderr, "exiting...\nerror: operand is not a valid numeric literal or reachable label\n"
                        "check again in: %s at line: %d\n", fname_in, linec);
            		std::free(line);
                wombat2_mif_destroy(wbt2);
                return;
              }
            }
            break;

          // Instruction Format:
          // ------------------------------------------
          // | op: 5 | reg: 3   | reg: 3   | un: 5    |
          // ------------------------------------------
          case instruction::ADD:
          case instruction::SUBTRACT:
          case instruction::MULTIPLY:
          case instruction::DIVIDE:
          case instruction::MOVE:
						op1 = strtok(NULL, "\t ");
            op2 = strtok(NULL, "\t ");
            if(op1 == NULL || op2 == NULL) {
              std::fprintf(stderr, "exiting...\nerror: instruction: %s require two SFR's (A0...A7, IO) as operands\n"
                      "check again in: %s at line: %d\n", token, fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_sfr(sfr_tbl, &instruction, op1, 8)) {
              std::fprintf(stderr, "exiting...\nerror: first operand is not a valid SFR (A0...A7, IO)\n"
                      "check again in: %s at line: %d\n", fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }

            if(!check_if_valid_sfr(sfr_tbl, &instruction, op2, 5)) {
              std::fprintf(stderr, "exiting...\nerror: second operand is not a valid SFR (A0...A7, IO)\n"
                      "check again in: %s at line: %d\n", fname_in, linec);
          		std::free(line);
              wombat2_mif_destroy(wbt2);
              return;
            }
            break;

          default: 
            break;
        }
        wombat2_mif_write(wbt2, addr_c++, (instruction & 0xFF00) >> 8, NULL);
        wombat2_mif_write(wbt2, addr_c++, instruction & 0xFF, NULL);

			}
			token = strtok(NULL, "\t ");
		}
    linec++;
		std::free(line);
	}

  argv_lst_t argv_lst;
  // process ".data" pseudo instruction:
  get_pseudo_inst_argv_lst(pseudo_inst_tbl, pseudo_inst::DATA_SECT_NUM, argv_lst);
  long long int data;
  unsigned char data_arr[sizeof(long long int)];
  for(argv_lst_t::iterator it = argv_lst.begin(); it != argv_lst.end(); it++) {
    pseudo_inst_tbl_t *pseudo_inst = *it;
    int addr = pseudo_inst->address;
    for(arglst_t::iterator it2 = pseudo_inst->arglst.begin(); 
          it2 != pseudo_inst->arglst.end(); it2++) {
      char *str = (char *)(*it2).c_str();
      int len = std::strlen(str);
      if(is_dec(str, len))
        data = std::strtoll(str, NULL, 10);
      else if(is_hex(str, len))
        data = std::strtoll(str, NULL, 16);
      else if(is_bin(str, len)) {
#if 1
        int idx = 0;
        if((str[0] == '+') | (str[0] == '-')) idx = 3;
        else idx = 2;
        data = strtoll(&str[idx], NULL,  2);
        if(str[0] == '-') {
          data  = ~data;
          data += 1; 
        }
      }
#endif        
//        data = std::strtoll(&str[str[0] == '+' || str[0] == '-' ? 3 : 2], NULL,  2);
//        data = str[0] == '-' ? (~data) + 1 : data;
#if 0
      }else if(is_char(str, len)) {
        data = str[2];
      }
#endif

      for(int i = 0; i < pseudo_inst->nbytes; i++) {
        data_arr[i] = data & 0xFF;
        data >>= 8;
      }

      for(int i = pseudo_inst->nbytes - 1; i >= 0; i--) {
        wombat2_mif_write(wbt2, addr++, data_arr[i], NULL);
      }
    }
    pseudo_inst->arglst.clear();
  }
  argv_lst.clear();
  wombat2_mif_destroy(wbt2);
}

