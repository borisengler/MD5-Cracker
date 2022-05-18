#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> 
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "md5.h"

#define UNUSED(VAR) ((void) (VAR))

typedef struct configuration {
    bool try_changes;
    char *file;
    char *hash;
} configuration;

// prepares mask for transormations
long long unsigned int find_combinations(char *string) {
    long long unsigned int amount = 1;
    int i = 0;
    while (string[i] != '\0') {
        switch (string[i]) {
            case 'A':
            case 'a':
            case 'S':
            case 's':
                amount *= 4;
                break;
            case 'B':
            case 'b':
            case 'E':
            case 'e':
            case 'I':
            case 'i':
            case 'L':
            case 'l':
            case 'O':
            case 'o':
            case 'T':
            case 't':
                amount *= 2;
                break;
        }
    i++;
    }
    return amount;
}

void apply_mask(char *string, char *string2, long long unsigned int mask) {
    int i = 0;
    long long unsigned int local_mask = mask;
    while (string[i] != '\0' && i < 30) {
        switch (string[i]) {
            case 'A':
            case 'a':
                if ((local_mask & 1) == 1) {
                    string2[i] = '@';
                }
                else if ((local_mask & 2) == 2) {
                    string2[i] = '4';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 2;
                break;
            case 'S':
            case 's':
                if ((local_mask & 1) == 1) {
                    string2[i] = '$';
                }
                else if ((local_mask & 2) == 2) {
                    string2[i] = '5';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 2;
                break;
            case 'B':
            case 'b':
                if ((local_mask & 1) == 1) {
                    string2[i] = '8';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 1;
                break;
            case 'E':
            case 'e':
                if ((local_mask & 1) == 1) {
                    string2[i] = '3';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 1;
                break;
            case 'I':
            case 'i':
                if ((local_mask & 1) == 1) {
                    string2[i] = '!';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 1;
                break;
            case 'L':
            case 'l':
                if ((local_mask & 1) == 1) {
                    string2[i] = '1';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 1;
                break;
            case 'O':
            case 'o':
                if ((local_mask & 1) == 1) {
                    string2[i] = '0';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 1;
                break;
            case 'T':
            case 't':
                if ((local_mask & 1) == 1) {
                    string2[i] = '7';
                }
                else {
                    string2[i] = string[i];
                }
                local_mask >>= 1;
                break;
            case '\0':
                return;
        }
    i++;
    }
}

bool parse_args(int argc, char **argv, configuration *config)
{
    if (argc < 3) {
        fprintf(stderr, "Use format ./cracker [OPTIONS] FILE HASH\n");
        return false;
    }

    // switches
    int switches = 0;
    if (argv[1][0] == '-') {
        if (strcmp(argv[1], "-t") == 0) {
            config->try_changes = true;
            switches++;
        }
        else {
            fprintf(stderr, "Unknown switch %s\n", argv[1]);
            return false;
        }
    }

    if (argc < 3 + switches) {
        fprintf(stderr, "Use format ./cracker [OPTIONS] FILE HASH\n");
        return false;
    }

    // file
    config->file = argv[1 + switches];
    
    // hash
    config->hash = argv[2 + switches];
    if (strlen(config->hash) != 32) {
        fprintf(stderr, "HASH must be 32 characters long\n");
        return false;
    }
    for (int i = 0; i < 32; i++) {
        if (config->hash[i] < 48 || (config->hash[i] > 57 && config->hash[i] < 65)
        || (config->hash[i] > 70 && config->hash[i] < 97) || config->hash[i] > 102) {
            fprintf(stderr, "HASH contains invalid characters\n");
            return false;
        }
    }
    if (argc > switches + 3) {
        fprintf(stderr, "Too many parameters\n");
        return false;
    }
    return true;
}

int read_line(FILE *file, char *buffer) {
    char ch;
    int i = 0;
    while ((ch = fgetc(file)) != '\n') {
        if (ch == EOF) {
            *(buffer + i) = '\0';
            return 1;
        }
        *(buffer + i) = ch;
        i++;
    }
    *(buffer + i) = '\0';
    return 0;
}

void string_to_hexadecimal(char *string[32], unsigned char *hex_bytes) {
    for (int i = 0; i < 16; i++) {
        unsigned char hexadecimal = 0;
        for (int j = 0; j < 2; j++) {
            switch ((*string)[(2*i)+j]) {
                case 'A':
                case 'a':
                    hexadecimal += 10 * (16 >> 4 * (j));
                    break;
                case 'B':
                case 'b':
                    hexadecimal += 11 * (16 >> 4 * (j));
                    break;
                case 'C':
                case 'c':
                    hexadecimal += 12 * (16 >> 4 * (j));
                    break;
                case 'D':
                case 'd':
                    hexadecimal += 13 * (16 >> 4 * (j));
                    break;
                case 'E':
                case 'e':
                    hexadecimal += 14 * (16 >> 4 * (j));
                    break;
                case 'F':
                case 'f':
                    hexadecimal += 15 * (16 >> 4 * (j));
                    break;
                default:
                    hexadecimal += ((*string)[2*i+j] - '0') * (16 >> 4 * (j));
                    break;
            }
        }
        hex_bytes[i] = (unsigned char)hexadecimal;
    }

}

int main(int argc, char **argv) {
    configuration config = {false, NULL, NULL};
    if (!parse_args(argc, argv, &config)) {
        return 1;
    }

    char *string = malloc(512);
    char *string2 = calloc(512, 1); // used for combinations of characters later
    unsigned char md5_hash[16];
    unsigned char *input_hash = malloc(16);

    string_to_hexadecimal(&config.hash, input_hash);
    MD5_CTX md5_ctx;
    int found = 0;
    FILE *file = fopen(config.file, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", config.file);
        free(string);
        free(string2);
        free(input_hash);
        return 1;
    }

    while (!found) {
        int found_end;
        found_end = read_line(file, string);
        if (found_end) {
            if (strlen(string) == 0) {
                printf("password not found\n");
                break;
            }
        }
        long long unsigned int combinations = 0;
        if (config.try_changes) {
            combinations = find_combinations(string);            
        }
        strncpy(string2, string, strlen(string) + 1);

        int count = 0;
        for (long long unsigned int i = 0; i <= combinations; i++) {
            count++;
            apply_mask(string, string2, i);

            MD5_Init(&md5_ctx);
            MD5_Update(&md5_ctx, string2, strlen(string2));
            MD5_Final(md5_hash, &md5_ctx);

            if (memcmp(md5_hash, input_hash, sizeof(md5_hash)) == 0) {
                found++;
                printf("password found\n%s\n", string2);
                free(string);
                free(string2);
                free(input_hash);
                fclose(file);
                return 0;
            }
            if (!found && found_end) {
                printf("password not found\n");
                break;
            }

        }
    }
    free(string);
    free(string2);
    free(input_hash);
    fclose(file);
    return 0;
}