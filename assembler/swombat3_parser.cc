#include <cstdlib>
#include <cctype>
#include <cstring>
#include "swombat3_parser.h"

//-----------------------------------------------------------------------------
// Checks whether it is a valid binary number:
//-----------------------------------------------------------------------------
bool is_bin (char *str, int size) {
  int idx = 0;
  if(str[0] == '+' || str[0] == '-') {
    if(size > 3) idx = 1;
    else return false;
  }else if(size <= 2) return false;
  if(str[idx] == '0' && str[idx + 1] == 'b') {
    for(int i = idx + 2; i < size; i++) if(str[i] != '0' && str[i] != '1') return false;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Checks whether it is a valid hexadecial number:
//-----------------------------------------------------------------------------
bool is_hex (char *str, int size) {
  int idx = 0;
  if(str[0] == '+' || str[0] == '-') {
    if(size > 3) idx = 1;
    else return false;
  }else if(size <= 2) return false;
  if(str[idx] == '0' && str[idx + 1] == 'x') {
    for(int i = idx + 2; i < size; i++) if(!std::isxdigit(str[i])) return false;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Checks whether it is a valid decimal number:
//-----------------------------------------------------------------------------
bool is_dec (char *str, int size) {
  int idx = 0;
  if(size > 1) {
    if(str[0] == '+' || str[0] == '-') idx = 1;
    for(int i = idx; i < size; i++) if(!std::isdigit(str[i])) return false;
    return true;
  }else if(size == 1 && std::isdigit(str[0])) return true;

  return false;
}

//-----------------------------------------------------------------------------
// Checks whether it is a valid (printable) ASCII character:
//-----------------------------------------------------------------------------
bool is_char (char *str, int size) {
  if(size == 3 && str[0] == '\'' && str[2] == '\'' &&
     (unsigned char)str[1] >= 0x20 && (unsigned char)str[1] <= 0x79)
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// Parser for literals:
//-----------------------------------------------------------------------------
literal_t literal_parser (char *str) {
  int len = (int)std::strlen(str);
  literal_t literal;
  if(len == 3 && str[0] == '\'' && str[2] == '\'' &&
                 str[1] >= 0x20 && str[1] <= 0x79) {    // is a single ASCII char ?
    literal.type = IS_CHAR;
    literal.num  = str[1];
  }else if(str[0] == '"') {                             // is a valid string ? 
    literal.type = IS_STRING;
    std::fprintf(stderr, "exiting...\nerror: strings not allowed as literal!\n");
    std::exit(EXIT_FAILURE);
  }else {                                               // is a valid number ?
    if(is_bin(str, len)) {                              // is a binary number ?
      literal.type = IS_BIN;
      int idx = 0;
      if(str[0] == '+' || str[0] == '-') idx = 3;
      else idx = 2;
      literal.num = std::strtol(&str[idx], NULL,  2);
      if(str[0] == '-') {                               // if neg, take two complement
        literal.num  = ~literal.num;
        literal.num += 1; 
      }
    }else if(is_hex(str, len)) {                        // is a hex number ?
      literal.num = std::strtol(str, NULL, 16) & 0xFF;
    }else if(is_dec(str, len)) {                        // is a dec number ?
      literal.num = std::strtol(str, NULL, 10) & 0xFF;
    }else {                                             // i don't know what is it!
      literal.type = IS_INVALID;
    }
  }
  return literal;
}

//-----------------------------------------------------------------------------
// Get the next stream line:
//-----------------------------------------------------------------------------
char *get_next_line (file_t *f) {
	int count = 0, max = 80; // char's counter and max number of char's into a line
	char *line = (char *)std::malloc(max*sizeof(char));
	char c;
	while(true) {
		c = std::fgetc(f);
		if(c == EOF || c == '\n') {
			break;
		}

    if(c == '\r') {               // Windows OS breakline
      std::fgetc(f);
      break;
    }
    if(count == (max - 1)) {      // If is not enough, realloc 
      line = (char *)std::realloc(line, (2*max)*sizeof(char));
      max = 2*max;
    }
    line[count] = c;
    count++;
	}
	line[count] = '\0';
	if(c == EOF && count == 0){
		std::free(line);
		return NULL;
	}
	return line;
}

//-----------------------------------------------------------------------------
// Create and initialize opcode table:
//-----------------------------------------------------------------------------
opcode_tbl_t *opcode_tbl_create () {
  opcode_tbl_t *opcode = new opcode_tbl_t();
  auto it = std::begin(instruction::opcode_tbl);
  int i = 0;
  while(it != std::end(instruction::opcode_tbl)) {
    (*opcode)[(char *)(instruction::name_tbl[i])] = instruction::info_tbl[i];
    it++;
    i++;
  }
  return opcode;
}

//-----------------------------------------------------------------------------
// Check if its a valid instruction:
//-----------------------------------------------------------------------------
bool is_valid_instruction (opcode_tbl_t *opcode_tbl, const char *token) {
  return opcode_tbl->find((char *)token) != opcode_tbl->end() ? true : false;
}

//-----------------------------------------------------------------------------
// Get instruction opcode info by name:
//-----------------------------------------------------------------------------
int get_opcode_param_by_name (opcode_tbl_t *opcode_tbl, const char *token, int p) {
  opcode_tbl_t::const_iterator it = opcode_tbl->find((char *)token);
  if(it != opcode_tbl->end()) {
    instruction::info_tbl_t info = it->second;
    if(p == 0)      return info.opcode;
    else if(p == 1) return info.ilength;
    else if(p == 2) return info.noper;
    else return -1;
  }
  return -1;
}

//-----------------------------------------------------------------------------
// Destroy opcode table:
//-----------------------------------------------------------------------------
void opcode_tbl_destroy(opcode_tbl_t *opcode_tbl) {
  delete opcode_tbl;
}

//-----------------------------------------------------------------------------
// Create and initialize SFR's (Special Function Registers) symbol table:
//-----------------------------------------------------------------------------
sfr_tbl_t *sfr_tbl_create () {
  sfr_tbl_t *sfr_tbl = new sfr_tbl_t();
  auto it = std::begin(sfr::opcode_tbl);
  int i = 0;
  while(it != std::end(sfr::opcode_tbl)) {
    (*sfr_tbl)[(char *)(sfr::name_tbl[i])] = sfr::info_tbl[i];
    it++;
    i++;
  }
  return sfr_tbl;
}

//-----------------------------------------------------------------------------
// Get instruction opcode info by name:
//-----------------------------------------------------------------------------
int get_sfr_param_by_name (sfr_tbl_t *sfr_tbl, const char *token, int p) {
  sfr_tbl_t::const_iterator it = sfr_tbl->find((char *)token);
  if(it != sfr_tbl->end()) {
    sfr::info_tbl_t info = it->second;
    if(p == 0)      return info.opcode;
    else if(p == 1) return info.length;
    else return -1;
  }
  return -1;
}

//-----------------------------------------------------------------------------
// Destroy SFR's (Special Function Registers) table:
//-----------------------------------------------------------------------------
void sfr_tbl_destroy (sfr_tbl_t *sfr_tbl) {
  delete sfr_tbl;
}

//-----------------------------------------------------------------------------
// Create symbol table:
//-----------------------------------------------------------------------------
symbol_dict_t *symbol_tbl_create () {
  symbol_dict_t *symbol_dict = new symbol_dict_t();
  return symbol_dict;
}

//-----------------------------------------------------------------------------
// Insert symbol in the table:
//-----------------------------------------------------------------------------
void insert_in_symbol_tbl (symbol_dict_t *symbol_dict, 
                           const char *label, int address, int file) {
  symbol_tbl_t *symbol = new symbol_tbl_t();
  symbol->address = address;
  symbol->file    = file;
  std::string s(label);
  (*symbol_dict)[s] = symbol;
}

//-----------------------------------------------------------------------------
// Get address symbol in the table:
//-----------------------------------------------------------------------------
int get_symbol_tbl_address (symbol_dict_t *symbol_dict, const char *label) {
  std::string s(label);
  symbol_dict_t::const_iterator it = symbol_dict->find(s);
  if(it != symbol_dict->end()) {
    symbol_tbl_t *symbol = it->second;
    return symbol->address;
  }
  return -1;
}

//-----------------------------------------------------------------------------
// Destroy symbol table:
//-----------------------------------------------------------------------------
void symbol_tbl_destroy (symbol_dict_t *symbol_dict) {
  symbol_dict_t::const_iterator it = symbol_dict->begin();
  while(it != symbol_dict->end()) {
    symbol_tbl_t *symbol = it->second;
    delete symbol;
    it++;
  }
  delete symbol_dict;
}

//-----------------------------------------------------------------------------
// Create a pseudo-instruction table:
//-----------------------------------------------------------------------------
pseudo_inst_dict_t *pseudo_inst_tbl_create () {
  pseudo_inst_dict_t *pseudo_inst_dict = new pseudo_inst_dict_t();
  return pseudo_inst_dict;
}

//-----------------------------------------------------------------------------
// Insert a pseudo-instruction data into a table:
//-----------------------------------------------------------------------------
void insert_in_pseudo_inst_tbl (pseudo_inst_dict_t *pseudo_inst_dict, const char *label, 
                                pseudo_inst::type_t type, const char *param, int address, 
                                int file, int nbytes) {
  pseudo_inst_tbl_t *pseudo_inst_tbl = new pseudo_inst_tbl_t();
  pseudo_inst_tbl->address = address;
  pseudo_inst_tbl->file    = file;
  pseudo_inst_tbl->nbytes  = nbytes;
  pseudo_inst_tbl->type    = type;
  // TODO: tokenize param argument and put each one into the list:
  pseudo_inst_tbl->arglst.clear();
  char *token = std::strtok((char *)param, "\t ;");
  token = std::strtok(NULL, "\t ;");
  token = std::strtok(NULL, "\t ;");
  token = std::strtok(NULL, "\t ;");
  //  while(token) {
  //std::string token_str(token);
  std::string token_str(token);
  pseudo_inst_tbl->arglst.push_back(token_str);
  //token = std::strtok(NULL, "\t ");
  //  }
  std::string label_str(label);
  (*pseudo_inst_dict)[label_str] = pseudo_inst_tbl;
}

//-----------------------------------------------------------------------------
// Get an address from pseudo-instruction table:
//-----------------------------------------------------------------------------
int get_pseudo_inst_tbl_address (pseudo_inst_dict_t *pseudo_inst_dict, const char *label) {
  std::string str(label);
  pseudo_inst_dict_t::const_iterator it = pseudo_inst_dict->find(str);
  if(it != pseudo_inst_dict->end()) {
    pseudo_inst_tbl_t *pseudo_inst = it->second;
    return pseudo_inst->address;
  }
  return -1;
}

//-----------------------------------------------------------------------------
// A predicate to compare address stored into a list container:
//-----------------------------------------------------------------------------
static bool cmp_argv_lst (pseudo_inst_tbl_t const *a, pseudo_inst_tbl_t const *b) {
  return(a->address < b->address);
}

//-----------------------------------------------------------------------------
// Get and sort an argument list of pseudo-instructions data:
//-----------------------------------------------------------------------------
void get_pseudo_inst_argv_lst (pseudo_inst_dict_t *pseudo_inst_dict, 
                               pseudo_inst::type_t type, 
                               argv_lst_t &argv_lst) {

  for(auto &kv : *pseudo_inst_dict) {
    pseudo_inst_tbl_t *pseudo_inst = kv.second;
    if(pseudo_inst->type == type) {
      argv_lst.push_back(pseudo_inst);
    }
  }
  argv_lst.sort(cmp_argv_lst);
}

//-----------------------------------------------------------------------------
// Destroy pseudo-instruction table:
//-----------------------------------------------------------------------------
void pseudo_inst_tbl_destroy (pseudo_inst_dict_t *pseudo_inst_dict) {
  pseudo_inst_dict_t::const_iterator it = pseudo_inst_dict->begin();
  for(auto &kv : *pseudo_inst_dict) {
    pseudo_inst_tbl_t *pseudo_inst = kv.second;
		pseudo_inst->arglst.clear();
		delete pseudo_inst;
  }
  delete pseudo_inst_dict;
}

//-----------------------------------------------------------------------------
// Check if is a valid pseudo-instruction preceeded by a label:
//-----------------------------------------------------------------------------
bool is_valid_lpseudo_instruction (const char *pseudo_inst) {
  for(int i = 0; i < 3; i++)
    if(!strcmp(pseudo_inst, pseudo_inst::name_tbl[i])) 
      return true;
  return false;
}

