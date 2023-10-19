/*
    Student ID:     10346186
    Student Name:   Konstantinos Smanis

    Note:
    The instruction to open my individual .trc file is on line 52.
    This file must be saved in the same location as this .c file.
*/



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Metrics
unsigned int external_reads = 0, external_writes = 0;
unsigned int cache_read_hits = 0, cache_read_misses = 0;
unsigned int cache_write_hits = 0, cache_write_misses = 0;

// Controller Mode Setup
unsigned short int controller_mode = 0;
unsigned int block_size, cache_size, num_of_cache_blocks;

// Cache Block Structure
struct Cache_Block {
    unsigned short int valid_bit[256];
    unsigned short int dirty_bit[256];
    unsigned short int tag_bits[256];
    unsigned short int CMBID[256];
}cache_block;

// Other Variables
unsigned short int read_mode, write_mode, char_offset;
unsigned int extracted_address;
char address_buffer[4];
unsigned short int extracted_MMBID, extracted_CMBID, extracted_tag_bits;
bool extracted_is_valid, extracted_tag_bits_match;
int total_reads = 0; int total_writes = 0;


// Function Declarations
void setupController(unsigned short int controller_mode, unsigned int *block_size,unsigned int *cache_size,unsigned int *num_of_cache_blocks);
void setupCacheBlocks(struct Cache_Block *cache_block);
unsigned int extract_address(char address_buffer[4]);
void assessValidity_and_TagBits(struct Cache_Block *cache_block, unsigned short int num_of_cache_blocks, unsigned short int block_ID, unsigned short int extracted_tag_bits, bool *validity, bool *tag_bit_matching);
void updateCacheBlock(struct Cache_Block *cache_block, unsigned short int block_ID, unsigned short int extracted_tag_bits, unsigned short int read_mode, unsigned short int write_mode);

int main(){

    // File Setup
    FILE *fp;
    int access_counter = 0;
    char current_char;
    char file_name[100] = "bubble_sort_trace_074.trc";
    fp = fopen(file_name, "r");

    while(controller_mode < 12){
        //Preprocessing Setup
        setupController(controller_mode, &block_size, &cache_size, &num_of_cache_blocks);
        setupCacheBlocks(&cache_block);
        access_counter = 0;

        // S T A R T   F I L E   P R O C E S S I N G (character by character)
        do{
            // Get next character
            current_char = fgetc(fp);

            // Read or Write ? (works when needed)
            if(current_char == 'W'){
                read_mode = 0;
                write_mode = 1;
                char_offset = 0;
                total_writes++;
            }else if(current_char == 'R'){
                read_mode = 1;
                write_mode = 0;
                char_offset = 0;
                total_reads++;
            }

            // Extract and Analyse new address (works when needed)
            if(current_char == '\n'){

                // Retrieve next character
                extracted_address = extract_address(address_buffer);
                access_counter++;


                // Analyse Extracted Address
                extracted_MMBID = extracted_address / block_size;
                extracted_tag_bits = extracted_MMBID / num_of_cache_blocks;
                extracted_CMBID = extracted_MMBID % num_of_cache_blocks ;


                // Assess Validity and Tag Bit Matching of Extracted address
                assessValidity_and_TagBits(&cache_block, num_of_cache_blocks, extracted_CMBID, extracted_tag_bits, &extracted_is_valid, &extracted_tag_bits_match);

                // Access mode: Read
                if(read_mode){
                    if(extracted_is_valid){
                        if(extracted_tag_bits_match){           // Valid Match
                            cache_read_hits++;
                        }else if(!extracted_tag_bits_match){    // Aliased Match
                            if(cache_block.dirty_bit[extracted_CMBID]){
                                external_writes += block_size;
                            }
                            cache_read_misses++;
                            external_reads += block_size;
                            updateCacheBlock(&cache_block,extracted_CMBID, extracted_tag_bits, read_mode, write_mode);

                        }else{
                            printf("Sooomething's wrong. I can feel it.");
                        }
                    }else if(!extracted_is_valid){
                        cache_read_misses++;
                        external_reads += block_size;
                        updateCacheBlock(&cache_block,extracted_CMBID, extracted_tag_bits, read_mode, write_mode);
                    }else{
                        printf("Sooomething's wrong. I can feel it.");
                    }
                // Access mode: Write
                }else if(write_mode){
                    if(extracted_is_valid){
                        if(extracted_tag_bits_match){           // Valid Match
                            cache_write_hits++;
                            updateCacheBlock(&cache_block,extracted_CMBID, extracted_tag_bits, read_mode, write_mode);
                        }else if(!extracted_tag_bits_match){    // Aliased Match
                            printf("Yes it does.\n");
                            if(cache_block.dirty_bit[extracted_CMBID]){
                                external_writes += block_size;
                            cache_write_misses++;
                            external_reads += block_size;
                            updateCacheBlock(&cache_block,extracted_CMBID, extracted_tag_bits, read_mode, write_mode);
                            }
                        }else{
                            printf("Sooomething's wrong. I can feel it.");
                        }
                    }else if(!extracted_is_valid){
                        cache_write_misses++;
                        external_reads += block_size;
                        updateCacheBlock(&cache_block,extracted_CMBID, extracted_tag_bits, read_mode, write_mode);
                    }else{
                        printf("Sooomething's wrong. I can feel it.");
                    }
                }
            }else if(char_offset > 1){
                // Copy hex character into address buffer
                address_buffer[char_offset - 2] = current_char;
            }
            char_offset++;
        }while (current_char != EOF);
        // E N D   O F   F I L E   P R O C E S S I N G

        // D I S P L A Y   M O D E   R E S U L T S
        printf("bubble_sort_trace_74.trc,%d,%d,%d,%d,%d,%d,%d\n" , (controller_mode+1), external_reads, external_writes, cache_read_hits, cache_read_misses, cache_write_hits, cache_write_misses);

        // R E S E T   F I L E   &   P R E P A R E   N E X T   M O D E
        controller_mode++;
        total_reads = 0;
        total_writes = 0;
        external_reads = 0; external_writes = 0; cache_read_hits = 0; cache_read_misses = 0; cache_write_hits = 0; cache_write_misses = 0;
        rewind(fp);
    }

    // Close File
    fclose(fp);

    return 0;
}


