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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coff.h"

// Analyze COFF file named filename, extract code labels in label[address], undefined labels in ulabel[address],
// info about each symbol (name, source line, source file index) in source_info,
// list of source files (index, name, file pointer) in s_files, raw data in data[0x2200]
int analyzeCOFF(char *filename,char* label[LMAX],char* ulabel[ULMAX],struct src_i source_info[LMAX],struct srcfile **s_files_p,unsigned short data[0x2200],struct symbol **sym_p, int* nsym)
{
	int i,j,k;
	FILE*f=fopen(filename,"rb");
	if(!f) return 0;
	scnhdr_t* sections;
	filehdr_t FileHeader;
	opthdr_t OptHeader;
	fread((char*)&FileHeader,sizeof(filehdr_t),1,f); //file header
/*	printf("File header:\nmagic %X, sections %d, time %d, symbol ptr %d, nsymbol %d, opt size %d, flags %X\n",\
	FileHeader.f_magic,FileHeader.f_nscns,FileHeader.f_timdat,FileHeader.f_symptr,FileHeader.f_nsyms,FileHeader.f_opthdr,FileHeader.f_flags);*/
	if(FileHeader.f_opthdr==18){	//optional header
		fread((char*)&OptHeader,2,1,f);
		fread((char*)&OptHeader+4,16,1,f);
/*		printf("Optional header:\nmagic %X, version %X, processor %X, rom width %d, data width %d\n",\
		OptHeader.magic,OptHeader.vstamp,OptHeader.proc_type,OptHeader.rom_width_bits,OptHeader.ram_width_bits);*/
	}
	sections=(scnhdr_t*)malloc(sizeof(scnhdr_t)*FileHeader.f_nscns);
	for(i=0;i<FileHeader.f_nscns;i++){
		fread(&sections[i],40,1,f);
	}
	if(fseek(f,FileHeader.f_symptr+FileHeader.f_nsyms*20,SEEK_SET)) printf("seek error\n");
	int strsize=0,strN=0;
	char* strtable,**strings;
	fread((char*)&strsize,4,1,f);
	strtable=malloc(strsize);
	strtable[0]=strtable[1]=strtable[2]=strtable[3]=0;
	fread(&strtable[4],strsize-4,1,f);
	for(i=4;i<strsize;i++) if(strtable[i]==0) strN++;  //count end of string
	strings=malloc(strN*sizeof(char*));
	if(strN) strings[0]=&strtable[4];
	j=1;
	for(i=4;i<strsize&&j<strN;i++){
		if(strtable[i]==0){
			strings[j]=&strtable[i+1];
			j++;
		}
	}
	char* name,null[]="";
	for(i=0;i<FileHeader.f_nscns;i++){
		if(sections[i]._s._s_name[0]) name=sections[i]._s._s_name;
		else if(sections[i]._s._s_s._s_zeroes==0) name=&strtable[sections[i]._s._s_s._s_offset];
		else name=null;
/*		printf("Section %d:\nname \"%s\", Paddr %X, Vaddr %X, size %d\nrawptr %d, relptr %d, lineptr %d, Nreloc %d, Nlines %d, flags %X\n",\
		i,name,sections[i].s_paddr,sections[i].s_vaddr,sections[i].s_size,sections[i].s_scnptr,sections[i].s_relptr,\
		sections[i].s_lnnoptr,sections[i].s_nreloc,sections[i].s_nlnno,sections[i].s_flags);*/
	}
	syment_t symbol;
	for(i=0;i<LMAX;i++) label[i]=0;
	for(i=0;i<ULMAX;i++) ulabel[i]=0;
	int filemax=0;
	*nsym=0;
	if(fseek(f,FileHeader.f_symptr,SEEK_SET)) printf("seek error\n");
	// read all symbols
	for(i=0;i<FileHeader.f_nsyms;i++){
		j=fread((void*)&symbol,  1,14,f);
		j=fread((void*)&symbol+16,1,6,f);
		if(symbol._n._n_name[0]) name=symbol._n._n_name;
		else if(symbol._n._n_n._n_zeroes==0) name=&strtable[symbol._n._n_n._n_offset];
		else name=null;
		if(symbol.n_sclass==6&&symbol.n_value<LMAX){	//code labels
			if(label[symbol.n_value]==0){
				label[symbol.n_value]=malloc(strlen(name)+1);
				strcpy(label[symbol.n_value],name);
			}
			else{
				printf("conflicting labels at address %X: %s vs. %s\n",(unsigned int)symbol.n_value,label[symbol.n_value],name);
/*				char* temp=malloc(strlen(label[symbol.n_value])+strlen(name)+2);
				sprintf(temp,"%s %s",label[symbol.n_value],name);
				free(label[symbol.n_value]);
				label[symbol.n_value]=temp;*/
			}
		}
		//if(symbol.n_sclass==7&&includesym&&symbol.n_value==0x3fff) includesym=0;
		if(symbol.n_sclass==7&&symbol.n_value<ULMAX&&symbol.n_scnum>0&&sections[symbol.n_scnum-1].s_size>0){	//store symbols defined in non-empty sections up to ULMAX
			if(ulabel[symbol.n_value]==0){
				ulabel[symbol.n_value]=malloc(strlen(name)+1);
				strcpy(ulabel[symbol.n_value],name);
			}
			else{
				printf("conflicting labels at address %X: %s vs. %s\n",(unsigned int)symbol.n_value,ulabel[symbol.n_value],name);
/*				char* temp=malloc(strlen(ulabel[symbol.n_value])+strlen(name)+2);
				sprintf(temp,"%s %s",ulabel[symbol.n_value],name);
				free(ulabel[symbol.n_value]);
				ulabel[symbol.n_value]=temp;*/
			}
//			printf("%d %s\n",symbol.n_value,name);
		}
		if(symbol.n_sclass==7){
			//store all symbols in the sym_p array
			(*nsym)++;
			*sym_p=realloc(*sym_p,(*nsym)*sizeof(struct symbol));
			(*sym_p)[*nsym-1].name=strdup(name);
			(*sym_p)[*nsym-1].value=symbol.n_value;
		}
/*		printf("\"%s\", Value %X, Section %d, type %X, Storage class %d, Naux %d, %d\n",\
		name,symbol.n_value,symbol.n_scnum,symbol.n_type,symbol.n_sclass,symbol.n_numaux,i);*/
		// store file info in s_files
		if(symbol.n_sclass==103){	//type C_FILE
			aux_file_t aux;
			fread(&aux,20,1,f);
			symbol.n_numaux--;
//			printf("-> \"%s\", line %d, flags %X\n",&strtable[aux.x_offset],aux.x_incline,aux.x_flags);
			filemax++;
			*s_files_p=realloc(*s_files_p,filemax*sizeof(struct srcfile));
			(*s_files_p)[filemax-1].name=strdup(&strtable[aux.x_offset]);
			(*s_files_p)[filemax-1].ptr=0;
			(*s_files_p)[filemax-1].l_srcndx=i;
			(*s_files_p)[filemax-1].nlines=0;
			(*s_files_p)[filemax-1].lineptr=NULL;
//			printf("%s, idx %d, a %X, i=%d\n",(*s_files_p)[filemax-1].name,(*s_files_p)[filemax-1].l_srcndx,&(*s_files_p)[filemax-1],filemax-1);
			i++;
		}
		i+=symbol.n_numaux;
		for(j=symbol.n_numaux;j;j--)fread(&symbol,20,1,f);
	}
//	printf("filemax=%d a[0]=%X\n",filemax,&s_files[0]);
	// read line info for all sections (line physical address should be unique)
	struct coff_lineno line;
	for(i=0;i<LMAX;i++){
		source_info[i].label=0;
		source_info[i].src_file=0;
		source_info[i].src_line=0;
	}
	for(i=0;i<FileHeader.f_nscns;i++){
//		printf("lines in section %d:\n",i);
		if(fseek(f,sections[i].s_lnnoptr,SEEK_SET)) printf("seek error\n");
		for(j=0;j<sections[i].s_nlnno;j++){
			fread((void*)&line,6,1,f);
			fread((void*)&line+8,6,1,f);
			fread((void*)&line+16,4,1,f);
			if(line.l_paddr<DATA_MAX){
				source_info[line.l_paddr].label=0;
				for(k=0;k<filemax;k++){
					if((*s_files_p)[k].l_srcndx==line.l_srcndx){
						source_info[line.l_paddr].src_file=k;
						if((*s_files_p)[k].ptr==0){
							//printf("file %s (%X), nlines %d, k=%d, a=%X\n",(*s_files_p)[k].name,(*s_files_p)[k].name,(*s_files_p)[k].nlines,k,s_files_p[k]);
							scanSourceFile(&(*s_files_p)[k]);
						}
						k=filemax+10;
					}
				}
				if(k==filemax)source_info[line.l_paddr].src_file=-1; //no source found
				source_info[line.l_paddr].src_line=line.l_lnno;
			}
		}
	}
	unsigned char* raw;
	for(i=0;i<0x2200;i++) data[i]=0xFFFF;
	// read raw data in data[0x2200]
	for(i=0;i<FileHeader.f_nscns;i++){
		if(sections[i].s_size>0){
			if(fseek(f,sections[i].s_scnptr,SEEK_SET)) printf("seek error\n");
			raw=malloc(sections[i].s_size);
			fread(raw,sections[i].s_size,1,f);
			if((sections[i].s_paddr+sections[i].s_size/2)<0x2200)memcpy(&data[sections[i].s_paddr],raw,sections[i].s_size);
			free(raw);
		}
	}
	free(strtable);
	free(strings);
	free(raw);
	return 1;
}

//open a source file and store a pointer for each line
FILE* scanSourceFile(struct srcfile *s_files_p){
//	printf("Scansource  ptr %X,  file %s, nlines %d\n",s_files_p,(*s_files_p).name,(*s_files_p).nlines);
	FILE* f=fopen((*s_files_p).name,"rb");
	char line[4096];
	long int p;
	char* i;
	(*s_files_p).ptr=f;
	if(!f) return f;
	(*s_files_p).nlines=1;		//line # start from index 1
	p=ftell(f);
	for(i=fgets(line,4096,f);i;){
		(*s_files_p).lineptr=realloc((*s_files_p).lineptr,((*s_files_p).nlines+1)*sizeof(long int));
		(*s_files_p).lineptr[(*s_files_p).nlines]=p;
		//printf("nlines%d ptr%d %s",(*s_files_p).nlines,p,line);
		(*s_files_p).nlines++;
		p=ftell(f);
		i=fgets(line,4096,f);
	}
//	printf("file %s, nlines %d\n",(*s_files_p).name,(*s_files_p).nlines);
	return f;
}
