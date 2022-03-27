// COFF file parser
// COFF file structure:
// file header
// optional header
// sections(name, address, size, raw data pointer, reloc entry, ptr to first line, number of lines)
// relocation entries
// symbols (name, value, section, type, class) + optional info
// lines (source file symbol, line number, address of code)
// symbol table
// string table

#define LMAX 0x2200
#define DATA_MAX 0x2200
#define ULMAX 0x400

typedef struct filehdr
{
    unsigned short f_magic;
    unsigned short f_nscns;
    unsigned long f_timdat;
    unsigned long f_symptr;
    unsigned long f_nsyms;
    unsigned short f_opthdr;
    unsigned short f_flags;
} filehdr_t;

typedef struct opthdr
{
    unsigned short magic;
    unsigned long vstamp;
    unsigned long proc_type;
    unsigned long rom_width_bits;
    unsigned long ram_width_bits;
} opthdr_t;

typedef struct scnhdr
{
    union
    {
      char _s_name[8]; /* section name is a string */
      struct
        {
          unsigned long _s_zeroes;
          unsigned long _s_offset;
        }_s_s;
    }_s;
	unsigned long s_paddr;
    unsigned long s_vaddr;
    unsigned long s_size;
    unsigned long s_scnptr;
    unsigned long s_relptr;
    unsigned long s_lnnoptr;
    unsigned short s_nreloc;
    unsigned short s_nlnno;
    unsigned long s_flags;
} scnhdr_t;

typedef struct reloc
{
    unsigned long r_vaddr;
    unsigned long r_symndx;
    short r_offset;
    unsigned short r_type;
} reloc_t;

#define SYMNMLEN 8
typedef struct syment
{
    union
    {
      char _n_name[SYMNMLEN];
      struct
      {
        unsigned long _n_zeroes;
        unsigned long _n_offset;
      } _n_n;
      char *_n_nptr[2];
    } _n;
    unsigned long n_value;
    short n_scnum;
    unsigned long n_type;
    char n_sclass;
    unsigned char n_numaux;
} syment_t;

typedef struct coff_lineno
{
    unsigned long l_srcndx;
    unsigned short l_lnno;
    unsigned long l_paddr;
    unsigned short l_flags;
    unsigned long l_fcnndx;
} coff_lineno_t;

typedef struct aux_file
{
    unsigned long x_offset;
    unsigned long x_incline;
    unsigned char x_flags;
    char _unused[11];
} aux_file_t;

typedef struct aux_scn
{
    unsigned long x_scnlen;
    unsigned short x_nreloc;
    unsigned short x_nlinno;
    char _unused[12];
} aux_scn_t;

typedef struct aux_tag
{
    char _unused[6];
    unsigned short x_size;
    char _unused2[4];
    unsigned long x_endndx;
    char _unused3[4];
} aux_tag_t;

typedef struct aux_eos
{
    unsigned long x_tagndx;
    char _unused[2];
    unsigned short x_size;
    char _unused2[12];
} aux_eos_t;

typedef struct aux_fcn
{
    unsigned long x_tagndx;
    unsigned long x_size;
    unsigned long x_lnnoptr;
    unsigned long x_endndx;
    short x_actscnum;
    char _unused[2];
} aux_fcn_t;

typedef struct aux_fcn_calls
{
    unsigned long x_calleendx;
    unsigned long x_is_interrupt;
    char _unused[12];
} aux_fcn_calls_t;

#define X_DIMNUM 4
typedef struct aux_arr
{
    unsigned long x_tagndx;
    unsigned short x_lnno;
    unsigned short x_size;
    unsigned short x_dimen[X_DIMNUM];
    char _unused[4];
} aux_arr_t;

typedef struct aux_eobf
{
    char _unused[4];
    unsigned short x_lnno;
    char _unused2[14];
} aux_eobf_t;

typedef struct aux_bobf
{
    char _unused[4];
    unsigned short x_lnno;
    char _unused2[6];
    unsigned long x_endndx;
    char _unused3[4];
} aux_bobf_t;

typedef struct aux_var
{
    unsigned long x_tagndx;
    char _unused[2];
    unsigned short x_size;
    char _unused2[12];
} aux_var_t;

typedef struct aux_field
{
    char _unused[6];
    unsigned short x_size;
    char _unused2[12];
} aux_field_t;

struct src_i
{
	char* label;
	int src_file;
	int src_line;
};

struct srcfile {
	unsigned long l_srcndx;
	char* name;
	FILE* ptr;
	int nlines;
	long int *lineptr;	//line pointer array
};

struct symbol {
	char* name;
	int value;
};

FILE* scanSourceFile(struct srcfile *s_files_p);
int analyzeCOFF(char *filename,char* label[LMAX],char* ulabel[ULMAX],struct src_i source_info[LMAX],\
					struct srcfile **s_files_p,unsigned short data[0x2200],struct symbol **sym_p, int* nsym);
