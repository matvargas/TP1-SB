#ifndef WOMBAT2_PARSER_H
  #define WOMBAT2_PARSER_H

  #include <cstdio>
  #include <cstdlib>
  #include <cstring>
  #include <new>
  #include <string>
  #include <list>
  #include <unordered_map>

  typedef FILE file_t;

  // Get a line from assembly source file stream:
  char *get_next_line (file_t *f);

  // A simple parser for literals:
  bool is_bin (char *str, int size);
  bool is_hex (char *str, int size);
  bool is_dec (char *str, int size);
  bool is_char (char *str, int size);

  // Literal type:
  enum ltype_t {
    IS_BIN = 0,         // is a valid binary number
    IS_HEX,             // is a valid hexadecimal number
    IS_DEC,             // is a valid decimal number
    IS_CHAR,            // is a valid ASCII character
    IS_STRING,          // is a valid ASCII string
    IS_INVALID
  };

  // A structure for literal's
  struct literal_t {
    char *lstr;         // pointer to literal string
    ltype_t type;       // literal type
    long long int num;  // numeric value

  };

  // Checks whether it is a valid literal:
  literal_t literal_parser (char *str);

  // Special Function Registers (SFR) tables:
  namespace sfr {
    // enum opcode_t {
    //   A0 = 0, A1 = 1, A2 = 2, A3 = 3, 
    //   A4 = 4, A5 = 5, A6 = 6, A7 = 7, 
    //   IO = 254      
    // };

    enum opcode_t {
      A0 = 0, 
      A1 = 1, 
      A2 = 2, 
      A3 = 3,
      IO = 254
    };
  
    // static const opcode_t opcode_tbl[] = {
    //   A0, A1, A2, A3, A4, A5, A6, A7, IO      
    // };

    static const opcode_t opcode_tbl[] = {
      A0, A1, A2, A3, IO
    };

    // static const char *name_tbl[] __attribute__((unused)) = {
    //   (char *)"A0", (char *)"A1", (char *)"A2", (char *)"A3",
    //   (char *)"A4", (char *)"A5", (char *)"A6", (char *)"A7",
    //   (char *)"IO"
    // };

    static const char *name_tbl[] __attribute__((unused)) = {
      (char *)"A0", 
      (char *)"A1", 
      (char *)"A2", 
      (char *)"A3",
      (char *)"IO"
    };

    struct info_tbl_t {
      opcode_t opcode;    // register opcode
      int length;         // register length 
    };

    // const info_tbl_t info_tbl[] = {
    //   {A0, 2}, {A1, 2}, {A2, 2}, {A3, 2}, 
    //   {A4, 2}, {A5, 2}, {A6, 2}, {A7, 2}, 
    //   {IO, 2}   
    // };

    const info_tbl_t info_tbl[] = {
      {A0, 2}, 
      {A1, 2}, 
      {A2, 2}, 
      {A3, 2},
      {IO, 2}
    };
  }

  // Instructions tables:
  namespace instruction {
    // enum opcode_t {
    //   EXIT = 0,   LOADI,    STOREI,    ADD,
    //   SUBTRACT,   MULTIPLY, DIVIDE,    JUMP,
    //   JMPZ,       JMPN,     MOVE,      LOAD,
    //   STORE,      LOADC,    CLEAR,     NEGATE,
    //   PUSH,       POP,      ADDI,      CALL, 
    //   RETURN
    // };

    enum opcode_t {
      STOP = 0,  LOAD,     STORE,    READ,    WRITE,
      ADD,       SUBTRACT, MULTIPLY, DIVIDE,
      JUMP,      JMPZ,     JMPN,     MOVE,
      PUSH,      POP,      CALL,     RETURN,
      LOADS,     STORES,   LOADC,    LOADI,
      STOREI,    COPYTOP
   };

    // static const opcode_t opcode_tbl[] = {
    //   EXIT,       LOADI,    STOREI,    ADD,
    //   SUBTRACT,   MULTIPLY, DIVIDE,    JUMP,
    //   JMPZ,       JMPN,     MOVE,      LOAD,
    //   STORE,      LOADC,    CLEAR,     NEGATE,
    //   PUSH,       POP,      ADDI,      CALL, 
    //   RETURN
    // };

    static const opcode_t opcode_tbl[] = {
      STOP,      LOAD,     STORE,    READ,    WRITE,
      ADD,       SUBTRACT, MULTIPLY, DIVIDE,
      JUMP,      JMPZ,     JMPN,     MOVE,
      PUSH,      POP,      CALL,     RETURN,
      LOADS,     STORES,   LOADC,    LOADI,
      STOREI,    COPYTOP
    };

/**
    static const char *name_tbl[] __attribute__((unused)) = {
      (char *)"exit",     (char *)"loadi",    (char *)"storei", (char *)"add",
      (char *)"subtract", (char *)"multiply", (char *)"divide", (char *)"jump",
      (char *)"jmpz",     (char *)"jmpn",     (char *)"move",   (char *)"load",
      (char *)"store",    (char *)"loadc",    (char *)"clear",  (char *)"negate",
      (char *)"push",     (char *)"pop",      (char *)"addi",   (char *)"call",
      (char *)"return"
    };
**/

    static const char *name_tbl[] __attribute__((unused)) = {
      (char *)"stop",     (char *)"load",    (char *)"store", (char *)"read",
      (char *)"write",    (char *)"add",     (char *)"subtract", (char *)"multiply",
      (char *)"divide",   (char *)"jump",    (char *)"jmpz",   (char *)"move",
      (char *)"store",    (char *)"loadc",   (char *)"clear",  (char *)"negate",
      (char *)"push",     (char *)"pop",     (char *)"call",   (char *)"return",
      (char *)"load_s",   (char *)"store_s", (char *)"loadc",  (char *)"loadi",
      (char *)"storei",   (char *)"copytop"
    };

    struct info_tbl_t {
      opcode_t opcode;    // instruction opcode
      int ilength;        // instruction length (in bytes)
      int noper;          // number of operands
    };

    /**
    const info_tbl_t info_tbl[] __attribute__((unused)) = {
      {EXIT,      2,  0}, {LOADI,    2,  2}, {STOREI,  2,  2}, {ADD,     2,  2}, 
      {SUBTRACT,  2,  2}, {MULTIPLY, 2,  2}, {DIVIDE,  2,  2}, {JUMP,    2,  1},	
      {JMPZ,      2,  2}, {JMPN,     2,  2}, {MOVE,    2,  2}, {LOAD,    2,  2},
      {STORE,     2,  2}, {LOADC,    2,  2}, {CLEAR,   2,  1}, {NEGATE,  2,  2},
      {PUSH,      2,  1}, {POP,      2,  1}, {ADDI,    2,  2}, {CALL,    2,  1},	
      {RETURN,    2,  0}
    };
     **/

    const info_tbl_t info_tbl[] __attribute__((unused)) = {
      {STOP,   2,  0}, {LOAD,   2,  2}, {STORE,     2,  2}, {READ,     2,  1},
      {WRITE,  2,  1}, {ADD,    2,  2}, {SUBTRACT,  2,  2}, {MULTIPLY, 2,  2},
      {DIVIDE, 2,  2}, {JUMP,   2,  1}, {JMPZ,      2,  2}, {JMPN,     2,  2},
      {MOVE,   2,  2}, {PUSH,   2,  1}, {POP,       2,  1}, {CALL,     2,  1},
      {RETURN, 2,  0}, {LOADS,  2,  2}, {STORES,    2,  2}, {LOADC,    2,  2},
      {LOADI,  2,  2}, {STOREI, 2,  2}, {COPYTOP,   2,  1}
    };
  }

  // Pseudo-Instructions tables:
  namespace pseudo_inst {
    enum type_t {
      DATA_SECT_ASCII = 0, DATA_SECT_NUM, END_MACRO, EQUATE, GLOBAL, INCLUDE, MACRO 
    };

    static const type_t type_tbl[] = {
      DATA_SECT_ASCII, DATA_SECT_NUM, END_MACRO, EQUATE, GLOBAL, INCLUDE, MACRO 
    };

    static const char *name_tbl[] __attribute__((unused)) = {
      (char *)".ascii", (char *)".data",   (char *)"EQU", 
      (char *)"ENDM",   (char *)".global", (char *)".include", (char *)"MACRO"
    };

  }

  // Predicate do compare C style strings:
  struct equal_to : public std::binary_function<const char *, const char *, bool> {  
    bool operator()(const char *a, const char *b) const  { 
      return(std::strcmp(a, b) == 0); 
    }
  };

  // Predicate to compute a hash key from C style string:
  struct hash_func {
    // Brian Kernighan and Dennis Ritchie's hash
    // http://snipplr.com/view/55639/hashing-function-for-c/
    int operator() (const char *str) const {
      const int seed = 131;//31  131 1313 13131131313 etc//
      int hash = 0;
      while(*str)
        hash = (hash * seed) + (*str++);
      return hash & (0x7FFFFFFF);
    }
  };

  // Instruction table functions:
  typedef std::unordered_map<char *, instruction::info_tbl_t, hash_func, equal_to> opcode_tbl_t;
  opcode_tbl_t *opcode_tbl_create ();
  int get_opcode_param_by_name (opcode_tbl_t *opcode_tbl, const char *token, int p);
  bool is_valid_instruction (opcode_tbl_t *opcode_tbl, const char *token);
  void opcode_tbl_destroy (opcode_tbl_t *opcode_tbl);

  // SFR (Special Function Registers) table functions:
  typedef std::unordered_map<char *, sfr::info_tbl_t, hash_func, equal_to> sfr_tbl_t;
  sfr_tbl_t *sfr_tbl_create ();
  int get_sfr_param_by_name (sfr_tbl_t *sfr_tbl, const char *token, int p);
  void sfr_tbl_destroy (sfr_tbl_t *sfr_tbl);


  // Symbol table functions:
  struct symbol_tbl_t {
    int  address;
    int  file;
  };

  typedef std::unordered_map<std::string, symbol_tbl_t*> symbol_dict_t;
	//typedef std::unordered_map<char const*, symbol_tbl_t *, hash_func, comp_c> symbol_dict_t;
  symbol_dict_t *symbol_tbl_create ();
  void insert_in_symbol_tbl (symbol_dict_t *symbol_dict, const char *label, int address, int file);
  int get_symbol_tbl_address (symbol_dict_t *symbol_dict, const char *label);
  void symbol_tbl_destroy (symbol_dict_t *symbol_dict);

  // Pseudo-instruction table functions:
  typedef std::list<std::string> arglst_t;

  struct pseudo_inst_tbl_t {
    int address;
    int file;
    int nbytes;
    pseudo_inst::type_t type;
    arglst_t arglst;
  };

  typedef std::unordered_map<std::string, pseudo_inst_tbl_t*> pseudo_inst_dict_t;
  typedef std::list<pseudo_inst_tbl_t*> argv_lst_t;
  pseudo_inst_dict_t *pseudo_inst_tbl_create ();
  void insert_in_pseudo_inst_tbl (pseudo_inst_dict_t *pseudo_inst_dict, const char *label, 
                                  pseudo_inst::type_t type, const char *param, int address, int file, int nbytes);
  bool is_valid_pseudo_inst ();
  int get_pseudo_inst_tbl_address (pseudo_inst_dict_t *pseudo_inst_dict, const char *label);
  void get_pseudo_inst_argv_lst (pseudo_inst_dict_t *pseudo_inst_dict, 
                                      pseudo_inst::type_t type, 
                                      argv_lst_t &argv_lst);
  void pseudo_inst_tbl_destroy (pseudo_inst_dict_t *pseudo_inst_dict);

  bool is_valid_lpseudo_instruction (const char *pseudoinst);

#endif // WOMBAT2_PARSER_H