// Function Definitions

void updateCacheBlock(struct Cache_Block *cache_block, unsigned short int block_ID, unsigned short int extracted_tag_bits, unsigned short int read_mode, unsigned short int write_mode){

    // Validate
    cache_block->valid_bit[block_ID] = 1;

    // "Clean" or "Dirty"
    if(read_mode)
        cache_block->dirty_bit[block_ID] = 0;
    else if(write_mode)
        cache_block->dirty_bit[block_ID] = 1;

    // Update Tag Bits
    cache_block->tag_bits[block_ID] = extracted_tag_bits;
}

void assessValidity_and_TagBits(struct Cache_Block *cache_block, unsigned short int num_of_cache_blocks, unsigned short int extracted_CMBID, unsigned short int extracted_tag_bits, bool *extracted_is_valid, bool *extracted_tag_bits_match){

    //Search through cache_block struct
    for(unsigned short int i=0; i<num_of_cache_blocks; i++){
        //Identify relevant cache block from CMBID
        if((cache_block->CMBID[i]) == extracted_CMBID){
            // return validity
            *extracted_is_valid = cache_block->valid_bit[i];
            // return tag bit matching (or lack thereof)
            if(cache_block->tag_bits[i] == extracted_tag_bits){
                *extracted_tag_bits_match = true;
            }else{
                *extracted_tag_bits_match = false;
            }
        }
    }
}

unsigned int extract_address(char address_buffer[4]){

    int address = 0;
    int equivalent_decimal_digit;

    //Converting each hex digit to decimal
    for(int digit = 0; digit < 4; digit++){
        if( (address_buffer[digit] == 'A') || (address_buffer[digit] == 'a'))
            equivalent_decimal_digit = 10;
        else if( (address_buffer[digit] == 'B') || (address_buffer[digit] == 'b'))
            equivalent_decimal_digit = 11;
        else if( (address_buffer[digit] == 'C') || (address_buffer[digit] == 'c'))
            equivalent_decimal_digit = 12;
        else if( (address_buffer[digit] == 'D') || (address_buffer[digit] == 'd'))
            equivalent_decimal_digit = 13;
        else if( (address_buffer[digit] == 'E') || (address_buffer[digit] == 'e'))
            equivalent_decimal_digit = 14;
        else if( (address_buffer[digit] == 'F') || (address_buffer[digit] == 'f'))
            equivalent_decimal_digit = 15;
        else
            equivalent_decimal_digit = (int)(address_buffer[digit]-'0'); //Converts number from char form to int form. For example '3' turns to 3

        // For the decimal value of digit #n, left shifting it by 4*n places is equivalent to *16^n
        address += ((equivalent_decimal_digit) << (4*(3-digit)));
    }

    return address;
}

void setupController(unsigned short int controller_mode, unsigned int *block_size,unsigned int *cache_size,unsigned int *num_of_cache_blocks){
    switch(controller_mode){
            case 0:
                *block_size = 4; *cache_size = 512; *num_of_cache_blocks = 128;
                break;
            case 1:
                *block_size = 8; *cache_size = 512; *num_of_cache_blocks = 64;
                break;
            case 2:
                *block_size = 16; *cache_size = 512; *num_of_cache_blocks = 32;
                break;
            case 3:
                *block_size = 32; *cache_size = 512; *num_of_cache_blocks = 16;
                break;
            case 4:
                *block_size = 64; *cache_size = 512; *num_of_cache_blocks = 8;
                break;
            case 5:
                *block_size = 128; *cache_size = 512; *num_of_cache_blocks = 4;
                break;
            case 6:
                *block_size = 16; *cache_size = 64; *num_of_cache_blocks = 4;
                break;
            case 7:
                *block_size = 16; *cache_size = 128; *num_of_cache_blocks = 8;
                break;
            case 8:
                *block_size = 16; *cache_size = 256; *num_of_cache_blocks = 16;
                break;
            case 9:
                *block_size = 16; *cache_size = 1024; *num_of_cache_blocks = 64;
                break;
            case 10:
                *block_size = 16; *cache_size = 2048; *num_of_cache_blocks = 128;
                break;
            case 11:
                *block_size = 16; *cache_size = 4096; *num_of_cache_blocks = 256;
                break;
            default:
                printf("Sooomething's wrong. I can feel it.");
                break;
        }

}

void setupCacheBlocks(struct Cache_Block *cache_block){
    srand(time(NULL));
    for(int i = 0; i < 256; i++){
        cache_block->valid_bit[i] = 0;          // Initialise as invalid
        cache_block->dirty_bit[i] = rand() % 2; // Initialise randomly
        cache_block->tag_bits[i] = rand();      // Initialise randomly
        cache_block->CMBID[i] = i;              // IDs range from 0 to 255 increasingly. 256 is the maximum number of cache blocks out of all controller modes. So it is enough to cover every case.
    }
}
